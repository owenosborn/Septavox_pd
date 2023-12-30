/*
 * sampler.c
 *
 *  Created on: Oct 16, 2014
 *      Author: owen
 */

#include "sampler.h"
#include "stm32f4xx.h"
#include "arm_math.h"

//#include "piano.h"                     // contains the lookup tables
//#include "cello.h"                     // contains the lookup tables
#include "violin.h"                     // contains the lookup tables
//#include "sax.h"                     // contains the lookup tables



void sampler_init(sampler * samp){
	samp->freq = 0;
	samp->phase_step = 0;
	samp->sample_index = 0;
	samp->sample_index_float = 0;
	samp->loop = 0;
	samp->play_status =0;
	samp->amplitude =0;
	samp->play_length = violin_length - 2;

}

void sampler_set_freq(sampler * samp, float32_t freq){
	samp->freq = freq;
}

void sampler_set_amp(sampler * samp, float32_t amp){
	samp->amplitude = amp;
}

void sampler_rewind(sampler * samp){
	samp->sample_index = 0;
	samp->sample_index_float = 0;
}

void sampler_play(sampler * samp){
	samp->play_status = 1;
}
void sampler_pause(sampler * samp){
	samp->play_status = 0;
}
void sampler_set_loop(sampler * samp, uint8_t loop){
	samp->loop = loop;
}
void sampler_set_play_length(sampler * samp, uint32_t len){
	samp->play_length = len;
}

float32_t sampler_process(sampler * samp){

	float32_t sig;

	float32_t sample1, sample2;
	uint8_t sample;

	float32_t frac;

	sig = 0;
	if (samp->play_status){
		// get phase step from frequency
		samp->phase_step = samp->freq / SAMPLE_BASE_FREQ;

		// floating point index
		samp->sample_index_float += samp->phase_step;

		// get nearest sample and fractional part
		samp->sample_index = floor(samp->sample_index_float);
		frac = samp->sample_index_float - samp->sample_index;

		if (samp->sample_index >= samp->play_length){//(sax_length - 2)){
			samp->sample_index = 0;
			samp->sample_index_float = 0;
			if (samp->loop) {
				samp->play_status = 1;
			} else {
				samp->play_status = 0;
			}
		}

		sample = violin_sample[samp->sample_index];//(128 - piano_sample[sample_index[0]]);

		sample1 = ((float32_t) sample - 128) / 128;

		sample = violin_sample[samp->sample_index + 1];//(128 - piano_sample[sample_index[0]]);

		sample2 = ((float32_t) sample - 128) / 128;

		//sig = ((float32_t) sample - 128) / 128;
		sig = sample1 + frac * (sample2 - sample1);
	}

    return sig;
}
