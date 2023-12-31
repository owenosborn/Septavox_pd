/*
 * mode_adsr.c
 *
 *  Created on: Dec 11, 2014
 *      Author: owen
 */

#include "oscillator.h"
#include "pp6.h"
#include "audio.h"
#include "mode_adsr.h"
#include "sadsr.h"
#include "wave_synth.h"



//extern pocket_piano pp6;

//static sadsr amp_env[4];



void mode_adsr_init(void){
	uint8_t i;
	for (i = 0; i<4; i++){
		sadsr_init(&amp_env[i]);
	}

}

float32_t mode_adsr_sample_process (void) {

	uint8_t i;

	// process envelopes
	for (i = 0; i < 4; i++) {
		pp6.amps[i] = sadsr_process(&amp_env[i]);
		pp6.amps[i] = pp6.amps[i] * pp6.amps[i];   // square it for fake log
	}


	return wave_synth_process();
}

void mode_adsr_control_process (void) {
	uint8_t i;

	static uint8_t voices_last[] = {0,0,0,0};

	for (i=0;i<4;i++){
		//sadsr_set(&amp_env[i], 1.f, 1.f, 1.f, .6f);
		sadsr_set(&amp_env[i], pp6_get_knob_3() * 1.f, pp6_get_knob_2() * 1.f, pp6_get_knob_2() * 2.f, .8f);
	}

	for (i=0;i<4;i++){

		if (pp6.voices[i] != voices_last[i]){
			if (pp6.voices[i])
				pp6.freqs[i] = c_to_f(pp6.voices[i] * 100) * 2;

			if (pp6.voices[i] ) {
				sadsr_go(&amp_env[i]);
			}
			else {
				sadsr_release(&amp_env[i]);
			}
			voices_last[i] = pp6.voices[i];
		}
	}
}
