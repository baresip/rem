/**
 * @file rem_auringbuf.h Audio Circular Buffer
 */

struct auringbuf;

int  auringbuf_alloc(struct auringbuf **abp, size_t min_sz, size_t max_sz);
int  auringbuf_append(struct auringbuf *ab, struct mbuf *mb);
int  auringbuf_write(struct auringbuf *ab, const uint8_t *p, size_t sz);
void auringbuf_read(struct auringbuf *ab, uint8_t *p, size_t sz);
int  auringbuf_get(struct auringbuf *ab, uint32_t ptime,
		   uint8_t *p, size_t sz);
void auringbuf_flush(struct auringbuf *ab);
int  auringbuf_debug(struct re_printf *pf, const struct auringbuf *ab);
size_t auringbuf_cur_size(const struct auringbuf *ab);


static inline int auringbuf_write_samp(struct auringbuf *ab,
				const int16_t *sampv, size_t sampc)
{
	return auringbuf_write(ab, (const uint8_t *)sampv, sampc * 2);
}


static inline void auringbuf_read_samp(struct auringbuf *ab, int16_t *sampv,
				size_t sampc)
{
	auringbuf_read(ab, (uint8_t *)sampv, sampc * 2);
}


static inline int auringbuf_get_samp(struct auringbuf *ab, uint32_t ptime,
				int16_t *sampv, size_t sampc)
{
	return auringbuf_get(ab, ptime, (uint8_t *)sampv, sampc * 2);
}
