/*
 * mode_slider.c
 *
 *  Created on: Jan 7, 2015
 *      Author: owen
 */

#include "oscillator.h"
#include "pp6.h"
#include "audio.h"
#include "mode_slider.h"
#include "wave_synth.h"
#include "sadsr.h"
#include "line.h"


extern pocket_piano pp6;

static sadsr amp_env[4];
static line framps[4];

static uint32_t note_dur[4] = {0,0,0,0};

void mode_slider_init(void){
	uint8_t i;
	for (i = 0; i<4; i++){
		sadsr_init(&amp_env[i]);
	}
}

void mode_slider_stop_notes(void){
	uint8_t i;
	for (i = 0; i<4; i++){
		note_dur[i] = 0;
		sadsr_init(&amp_env[i]);
	}
}

float32_t mode_slider_sample_process (void) {
	uint8_t i;
	float32_t vibrato;
	uint8_t tmpwav;

	// process envelopes
	for (i = 0; i < 4; i++) {
		pp6.amps[i] = sadsr_process(&amp_env[i]);
		pp6.amps[i] = pp6.amps[i] * pp6.amps[i];   // square it for fake log
		pp6.freqs[i] = c_to_f(line_process(&framps[i]));;
	}

	return wave_synth_process();
}

void mode_slider_control_process (void) {
	uint8_t i;
	static uint8_t voices_last[] = {0,0,0,0};
	float32_t cents;

	float32_t quickly;

	quickly = pp6_get_knob_3() *  pp6_get_knob_3();

	for (i=0;i<4;i++){

		if (pp6.voices[i] != voices_last[i]){

			if (pp6.voices[i] ) {
				/////
				note_dur[i] = 0;
				sadsr_set(&amp_env[i], .01f, .2f, .15f, .85f);
				cents = (pp6.voices[i] * 100) + 1200;
				line_set(&framps[i], cents + ((pp6_get_knob_2() - .5f)  * 2400.f));   // 2400 cents sharp
				line_go(&framps[i], cents, (quickly + .001f) * 1500.f);
				sadsr_go(&amp_env[i]);
			}
			else {
				//sadsr_set(&amp_env[i], .01f, .1f, (note_dur * 128) / SR, .85f);
				sadsr_set(&amp_env[i], .01f, .1f, (note_dur[i] * 128) / SR, .85f);
				sadsr_release(&amp_env[i]);
			}
			voices_last[i] = pp6.voices[i];
		}
		note_dur[i]++;
	}


}
