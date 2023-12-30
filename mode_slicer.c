/*
 * mode_slicer.c
 *
 *  Created on: Jan 6, 2015
 *      Author: owen
 */


#include "oscillator.h"
#include "pp6.h"
#include "audio.h"
#include "mode_slicer.h"
#include "wave_synth.h"
#include "line.h"

extern pocket_piano pp6;
extern uint8_t wavetable_selector;

static line aramps[4];

static sin_oscillator sin_lfo;

static float32_t freqs[4] = {100, 101, 102, 103};
static float32_t amps[4] = {0, 0, 0, 0};

static uint8_t pattern[9][24] = {
		{0,0,0,0, 0,0,0,0, 0,0,0,0, 1,1,1,1, 1,1,1,1, 1,1,1,1},
		{0,0,0,0, 1,1,1,1, 0,0,0,1, 1,1,1,0, 0,1,1,0, 1,0,1,0},
		{0,0,0,0, 1,1,1,1, 0,0,0,0, 1,1,1,1, 1,1,1,1, 1,1,1,1},
		{1,1,1,0, 0,0,0,1, 1,0,0,0, 1,1,1,1, 0,0,0,0, 1,1,0,0}, //swing


		{0,0,1,1, 1,1,1,1, 0,0,0,0, 0,0,1,1, 0,0,1,1, 1,0,0,0},
		{1,1,1,1, 0,0,0,0, 0,0,0,0, 1,1,1,1, 0,0,0,0, 1,1,0,0},
		{0,0,0,0, 0,0,0,0, 1,0,1,0, 1,1,0,0, 1,1,1,1, 1,1,1,1}, //end of song articulation
		{0,0,0,1, 0,0,1,0, 0,1,0,0, 1,0,0,0, 0,0,1,1, 0,1,1,0}, //morse
		{0,0,0,0, 0,0,0,0, 0,0,0,1, 0,0,0,0, 0,0,0,0, 0,0,0,1,}
};
/*
		{1,0,0,0, 1,1,1,1, 0,0,0,0, 0,0,0,1, 1,1,1,1, 0,0,0,0},
		{0,1,1,0, 0,0,1,1, 0,0,0,0, 1,1,1,1, 1,1,0,0, 0,0,0,0},
		{0,0,0,0, 0,0,0,0, 1,1,1,1, 1,1,1,1, 0,0,0,0, 0,0,0,0},
		{1,1,1,0, 0,0,0,1, 1,0,0,0, 1,1,1,1, 0,0,0,0, 1,1,0,0}, //swing

		{0,0,0,1, 0,1,1,0, 0,0,1,1, 1,1,0,0, 1,1,1,1, 0,1,1,1},
		{0,0,0,0, 0,0,0,0, 0,0,0,0, 1,1,1,1, 1,1,1,1, 1,1,1,1},
		{0,0,0,0, 0,0,0,0, 0,0,0,0, 1,1,1,1, 1,1,1,1, 1,1,1,1},
		{0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1, 0,1,0,1}
*/

static uint32_t step = 0;

static uint32_t sample_counter = 0;

static uint8_t slice_amp = 0;
static uint8_t slice_amp_last = 0;
static line slice_line;
static float32_t sliced;

//uint8_t slicer[16][8]

void mode_slicer_init(void){
	line_set(&slice_line, 0);
}

float32_t mode_slicer_sample_process (void) {
	uint8_t i;


	/*  basic slice
	float32_t slice;
	uint8_t tmpwav;
	tmpwav = wavetable_selector;   // backup wavetable
	wavetable_selector = 3;   // square wave

	sin_set(&sin_lfo, 50 * pp6_get_knob_3(), 1.f);
	slice = (sin_process(&sin_lfo) * .5f) + .5f;

	for (i = 0; i < 4; i++) {
		pp6.freqs[i] = freqs[i];
			pp6.amps[i] = amps[i];
	}
	wavetable_selector = tmpwav;   //restore it

	return wave_synth_process() * slice;*/

	// pattern slice

	//click along ,  do it from midi clock , or manual
	if (pp6_midi_clock_present()){
		step = pp6_get_midi_clock_count();
	}
	else {
		sample_counter++;

		if (sample_counter > ((1 - pp6_get_knob_3()) * 500.f)){
			sample_counter = 0;
			step++;
			if (step == 24) step = 0;
		}
	}

	for (i = 0; i < 4; i++) {
		pp6.freqs[i] = freqs[i];
		pp6.amps[i] = line_process(&aramps[i]);
	}

	slice_amp = (float32_t)pattern[(uint8_t)(pp6_get_knob_2() * 9)][step];

	if (slice_amp != slice_amp_last){
		line_go(&slice_line, (float32_t)slice_amp, 3.f);
		slice_amp_last = slice_amp;
	}
	sliced = line_process(&slice_line);

	return wave_synth_process() * sliced;
}

void mode_slicer_control_process (void) {
	uint8_t i;
	static uint8_t voices_last[] = {0,0,0,0};

	for (i=0;i<4;i++){
		if (pp6.voices[i] != voices_last[i]){
			if (pp6.voices[i]) {
				freqs[i] = c_to_f(pp6.voices[i] * 100) * 2;
				line_go(&aramps[i], 1, 5);

			}
			else {

				freqs[i] = 0.1f;
				line_go(&aramps[i], 0, 20);
			}
			voices_last[i] = pp6.voices[i];
		}
	}
}
