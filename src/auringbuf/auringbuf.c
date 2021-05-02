/**
 * @file auringbuf.c  Audio Circular Buffer
 */
#include <string.h>
#include <re.h>
#include <rem_auringbuf.h>


#define AURINGBUF_DEBUG 0


/** Locked audio-buffer with almost zero-copy */
struct auringbuf {
	struct lock *lock;
	size_t wish_sz;
	size_t cur_sz;
	size_t max_sz;
	size_t pos_write;
	size_t pos_read;
	struct mbuf *mb;
	uint64_t ts;

#if AURINGBUF_DEBUG
	struct {
		size_t or;
		size_t ur;
	} stats;
#endif
};


static void auringbuf_destructor(void *arg)
{
	struct auringbuf *ab = arg;

	mem_deref(ab->lock);
	mem_deref(ab->mb);
}


/**
 * Allocate a new audio buffer
 *
 * @param abp    Pointer to allocated audio buffer
 * @param min_sz Minimum buffer size
 * @param max_sz Maximum buffer size
 *
 * @return 0 for success, otherwise error code
 */
int auringbuf_alloc(struct auringbuf **abp, size_t min_sz, size_t max_sz)
{
	struct auringbuf *ab;
	int err;

	if (!abp || !min_sz || !max_sz)
		return EINVAL;

	ab = mem_zalloc(sizeof(*ab), auringbuf_destructor);
	if (!ab)
		return ENOMEM;

	err = lock_alloc(&ab->lock);
	if (err)
		goto out;

	ab->wish_sz = min_sz;
	ab->max_sz = max_sz;
	ab->pos_write = 0;
	ab->pos_read = 0;
	ab->mb = mbuf_alloc(max_sz);

 out:
	if (err)
		mem_deref(ab);
	else
		*abp = ab;

	return err;
}


/**
 * Write PCM samples to the audio buffer
 *
 * @param ab Audio buffer
 * @param p  Pointer to PCM data
 * @param sz Number of bytes to write
 *
 * @return 0 for success, otherwise error code
 */
int auringbuf_write(struct auringbuf *ab, const uint8_t *p, size_t sz)
{
	int err = 0;
	size_t diff;

	if (!ab || !p || !sz)
		return EINVAL;

	lock_write_get(ab->lock);

	if ((ab->cur_sz + sz) > ab->max_sz) {
#if AURINGBUF_DEBUG
		++ab->stats.or;
		(void)re_printf("auringbuf: %p overrun (cur=%zu)\n",
				ab, ab->cur_sz);
#endif
		goto out;
	}

	diff = ab->max_sz - ab->pos_write;
	if (diff < sz) {
		ab->mb->pos = ab->pos_write;
		err = mbuf_write_mem(ab->mb, p, diff);

		ab->mb->pos = 0;
		err = mbuf_write_mem(ab->mb, p + diff, (sz - diff));

		ab->pos_write = sz - diff;
		ab->cur_sz += sz;
		goto out;
	}

	ab->mb->pos = ab->pos_write;
	err = mbuf_write_mem(ab->mb, p, sz);

	ab->pos_write += sz;
	ab->cur_sz += sz;

out:
	lock_rel(ab->lock);

	return err;
}


/**
 * Read PCM samples from the audio buffer. If there is not enough data
 * in the audio buffer, silence will be read.
 *
 * @param ab Audio buffer
 * @param p  Buffer where PCM samples are read into
 * @param sz Number of bytes to read
 */
void auringbuf_read(struct auringbuf *ab, uint8_t *p, size_t sz)
{
	if (!ab || !p || !sz)
		return;

	lock_write_get(ab->lock);

	if (ab->cur_sz < sz) {
#if AURINGBUF_DEBUG
		(void)re_printf("auringbuf underrun: %ld\n",
				sz);
#endif
		memset(p, 0, sz);
		goto out;
	}

	ab->mb->pos = ab->pos_read;
	if ((ab->pos_read + sz) > ab->max_sz) {
		size_t left = ab->max_sz - ab->pos_read;
		(void)mbuf_read_mem(ab->mb, p, left);
		ab->mb->pos = 0;
		(void)mbuf_read_mem(ab->mb, p + left, (sz - left));
		ab->pos_read = sz - left;
	}
	else {
		(void)mbuf_read_mem(ab->mb, p, sz);
		ab->pos_read += sz;
	}

	ab->cur_sz -= sz;


 out:
	lock_rel(ab->lock);
}


/**
 * Timed read PCM samples from the audio buffer. If there is not enough data
 * in the audio buffer, silence will be read.
 *
 * @param ab    Audio buffer
 * @param ptime Packet time in [ms]
 * @param p     Buffer where PCM samples are read into
 * @param sz    Number of bytes to read
 *
 * @note This does the same as auringbuf_read() except that it also takes
 *       timing into consideration.
 *
 * @return 0 if valid PCM was read, ETIMEDOUT if no PCM is ready yet
 */
int auringbuf_get(struct auringbuf *ab, uint32_t ptime, uint8_t *p, size_t sz)
{
	uint64_t now;
	int err = 0;

	if (!ab || !ptime)
		return EINVAL;

	lock_write_get(ab->lock);

	now = tmr_jiffies();
	if (!ab->ts)
		ab->ts = now;

	if (now < ab->ts) {
		err = ETIMEDOUT;
		goto out;
	}

	ab->ts += ptime;

 out:
	lock_rel(ab->lock);

	if (!err)
		auringbuf_read(ab, p, sz);

	return err;
}


/**
 * Flush the audio buffer
 *
 * @param ab Audio buffer
 */
void auringbuf_flush(struct auringbuf *ab)
{
	if (!ab)
		return;

	lock_write_get(ab->lock);

	ab->cur_sz  = 0;
	ab->ts      = 0;
	ab->pos_read = 0;
	ab->pos_write = 0;

	lock_rel(ab->lock);
}


/**
 * Audio buffer debug handler, use with fmt %H
 *
 * @param pf Print function
 * @param ab Audio buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int auringbuf_debug(struct re_printf *pf, const struct auringbuf *ab)
{
	int err;

	if (!ab)
		return 0;

	lock_read_get(ab->lock);
	err = re_hprintf(pf, "wish_sz=%zu cur_sz=%zu",
			 ab->wish_sz, ab->cur_sz);

#if AURINGBUF_DEBUG
	err |= re_hprintf(pf, " [overrun=%zu underrun=%zu]",
			  ab->stats.or, ab->stats.ur);
#endif

	lock_rel(ab->lock);

	return err;
}


/**
 * Get the current number of bytes in the audio buffer
 *
 * @param ab Audio buffer
 *
 * @return Number of bytes in the audio buffer
 */
size_t auringbuf_cur_size(const struct auringbuf *ab)
{
	size_t sz;

	if (!ab)
		return 0;

	lock_read_get(ab->lock);
	sz = ab->cur_sz;
	lock_rel(ab->lock);

	return sz;
}
