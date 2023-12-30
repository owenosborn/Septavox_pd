/*
 * mode_octave_arp.c
 *
 *  Created on: Aug 3, 2014
 *      Author: owen
 */

/*
 * mode_octave_arp.c
 *
 *  Created on: Oct 18, 2012
 *      Author: owen
 */

#include "arm_math.h"
#include "oscillator.h"
#include "pp6.h"
#include "line.h"
#include "sad.h"
#include "audio.h"
#include "wave_synth.h"
#include "mode_octave_arp.h"

extern pocket_piano pp6;

static sad amp_env[4];
static bl_saw saws[4];
static bl_square squares[4];

static uint32_t  click = 0;
static uint32_t click_count = 0;

void mode_octave_arp_init(void){
	uint8_t i;
	for (i = 0; i<4; i++){
		bl_saw_reset(&saws[i]);
		sad_init(&amp_env[i]);
		bl_square_init(&squares[i]);
		sad_set(&amp_env[i], .01f, .5f);
	}
}

float32_t mode_octave_arp_sample_process (void) {
	uint8_t i;

	// process envelopes
	for (i = 0; i < 4; i++) {
		pp6.amps[i] = sad_process(&amp_env[i]);
		pp6.amps[i] = pp6.amps[i] * pp6.amps[i];   // square it for fake log
	}

	return wave_synth_process();
}

void mode_octave_arp_control_process (void) {

	   uint8_t i = 0;
	    static uint8_t l = 0;
	    static uint8_t octave_shift = 0;

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
		    if (click_count > ((100 * (1 - pp6_get_knob_3())) + 1) ) {
		    	click_count = 0;
		    	click = 1;
		    }

		}



	    // play notes in the allocated voices array, so sequence plays notes in the order they were pressed
	    if (click) {
	        click = 0;
	        i = 0;
	        do {
	            l++;
	            l &= 3;
	            if (!l) octave_shift++;
	            i++;
	        }
	        while (!pp6.voices[l] && (i < 4));
	        if (pp6.voices[l]) {
	        	pp6.freqs[l] = c_to_f(pp6.voices[l] * 100) * 2 * ((octave_shift & 1) + 1);
	        	sad_set(&amp_env[l], .01f, (pp6_get_knob_2() * 2.f) + .01f);
	        	sad_go(&amp_env[l]);
	        	// play it !
	            //oscil_phase_step[l] = ((((miditof[(voices[l] % 12)]) * tune ) >> 12) << ((voices[l] / 12)))  << (octave_shift & 0x1);

	        }
	        else {
	         //   oscil_phase_step[l] = 0;
	        	//pp6.freqs[i] = 0.1f;
	        }

	       // if(oscil_phase_step[l]) envelope[l] = 0xFFFF; //envelope up,  only if we have a note
	    }
}
