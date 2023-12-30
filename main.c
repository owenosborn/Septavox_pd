/*
/*
===============================================================================
 Name        : main.c
 Author      :
 Version     :
 Copyright   : Copyright (C)
 Description : main definition
===============================================================================
*/


#include "stm32f4xx.h"


/* Includes ------------------------------------------------------------------*/

#include "arm_math.h"
#include "CS4344.h"
#include "pp6.h"
#include "miditof.h"
#include "uart.h"
#include "midi.h"
#include "timer.h"
#include "eq.h"
#include "audio.h"
#include "notelist.h"
#include "spi.h"
#include "sequencer.h"

#include "mode_simple_poly.h"
#include "mode_vibrato_synth.h"
#include "mode_analog_style.h"
#include "mode_bass_delay.h"
#include "mode_octave_arp.h"
#include "mode_simple_fm.h"
#include "mode_fun_saws.h"
#include "mode_simple_sampler.h"
#include "mode_sample_picker.h"
#include "mode_adsr.h"
#include "mode_octave_cascade.h"
#include "mode_slicer.h"
#include "mode_slider.h"
#include "mode_mega_wave.h"
#include "mode_knob_test.h"
#include "wave_synth.h"


// from the DAC driver CS4344.c
extern unsigned int software_index;
extern unsigned int hardware_index;
extern short play_buf[];
extern uint32_t control_ticker;

// for keeping track of time

static void Delay(__IO uint32_t nCount);

// led stuff
static uint32_t led_counter = 0;  // for the above flash function
static void flash_led_record_enable(void);

// MIDI buffer
extern uint8_t  uart_recv_buf[32];
extern uint8_t  uart_recv_buf_write;
extern uint8_t  uart_recv_buf_read;
uint8_t tmp8;

extern pocket_piano pp6;

// for enabling knob check mode
uint8_t knob_check_mode = 0;

// for the eQ
EQSTATE eq;

