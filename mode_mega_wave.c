/*
 * mode_mega_wave.c
 *
 *  Created on: Jan 8, 2015
 *      Author: owen
 */




#include "oscillator.h"
#include "pp6.h"
#include "audio.h"
#include "mode_mega_wave.h"
#include "oscillator.h"
#include "line.h"

extern uint8_t wavetable_selector;

extern pocket_piano pp6;

static line framps[4];

static line aramps[4];


static sin_oscillator waves[4][4];

static float32_t freqs[4][4] = {
		{200, 200.1f, 300.3f, 400.4f},
		{200, 200.1f, 300.3f, 400.4f},
		{200, 200.1f, 300.3f, 400.4f},
		{200, 200.1f, 300.3f, 400.4f},
};

static float32_t amps[4] = {0,0,0,0};

void mode_mega_wave_init(void){
	uint8_t i, j;
	for (i = 0; i < 4; i++){
		for (j = 0; j< 4; j++){
			waves[i][j].phase_accum = 0;
			sin_set(&waves[i][j], freqs[i][j], 1.f);
		}
		line_set(&framps[i], 100.f);
	}
}

float32_t mode_mega_wave_sample_process (void) {
	uint8_t i, j;
	float32_t sigs[4];

	float32_t knob1;

	for (i = 0; i < 4; i++){
		sigs[i] = 0;
		freqs[i][0] = c_to_f(line_process(&framps[i]));
		freqs[i][1] = freqs[i][0] + (pp6_get_knob_2() * 10.f);
		freqs[i][2] = freqs[i][0] + (pp6_get_knob_2() * 20.f);
		freqs[i][3] = freqs[i][0] + (pp6_get_knob_2() * 30.f);

	}

	if (pp6.aux_button_count == 0) wavetable_selector = 0;
	if (pp6.aux_button_count == 1) wavetable_selector = 1;
	if (pp6.aux_button_count == 2) wavetable_selector = 2;
	if (pp6.aux_button_count == 3) wavetable_selector = 3;
	if (pp6.aux_button_count == 4) wavetable_selector = 4;
	if (pp6.aux_button_count == 5) wavetable_selector = 5;
	if (pp6.aux_button_count == 6) wavetable_selector = 6;


	knob1 = pp6_get_knob_1() + 1;
	for (i = 0; i < 4; i++){
		for (j = 0; j< 4; j++){
			sin_set(&waves[i][j], freqs[i][j] * knob1, 1.f);
			sigs[i] += sin_process_simple(&waves[i][j]) * .05f;

		}
		sigs[i] *= line_process(&aramps[i]);;
	}

	return sigs[0] + sigs[1] + sigs[2] + sigs[3];
}

void mode_mega_wave_control_process (void) {
	uint8_t i;
	static uint8_t voices_last[] = {0,0,0,0};
	float32_t cents;


	for (i=0;i<4;i++){
		if (pp6.voices[i] != voices_last[i]){
			if (pp6.voices[i]) {
				cents = (pp6.voices[i] * 100) + 1200;
				/*freqs[i][0] = (c_to_f(pp6.voices[i] * 100) * 2);
				freqs[i][1] = (c_to_f(pp6.voices[i] * 100) * 2) + (pp6_get_knob_2() * 10.f);
				freqs[i][2] = (c_to_f(pp6.voices[i] * 100) * 2) + (pp6_get_knob_2() * 20.f);
				freqs[i][3] = (c_to_f(pp6.voices[i] * 100) * 2) + (pp6_get_knob_2() * 30.f);*/
				line_go(&framps[i], cents, (pp6_get_knob_3() + .001f) * 1500.f);

				line_go(&aramps[i], 1, 5);
			}
			else {



				freqs[i][0] = 0.1f;
				freqs[i][1] = 0.1f;
				freqs[i][2] = 0.1f;
				freqs[i][3] = 0.1f;

				line_go(&aramps[i], 0, 500);
			}
			voices_last[i] = pp6.voices[i];
		}
	}

}
