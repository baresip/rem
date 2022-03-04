/**
 * @file ajb.c  Adaptive Jitter Buffer algorithm
 *
 * Copyright (C) 2022 Commend.com - c.spielberger@commend.com
 */
#include <stdlib.h>
#include <re.h>
#include <rem_au.h>
#include <rem_auframe.h>
#include <rem_aubuf.h>
#include "ajb.h"

#define DEBUG_LEVEL 6

/**
 * @brief The adaptive jitter computation is done by means of an exponential
 * moving average (EMA).
 *￼j_i = j_{i-1} + a (c - j_{i-1})
 *
 * Where $a$ ist the EMA coefficient and $c$ is the current value.
 */
enum {
	JITTER_EMA_COEFF   = 512,  /* Divisor for jitter EMA coefficient */
	JITTER_UP_SPEED    = 64,   /* 64 times faster up than down       */
	BUFTIME_EMA_COEFF  = 128,  /* Divisor for Buftime EMA coeff.     */
	BUFTIME_LO         = 125,  /* 125% of jitter                     */
	BUFTIME_HI         = 200,  /* 200% of jitter                     */
};


/** Adaptive jitter buffer statistics */
struct ajb {
	int32_t jitter;      /**< Jitter in [us]                  */
	struct lock *lock;

	uint32_t ts0;        /**< previous timestamp              */
	uint64_t tr0;        /**< previous time of arrival        */
#if DEBUG_LEVEL >= 6
	uint64_t tr00;       /**< arrival of first packet         */
	struct {
		int32_t d;
		uint32_t buftime;
		uint32_t bufmin;
		uint32_t bufmax;
	} plot;
#endif

	enum ajb_state as;    /**< computed jitter buffer state    */

	uint32_t ptime;      /**< Packet time [us]                */
	int32_t avbuftime;   /**< average buffered time [us]      */
	bool started;        /**< Started flag                    */
};


static void destructor(void *arg)
{
	struct ajb *ajb = arg;

	mem_deref(ajb->lock);
}


#if DEBUG_LEVEL >= 6
static void plot_ajb(struct ajb *ajb, uint64_t tr)
{
	uint32_t treal;

	if (!ajb->tr00)
		ajb->tr00 = tr;

	treal = (uint32_t) (tr - ajb->tr00);
	re_printf("%s, 0x%p, %u, %i, %u, %u, %u, %i, %i, %u\n",
			__func__,               /* row 1  - grep */
			ajb,                    /* row 2  - grep optional */
			treal,                  /* row 3  - plot optional */
			ajb->plot.d,            /* row 4  - plot */
			ajb->jitter,            /* row 5  - plot */
			ajb->plot.buftime,      /* row 6  - plot */
			ajb->avbuftime,         /* row 7  - plot */
			ajb->plot.bufmin,       /* row 8  - plot */
			ajb->plot.bufmax,       /* row 9  - plot */
			ajb->as);               /* row 10 - plot */
}
#endif

/**
 * Initializes the adaptive jitter buffer statistics
 *
 * @param ajb    Adaptive jitter buffer statistics
 */
struct ajb *ajb_alloc(void)
{
	struct ajb *ajb;
	int err;

	ajb = mem_zalloc(sizeof(*ajb), destructor);
	if (!ajb)
		return NULL;

	err = lock_alloc(&ajb->lock);
	if (err)
		goto out;

	ajb->ts0 = 0;
	ajb->tr0 = 0;
	ajb->as = AJB_GOOD;

out:
	if (err)
		ajb = mem_deref(ajb);

	return ajb;
}


void ajb_reset(struct ajb *ajb)
{
	if (!ajb)
		return;

	lock_write_get(ajb->lock);
	ajb->ts0 = 0;
	ajb->tr0 = 0;

	/* We start with wish size. */
	ajb->started = false;
	ajb->as = AJB_GOOD;
	lock_rel(ajb->lock);
}


/**
 * Computes the jitter for audio frame arrival.
 *
 * @param ajb     Adaptive jitter buffer statistics
 * @param af      Audio frame
 * @param cur_sz  Current aubuf size
 */
