/*
 * Audio frame
 */

/**
 * Defines a frame of audio samples
 */
struct auframe {
	enum aufmt fmt;      /**< Sample format (enum aufmt)        */
	uint32_t srate;      /**< Samplerate                        */
	void *sampv;         /**< Audio samples (must be mem_ref'd) */
	size_t sampc;        /**< Total number of audio samples     */
	uint64_t timestamp;  /**< Timestamp in AUDIO_TIMEBASE units */
	double level;        /**< Audio level in dBov               */
	uint8_t ch;          /**< Channels                          */
	uint8_t padding[7];
};

void auframe_init(struct auframe *af, enum aufmt fmt, void *sampv,
		  size_t sampc, uint32_t srate, uint8_t ch);

/**
 * Update an audio frame
 *
 * @param af        Audio frame
 * @param sampv     Audio samples
 * @param sampc     Total number of audio samples
 * @param timestamp Timestamp in AUDIO_TIMEBASE units
 */
static inline void auframe_update(struct auframe *af, void *sampv,
				  size_t sampc, uint64_t timestamp)
{
	if (!af)
		return;

	af->sampv = sampv;
	af->sampc = sampc;
	af->timestamp = timestamp;
}

size_t auframe_size(const struct auframe *af);
void   auframe_mute(struct auframe *af);

/**
 * Get audio level (only calculated once)
 *
 * @param af        Audio frame
 *
 * @return Audio level expressed in dBov on success and AULEVEL_UNDEF on error
 */
static inline double auframe_level(struct auframe *af)
{
	if (af->level == AULEVEL_UNDEF)
		af->level = aulevel_calc_dbov(af->fmt, af->sampv, af->sampc);

	return af->level;
}