int main(void)
{
	uint8_t i;
	uint32_t time = 0;
	uint32_t aux_button_depress_time = 0;

	uint8_t new_note_on = 0;


/*	pp6_knobs_init();

	for(;;){

		Delay(20000);

		pp6_knobs_update();
	}*/


	 // enable random number generator
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);

	// initialize low level stuff
	uart_init();
	timer_init();

	// initialize piano
	pp6_init();
	spi_init();

	// initialize modes
	mode_simple_poly_init();
	mode_octave_arp_init();
	mode_adsr_init();
	mode_octave_cascade_init();
	mode_slider_init();
	mode_slicer_init();
	mode_mega_wave_init();
	mode_knob_test_init();
	wave_synth_init();

	// setup codec
	CS4344_init();
	//CS4344_init();

	float32_t sig, f;
	q15_t  wave;

	f = 50.0;

	note_list nl;

	// uart echo test, test midi
	/*uint8_t ch;
	while (1){

		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET); // Wait for Character

		ch = USART_ReceiveData(USART1);
		AUX_LED_RED_ON;
		while (USART_GetFlagStatus (USART1, USART_FLAG_TXE) == RESET);
		USART_SendData (USART1, ch);
		AUX_LED_RED_OFF;

	}*/

	// timer test
	/*while (1){
		t = TIM_GetCounter(TIM2);
		TIM_SetCounter(TIM1,0);
		t_spent = t - t_last;
		t_last = t;
	}*/

	uint32_t count = 0;

	// the bass boost is hardcoded in eq.c
	init_3band_state(&eq, 880, 5000, SR);

	// initialize midi library with channel 1
	midi_init(1);

	// transpose thingie
	// get key state for doing octave transpose thinie
	// update keys has to be called > 8 times for key press to be debounced
	for (i = 0; i < 9; i++)
		pp6_keys_update();

	// check for aux or mode press
	pp6_get_key_events();

	// set no transpose
	pp6_set_global_octave_transpose(0);

	// then see if mode or aux pressed
	if (pp6_mode_button_pressed())   // mode button, transpose up
		pp6_set_global_octave_transpose(1);
	if (pp6_aux_button_pressed())  { // aux button, transpose down
		// if the mode button is also down,  do knob check
		if (pp6_mode_button_pressed()){
			knob_check_mode = 1;
		}
		else {
			pp6_set_global_octave_transpose(2);
		}
		// this is a hack to get back to the correct waveform, call it 6 times
		pp6_set_aux_button_pressed();
		pp6_set_aux_button_pressed();
		pp6_set_aux_button_pressed();
		pp6_set_aux_button_pressed();
		pp6_set_aux_button_pressed();
		pp6_set_aux_button_pressed();

	}


	pp6_clear_flags();  // clear out keypresses
	// end transpose thingie


	note_list_init(&nl);
	//for(;;);

	pp6_set_mode(0);

	//pp6_set_aux_led(BLACK);

	// go!
	while (1)	{

	    /* Update WWDG counter */
	    //WWDG_SetCounter(127);

        // process MIDI, pp6 will be updated from midi.c handlers if there are any relevant midi events
        if (uart_recv_buf_read != uart_recv_buf_write){
            tmp8 = uart_recv_buf[uart_recv_buf_read];
            uart_recv_buf_read++;
            uart_recv_buf_read &= 0x1f;
            recvByte(tmp8);
           // send sync right away
	    	if (tmp8 == STATUS_SYNC) {
		    	sendSync();
	    	}
        }

        // empty the tx buffer
        uart_service_tx_buf();

		/*
			 * Sample Rate
			 */
		if (software_index != hardware_index){

			if (software_index & 1){   // channel

				//pp6_set_aux_led(RED	);
				time = timer_get_time();
				if (pp6_get_mode() == 0) sig = knob_check_mode ? mode_knob_test_sample_process() : mode_simple_poly_sample_process();
				if (pp6_get_mode() == 1) sig = mode_slider_sample_process();
				if (pp6_get_mode() == 2) sig = mode_octave_arp_sample_process();
				if (pp6_get_mode() == 3) sig = mode_adsr_sample_process();
				if (pp6_get_mode() == 4) sig = mode_octave_cascade_sample_process();
				if (pp6_get_mode() == 5) sig = mode_slicer_sample_process();
				if (pp6_get_mode() == 6) sig = mode_mega_wave_sample_process();
				time = timer_get_time() - time;
				//if ( (!((k>>16) & 1)) )
				// eq it
				//	sig = do_3band(&eq, sig);

				//pDst[n] = (q15_t)(pSrc[n] * 32768);
				//arm_float_to_q15(&sig, &wave, 1);

				if (sig > 1.f)
					sig = 1.f;

				if (sig < -1.f)
					sig = -1.f;


				wave = (q15_t)(sig * 32768);

				// put the r
				play_buf[software_index] = wave;
				software_index++;
				software_index &= 0xf;

			}
			else {   // channel 2, do the same thing
				// put the l
				play_buf[software_index] = wave;
				software_index++;
				software_index &= 0xf;
			}
		}

		/*
		 * Control Rate, 64 sample periods
		 */
		//if ((!(sample_clock & 0x3F)) && (sample_clock != sample_clock_last)){
		if (control_ticker > 50){

			// make sure this only happens once every 64 sample periods
			//sample_clock_last = sample_clock;
			control_ticker = 0;

	        // midi clock auto detection
	        pp6_check_for_midi_clock();

	        // update keys knobs
	        pp6_keys_update();
			pp6_knobs_update();

			// check for new key events
			pp6_get_key_events();

			// allocate voices
			pp6_allocate_voices();

			if (pp6_mode_button_pressed()){
				pp6_change_mode();
				// zero out tails in slider mode so they don't keep ringing
				mode_slider_stop_notes();
			}

			// maintain LED flasher
			pp6_flash_update();

			// every 128 times, check if knobs got moved
			count++;
			count &= 0x7f;
			if (!count) {
				pp6_check_knobs_touched();   // this sets the knobs touched flags
			}

			// get note list
			for (i = 0; i < 128; i++) {
				if (pp6_get_note_state(i) != pp6_get_note_state_last(i)) {
					if (pp6_get_note_state(i)) {
						note_list_note_on(&nl, i);
						new_note_on = 1;
						sendNoteOn(1, i, 100);
					}
					else {
						note_list_note_off(&nl, i);
						sendNoteOff(1, i, 0);
					}
				}
			}
			pp6_set_current_note_state_to_last();

			// smooth the knobs here in case they are playing back
			pp6_smooth_knobs();

			//t1 =  timer_get_time();

			if (pp6_get_mode() == 0)  knob_check_mode ? mode_knob_test_control_process() : mode_simple_poly_control_process();
			if (pp6_get_mode() == 1)  mode_slider_control_process();
			if (pp6_get_mode() == 2)  mode_octave_arp_control_process();
			if (pp6_get_mode() == 3)  mode_adsr_control_process();
			if (pp6_get_mode() == 4)  mode_octave_cascade_control_process();
			if (pp6_get_mode() == 5)  mode_slicer_control_process();
			if (pp6_get_mode() == 6)  mode_mega_wave_control_process(); // SECRET MODE ??

			//t2 = timer_get_time();
			//t = t2 - t1;
			// clear all the events

			pp6_clear_flags();
			led_counter++;
		}


	}   // while 1
}  // main



