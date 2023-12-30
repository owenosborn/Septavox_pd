/*
 * wave_synth.c
 *
 *  Created on: Jan 1, 2015
 *      Author: owen
 */


#include "oscillator.h"
#include "pp6.h"
#include "audio.h"
#include "wave_synth.h"

extern pocket_piano pp6;
extern uint8_t wavetable_selector;
static sin_oscillator sins[4];
static bl_saw saws[4];
static bl_square squares[4];


void wave_synth_init(void){
	uint8_t i;
	for (i = 0; i<4; i++){
		bl_saw_reset(&saws[i]);
		bl_square_init(&squares[i]);
	}

}

float32_t wave_synth_process(void){

	uint8_t i;
	float32_t sig;

	// just make sure freqs are never 0  cause of divide by 0 issues in the saw and square implementations
	for (i = 0; i < 4; i++) {
		if (pp6.freqs[i] == 0) {
			pp6.freqs[i] = .01f;
		}
	}

	if (pp6.aux_button_count == 0){
		wavetable_selector = 1;
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&sins[i], pp6.freqs[i] * (pp6_get_knob_1() + 1), .1f);
			sig += sin_process(&sins[i]) * pp6.amps[i];
		}
		sig *= 1;
	}
	if (pp6.aux_button_count == 1){
		wavetable_selector = 0;
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&sins[i], pp6.freqs[i] * (pp6_get_knob_1() + 1), .1f);
			sig += sin_process(&sins[i]) * pp6.amps[i];
		}
		sig *= 1;
	}
	if (pp6.aux_button_count == 2){
		wavetable_selector = 2;
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&sins[i], pp6.freqs[i] * (pp6_get_knob_1() + 1), .1f);
			sig += sin_process(&sins[i]) * pp6.amps[i];
		}
		sig *= 1;
	}

	if (pp6.aux_button_count == 3){
		sig = 0;
		for (i = 0; i < 4; i++) {
			bl_square_set(&squares[i], pp6.freqs[i] * (pp6_get_knob_1() + 1) );
			sig += bl_square_process(&squares[i]) * pp6.amps[i];
		}
		sig *= .2f;
	}
	if (pp6.aux_button_count == 4){
		sig = 0;
		for (i = 0; i < 4; i++) {
			bl_saw_set(&saws[i], pp6.freqs[i] * (pp6_get_knob_1() + 1) );
			sig += bl_saw_process(&saws[i]) * pp6.amps[i];
		}
		sig *= .2f;
	}


	if (pp6.aux_button_count == 5){
		wavetable_selector = 3;
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&sins[i], pp6.freqs[i] * (pp6_get_knob_1() + 1), .1f);
			sig += sin_process(&sins[i]) * pp6.amps[i];
		}
		sig *= 1;
	}
	if (pp6.aux_button_count == 6){
		wavetable_selector = 4;
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&sins[i], pp6.freqs[i] * (pp6_get_knob_1() + 1), .1f);
			sig += sin_process(&sins[i]) * pp6.amps[i];
		}
		sig *= 1;
	}
	return sig;
}

