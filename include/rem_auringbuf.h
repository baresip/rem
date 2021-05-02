/**
 * @file rem_auringbuf.h Audio Circular Buffer
 */

struct auringbuf;

int  auringbuf_alloc(struct auringbuf **abp, size_t min_sz, size_t max_sz);
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


#ifdef AURING_REPLACES_AUBUF
struct aubuf;


static inline int aubuf_alloc(struct aubuf **abp, size_t min_sz, size_t max_sz)
{
	return auringbuf_alloc((void *)abp, min_sz, max_sz);
}


/* TODO: not implemented */
static inline int aubuf_append(struct aubuf *ab, struct mbuf *mb) {
	(void)ab;
	(void)mb;
	return 0;
}


static inline int aubuf_write(struct aubuf *ab, const uint8_t *p, size_t sz) {
	return auringbuf_write((void *)ab, p, sz);
}


static inline void aubuf_read(struct aubuf *ab, uint8_t *p, size_t sz) {
	auringbuf_read((void *)ab, p, sz);
}

static inline int aubuf_get(struct aubuf *ab, uint32_t ptime, uint8_t *p,
			    size_t sz) {
	return auringbuf_get((void *)ab, ptime, p, sz);
}

static inline void aubuf_flush(struct aubuf *ab) {
	auringbuf_flush((void *)ab);
}


static inline int aubuf_debug(struct re_printf *pf, const struct aubuf *ab) {
	return auringbuf_debug(pf, (void *)ab);
}


static inline size_t aubuf_cur_size(const struct aubuf *ab) {
	return auringbuf_cur_size((void *)ab);
}


static inline int aubuf_write_samp(struct aubuf *ab, const int16_t *sampv,
				   size_t sampc)
{
	return auringbuf_write((void *)ab, (const uint8_t *)sampv, sampc * 2);
}


static inline void aubuf_read_samp(struct aubuf *ab, int16_t *sampv,
				   size_t sampc)
{
	auringbuf_read((void *)ab, (uint8_t *)sampv, sampc * 2);
}


static inline int aubuf_get_samp(struct aubuf *ab, uint32_t ptime,
				 int16_t *sampv, size_t sampc)
{
	return auringbuf_get((void *)ab, ptime, (uint8_t *)sampv, sampc * 2);
}
#endif