void flash_led_record_enable() {
	/*if (led_counter > 150){
		led_counter = 0;
		if (pp6_get_aux_led()){
			pp6_set_aux_led(0);
		}
		else {
			pp6_set_aux_led(1);
		}
	}*/
}


// BEGIN SEQUENCER
    // tick the sequencer with midi clock if it is present, otherwise use internal
/*    if (pp6_midi_clock_present()){
    	if (pp6_get_midi_clock_tick()){
    		seq_tick();
    		pp6_clear_midi_clock_tick();
    	}
    }
	if (!pp6_midi_clock_present()){
		seq_tick();
	}

	// sequencer states
	if (seq_get_status() == SEQ_STOPPED){

		pp6_set_aux_led(BLACK);

		// aux button gets pressed and held
		if ( pp6_aux_button_pressed() ) {
			aux_button_depress_time++;
			if (aux_button_depress_time > 500){
				aux_button_depress_time = 0;
				new_note_on = 0;
				seq_set_status(SEQ_RECORD_ENABLE);
			}
		}

		if (pp6_aux_button_pressed() || pp6_get_midi_start()) {
			if (seq_get_length()) {  // only play if positive length
				seq_enable_knob_playback();
				seq_set_status(SEQ_PLAYING);
				sendStart();  // send out a midi start
			}
			else seq_set_status(SEQ_STOPPED);
			seq_rewind();
			//aux_button_depress_time = 0;
		}
	}
	else if (seq_get_status() == SEQ_RECORD_ENABLE){
		flash_led_record_enable();
		if (pp6_aux_button_pressed()) {
			seq_set_status(SEQ_STOPPED);
		}
		if (new_note_on){
			seq_set_status(SEQ_RECORDING);
			seq_start_recording();
			seq_log_events();
			seq_log_knobs(pp6_get_knob_array());
		//	sendStart();  // send out a midi start
		}
		if (pp6_get_midi_start()) {
			seq_set_status(SEQ_RECORDING);
			seq_start_recording();
			seq_log_first_note_null();   // sequence doesn't start with a note
		//	sendStart();  // send out a midi start
		}
	}
	else if (seq_get_status() == SEQ_RECORDING){

		pp6_set_aux_led(RED);

		seq_log_events();

		// stop recording
		if (pp6_aux_button_pressed() || seq_get_auto_stop()) {
			seq_stop_recording();
			seq_set_status(SEQ_PLAYING);
			seq_enable_knob_playback();
			aux_button_depress_time = 0;
			seq_clear_auto_stop();
			sendStop();  // send MIDI stop
		}
		if (pp6_get_midi_stop()) {   // if a midi stop is received, stop recording, and dont play
			seq_stop_recording();
			seq_set_status(SEQ_STOPPED);
			aux_button_depress_time = 0;
			seq_clear_auto_stop();
			sendStop();  // send MIDI stop
		}

	}
	else if (seq_get_status() == SEQ_PLAYING) {

		seq_play_knobs();  	// play knobs
		seq_play_tick();  	// play notes
		aux_led_color = GREEN;

		pp6_set_aux_led(aux_led_color);

		// flash white on rollover
		if (seq_get_time() == 0) pp6_flash_aux_led(75);

		// aux button gets pressed and held
		if ( pp6_aux_button_pressed() ) {
			aux_button_depress_time++;
			if (aux_button_depress_time > 500){
				aux_button_depress_time = 0;
				new_note_on = 0;
				seq_set_status(SEQ_RECORD_ENABLE);
				//pp6_set_synth_note_stop();  // stop the synth
				pp6_turn_off_all_on_notes();
			}
		}
		// aux button swithes to stop
		if (pp6_aux_button_pressed() || pp6_get_midi_stop()) {
			seq_set_status(SEQ_STOPPED);
		//	aux_button_depress_time = 0;
			//pp6_set_synth_note_stop();   // stop the synth
			pp6_turn_off_all_on_notes();
			sendStop();  // send MIDI stop
		}
	}
*/
	// END SEQUENCER


/*
filter_man_control_process(){


}*/



/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


