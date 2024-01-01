/*
 * wave_synth.c
 *
 *  Created on: Jan 1, 2015
 *      Author: owen
 */

#include <stdint.h>
#include "audio.h"
#include "oscillator.h"
#include "wave_synth.h"

void wave_synth_init(wave_synth *ws){
	uint8_t i;
	for (i = 0; i<4; i++){
		bl_saw_reset(&(ws->saws[i]));
		bl_square_init(&(ws->squares[i]));
	}

}

float wave_synth_process(wave_synth *ws){

	uint8_t i;
	float sig;

	// just make sure freqs are never 0  cause of divide by 0 issues in the saw and square implementations
	for (i = 0; i < 4; i++) {
		if (ws->freqs[i] == 0) {
			ws->freqs[i] = .01f;
		}
	}

	if (ws->wavetable_selector == 1){
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&(ws->sins[i]), ws->freqs[i] * (ws->tune + 1), .1f);
			sig += sin_process(&(ws->sins[i]), ws->wavetable_selector) * ws->amps[i];
		}
		sig *= 1;
	}
	if (ws->wavetable_selector == 0){
		ws->wavetable_selector = 0;
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&(ws->sins[i]), ws->freqs[i] * (ws->tune + 1), .1f);
			sig += sin_process(&(ws->sins[i]), ws->wavetable_selector) * ws->amps[i];
		}
		sig *= 1;
	}
	if (ws->wavetable_selector == 2){
		ws->wavetable_selector = 2;
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&(ws->sins[i]), ws->freqs[i] * (ws->tune + 1), .1f);
			sig += sin_process(&(ws->sins[i]), ws->wavetable_selector) * ws->amps[i];
		}
		sig *= 1;
	}

	if (ws->wavetable_selector == 5){
		sig = 0;
		for (i = 0; i < 4; i++) {
			bl_square_set(&(ws->squares[i]), ws->freqs[i] * (ws->tune + 1) );
			sig += bl_square_process(&(ws->squares[i])) * ws->amps[i];
		}
		sig *= .2f;
	}
	if (ws->wavetable_selector == 6){
		sig = 0;
		for (i = 0; i < 4; i++) {
			bl_saw_set(&(ws->saws[i]), ws->freqs[i] * (ws->tune + 1) );
			sig += bl_saw_process(&(ws->saws[i])) * ws->amps[i];
		}
		sig *= .2f;
	}
	if (ws->wavetable_selector == 3){
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&(ws->sins[i]), ws->freqs[i] * (ws->tune + 1), .1f);
			sig += sin_process(&(ws->sins[i]), ws->wavetable_selector) * ws->amps[i];
		}
		sig *= 1;
	}
	if (ws->wavetable_selector == 4){
		sig = 0;
		for (i = 0; i < 4; i++) {
			sin_set(&(ws->sins[i]), ws->freqs[i] * (ws->tune + 1), .1f);
			sig += sin_process(&(ws->sins[i]), ws->wavetable_selector) * ws->amps[i];
		}
		sig *= 1;
	}
	return sig;
}


