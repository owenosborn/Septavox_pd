/*
 * mode_simple_poly.c
 *
 *  Created on: Oct 18, 2013
 *      Author: owen
 */


#include "oscillator.h"
#include "pp6.h"
#include "audio.h"
#include "mode_simple_poly.h"
#include "wave_synth.h"
#include "line.h"

extern pocket_piano pp6;
extern uint8_t wavetable_selector;

static line aramps[4];

static sin_oscillator sin_lfo;

static float32_t freqs[4] = {100, 101, 102, 103};

void mode_simple_poly_init(void){

}

float32_t mode_simple_poly_sample_process (void) {
	uint8_t i;
	float32_t vibrato;
	uint8_t tmpwav;
	float32_t vibrato_depth;

	// backup wavetable
	tmpwav = wavetable_selector;
	wavetable_selector = 0;

	sin_set(&sin_lfo, 50 * pp6_get_knob_3(), 1.f);
	vibrato = sin_process(&sin_lfo);

	// cube it for poor mans log
	vibrato_depth = pp6_get_knob_2() * pp6_get_knob_2() * pp6_get_knob_2();

	for (i = 0; i < 4; i++) {

		pp6.freqs[i] = freqs[i] + (vibrato * vibrato_depth * freqs[i] * .5);  // square knob 2 so it turns up more gradually
		pp6.amps[i] = line_process(&aramps[i]);
	}
	wavetable_selector = tmpwav;   //restore it

	return wave_synth_process();
}

void mode_simple_poly_control_process (void) {
	uint8_t i;
	static uint8_t voices_last[] = {0,0,0,0};
	for (i=0;i<4;i++){
		if (pp6.voices[i] != voices_last[i]){
			if (pp6.voices[i]) {
				freqs[i] = c_to_f(pp6.voices[i] * 100) * 2;
				line_go(&aramps[i], 1, 5);
			}
			else {
				line_go(&aramps[i], 0, 20);
			}
		}
		voices_last[i] = pp6.voices[i];
	}
}
