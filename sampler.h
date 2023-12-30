
#include "arm_math.h"

#define SAMPLE_BASE_FREQ 440;

typedef struct  {
	 uint32_t sample_index;  // all voices to starting point
	 float32_t sample_index_float;
	 float32_t phase_step;
	 float32_t freq;
	 float32_t amplitude;
	 uint32_t play_length;
	 uint8_t play_status;
	 uint8_t loop;
} sampler;


void sampler_init(sampler * samp);

float32_t sampler_process(sampler * samp);

void sampler_set_freq(sampler * samp, float32_t freq);

void sampler_set_amp(sampler * samp, float32_t amp);

void sampler_rewind(sampler * samp);

void sampler_play(sampler * samp);

void sampler_pause(sampler * samp);

void sampler_set_loop(sampler * samp, uint8_t loop);

void sampler_set_play_length(sampler * samp, uint32_t len);
