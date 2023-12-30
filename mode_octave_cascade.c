/*
 * mode_octave_cascade.c
 *
 *  Created on: Jan 6, 2015
 *      Author: owen
 */



#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "sad.h"
#include "audio.h"
#include "wave_synth.h"
#include "mode_octave_cascade.h"

extern pocket_piano pp6;

static sin_oscillator sins[16];

static sad amp_env[4];
static bl_saw saws[4];
static bl_square squares[4];


static uint32_t  click = 0;
static uint32_t click_count = 0;



void mode_octave_cascade_init(void){
	uint8_t i;
	for (i = 0; i<4; i++){
		bl_saw_reset(&saws[i]);
		sad_init(&amp_env[i]);
		bl_square_init(&squares[i]);
	}
}

float32_t mode_octave_cascade_sample_process (void) {
	uint8_t i;

	// process envelopes
	for (i = 0; i < 4; i++) {
		pp6.amps[i] = sad_process(&amp_env[i]);
		pp6.amps[i] = pp6.amps[i] * pp6.amps[i];   // square it for fake log
	}

	return wave_synth_process();
}

void mode_octave_cascade_control_process (void) {

	uint8_t i = 0;
	    static uint8_t l = 0;
	    static uint8_t count2 = 0;
	    static uint8_t octave_shift[] = {0, 0, 0, 0};

	    //envelopes
		for (i=0;i<4;i++){
			//sad_set(&amp_env[i], .01f, (pp6_get_knob_2() * 2.f) + .01f);
		}

		//click along ,  do it from midi clock , or manual
		static uint8_t midi_clk_last = 0;
		uint8_t midi_clk = pp6_get_midi_clock_count();

		if (pp6_midi_clock_present()){
			if (midi_clk != midi_clk_last){

		        if (pp6_get_knob_3() < .25f) {
		            if (!(pp6_get_midi_clock_count() % 3)) {
		                click = 1;
		            }
		        }
		        else if (pp6_get_knob_3() < .5f) {
		            if (!(pp6_get_midi_clock_count() % 6)) {
		                click = 1;
		            }
		        }
		        else if (pp6_get_knob_3() < .75f) {
		            if (!(pp6_get_midi_clock_count() % 8)) {
		                click = 1;
		            }
		        }
		        else if (pp6_get_knob_3() < 1.1f) {
		            if (!(pp6_get_midi_clock_count() % 12)) {
		                click = 1;
		            }
		        }
		        midi_clk_last = midi_clk;
			}  // a new tick recieved
		} // midi clock present
		else {
		    click_count++;
		    if (click_count > ((100 * (1 - pp6_get_knob_3())) + 4) ) {
		    	click_count = 0;
		    	click = 1;
		    }
		}




	    //Pattern #3 - Cycle Through

	    if (click) {
	        click = 0;
	        l++;
	        l &= 3;
	        // find octave shifts for each voice
	        count2++;
	        if (count2 > 2) count2 = 0;

	        // do strage octave shifting  sounds cool,
	        // don't know how it works exactly, you'll have to ask chris
	        if (count2 ==0) {
	            for (i = 0; i < 4; i++){
	                octave_shift[i] = ((l+i) & 3);
	            }
	        }
	        else if (count2 == 1) {
	            for (i = 0; i < 4; i++){
	                octave_shift[i] =  ((l + ((i & 1) * 2)) & 3);
	            }
	        }
	        else if (count2 == 2) {
	            for (i = 0; i < 4; i++){
	                octave_shift[i] = l;
	            }
	        }

	        // update synth
	        for (i=0; i<4; i++){

	            if (pp6.voices[i]) {
		        	pp6.freqs[i] = c_to_f(pp6.voices[i] * 100) * 2 / (1 << octave_shift[i]);
	             //   oscil_phase_step[i] = ((( (miditof[(voices[i] % 12)]) * tune ) >> 10) << ((voices[i] / 12))) >> octave_shift[i]  ;
		        	sad_set(&amp_env[i], .01f, (pp6_get_knob_2() * 2.f) + .01f);
		        	sad_go(&amp_env[i]);
	            }
	            else {
		        	pp6.freqs[i] = 0.1f;
	            }
	        }
	    }
}