void ajb_calc(struct ajb *ajb, struct auframe *af, size_t cur_sz)
{
	uint64_t tr;                       /**< Real time in [us]            */
	uint32_t buftime, bufmax, bufmin;  /**< Buffer time in [us]          */
	int32_t d;                         /**< Time shift in [us]           */
	int32_t da;                        /**< Absolut time shift in [us]   */
	int32_t s;                         /**< EMA coefficient              */
	int64_t ts;                        /**< Time stamp                   */
	uint32_t tsd;                      /**< Time stamp divisor           */
	size_t sz;


	if (!ajb || !af || !af->srate)
		return;

	lock_write_get(ajb->lock);
	sz = aufmt_sample_size(af->fmt);
	ts = (int64_t) af->timestamp;
	tr = tmr_jiffies_usec();
	tsd = af->srate / 1000;
	if (!ajb->ts0)
		goto out;

	d = (int32_t) ( ((int64_t) tr - (int64_t) ajb->tr0) -
			(ts - (int64_t) ajb->ts0) * 1000 / tsd );

	da = abs(d);

	buftime = cur_sz / sz * 1000 * 1000 / af->srate;
	if (ajb->started) {
		ajb->avbuftime += ((int32_t) buftime - ajb->avbuftime) /
				  BUFTIME_EMA_COEFF;
		if (ajb->avbuftime < 0)
			ajb->avbuftime = 0;
	}
	else {
		/* Directly after "filling" of aubuf compute a good start value
		 * fitting to wish size. */
		ajb->avbuftime = buftime;
		ajb->jitter = ajb->avbuftime * 100 * 2 /
			(BUFTIME_LO + BUFTIME_HI);
		ajb->started = true;
	}

	if (!ajb->ptime)
		goto out;

	s = da > ajb->jitter ? JITTER_UP_SPEED : 1;

	ajb->jitter += (da - ajb->jitter) * s / JITTER_EMA_COEFF;
	if (ajb->jitter < 0)
		ajb->jitter = 0;

	bufmin = (uint32_t) ajb->jitter * BUFTIME_LO / 100;
	bufmax = (uint32_t) ajb->jitter * BUFTIME_HI / 100;

	bufmin = MAX(bufmin, ajb->ptime * 2 / 3);
	bufmax = MAX(bufmax, bufmin + 4 * ajb->ptime / 3);

	if ((uint32_t) ajb->avbuftime < bufmin)
		ajb->as = AJB_LOW;
	else if ((uint32_t) ajb->avbuftime > bufmax)
		ajb->as = AJB_HIGH;
	else
		ajb->as = AJB_GOOD;

#if DEBUG_LEVEL >= 6
	ajb->plot.d = d;
	ajb->plot.buftime = buftime;
	ajb->plot.bufmin  = bufmin;
	ajb->plot.bufmax  = bufmax;
	plot_ajb(ajb, tr / 1000);
#endif
out:
	lock_rel(ajb->lock);
	ajb->ts0 = ts;
	ajb->tr0 = tr;
}


enum ajb_state ajb_get(struct ajb *ajb, struct auframe *af)
{
	enum ajb_state as;

	if (!ajb || !af || !af->srate || !af->sampc)
		return AJB_GOOD;

	lock_write_get(ajb->lock);

	/* ptime in [us] */
	ajb->ptime = af->sampc * 1000 * 1000 / af->srate;
	as = ajb->as;
	if (!ajb->avbuftime)
		goto out;

	if (as == AJB_HIGH) {
		/* early adjustment of avbuftime */
		ajb->avbuftime -= ajb->ptime;
		ajb->as = AJB_GOOD;
	}
	else if (as == AJB_LOW) {
		/* early adjustment */
		ajb->avbuftime += ajb->ptime;
		ajb->as = AJB_GOOD;
	}

out:
	lock_rel(ajb->lock);
	return as;
}


int32_t ajb_debug(const struct ajb *ajb)
{
	int32_t jitter;

	lock_write_get(ajb->lock);
	jitter = ajb ? ajb->jitter : 0;
	lock_rel(ajb->lock);
	re_printf("jitter: %d, avbuftime: %d\n",
		  ajb->jitter / 1000, ajb->avbuftime);

	return jitter;
}
