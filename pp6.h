/*
 * pp6.h
 *
 *  Created on: Jun 23, 2012
 *      Author: owen
 */

#ifndef PP6_H_
#define PP6_H_

#define MODE_LED_BLUE_ON 
#define MODE_LED_RED_ON 
#define MODE_LED_GREEN_ON
#define MODE_LED_BLUE_OFF
#define MODE_LED_RED_OFF
#define MODE_LED_GREEN_OFF

#define AUX_LED_BLUE_ON 
#define AUX_LED_RED_ON 
#define AUX_LED_GREEN_ON 
#define AUX_LED_BLUE_OFF 
#define AUX_LED_RED_OFF 
#define AUX_LED_GREEN_OFF 

#define BLACK 0
#define RED 1
#define YELLOW 2
#define GREEN 3
#define CYAN 4
#define BLUE 5
#define MAGENTA 6

#define MIDI_CLOCK_TIMEOUT 20000  // around 1 second of not receiving midi clock


typedef struct {

	// global transpose
	uint8_t oct_transpose;   // 0 is no transpose

	// knobs
	uint8_t knob_touched[3]; 	// flag to see if knob is touched
	float knob[3];   		// stores knob values 0-1

	// secret mode
	uint8_t secret_mode_enabled;

	// wave synth.  freqs and amps that are shared between all modes
	float freqs[4];
	float amps[4];

	// voices the synth is playing
	uint8_t voices[4];
	uint8_t voices_note_on_flag[4];  // 4 flags for new notes

	uint8_t physical_notes_on;    // the number of non sequenced notes currently on
	uint8_t note_state[128];	  // state of all the midi notes -- 0 for off, anything else for on
	uint8_t note_state_last[128]; // the previos time thru the main loop that notes were updated.  compared with above to get note events

	// keys
	uint8_t keys[43];
	uint8_t keys_last[43];
	uint8_t keys_history[4][43];
	uint8_t num_keys_down;

	// current mode
	uint32_t mode;

	// mode and aux button event flags
	uint8_t mode_button_pressed;	// mode button event flags
	uint8_t mode_button_released;
	uint8_t aux_button_pressed;		// aux button event flags
	uint8_t aux_button_released;

	uint8_t aux_button_count;

	// LEDs
	uint8_t mode_led;
	uint8_t aux_led;
	uint8_t aux_led_flash; 			// these count down to zero, then the flash is over
	uint8_t mode_led_flash;

	// MIDI clock
	uint8_t midi_start_flag;        // midi start command
	uint8_t midi_stop_flag;    		// midi stop command
	uint32_t midi_in_clock_last;   // stores the system time of the last received midi clock
	uint8_t midi_clock_present;  // if a midi clock is currently present
	uint32_t midi_clock_period;  // time in between midi clock ticks
	uint8_t midi_clock_tick_count;
	uint8_t midi_clock_flag;

} pocket_piano;

void pp6_init(void);

void pp6_set_global_octave_transpose(uint8_t);
void pp6_enable_secret_mode(void);

float pp6_get_knob_1(void);
float pp6_get_knob_1_raw(void);
float pp6_get_knob_2(void);
float pp6_get_knob_3(void);
void pp6_set_knob_1(float v);
void pp6_set_knob_2(float v);
void pp6_set_knob_3(float v);
float * pp6_get_knob_array(void);
void pp6_set_knob_array(float * knobs);
void pp6_check_knobs_touched (void);
uint8_t pp6_any_knobs_touched(void);
uint8_t pp6_knob_1_touched(void);
uint8_t pp6_knob_2_touched(void);
uint8_t pp6_knob_3_touched(void);

uint8_t pp6_get_synth_note_start(void);
void pp6_set_synth_note_start (void );
uint8_t pp6_get_synth_note_stop(void);
void pp6_set_synth_note_stop (void );
uint8_t pp6_get_synth_note(void);
void pp6_set_synth_note(uint8_t note);


void pp6_clear_flags(void);

uint8_t pp6_is_playing (void);

uint32_t pp6_get_keys(void);   // returns the current key status
uint8_t pp6_get_num_keys_down(void);
void pp6_get_key_events(void);  // checks keys for new events
void pp6_allocate_voices(void);

void pp6_change_mode(void);
uint32_t pp6_get_mode(void);
void pp6_set_mode(uint32_t mode);

void pp6_keys_init(void);
void pp6_keys_update(void);   // scans and debounces keys
void pp6_leds_init(void);
void pp6_leds_update(uint8_t bank_led, uint8_t mode_led);
void pp6_knobs_init(void);
void pp6_knobs_update(void);
void pp6_smooth_knobs(void);



uint8_t pp6_get_mode_led(void);
uint8_t pp6_get_aux_led(void);
void pp6_set_mode_led(uint8_t mode_led);
void pp6_set_aux_led(uint8_t bank_led);

void pp6_flash_mode_led(uint8_t flash_time);
void pp6_flash_aux_led(uint8_t flash_time);
void pp6_flash_update(void);

void  pp6_set_mode_button_pressed(void);
void pp6_set_mode_button_released(void);
uint8_t pp6_mode_button_pressed(void);
uint8_t pp6_mode_button_released(void);

void  pp6_set_aux_button_pressed(void);
void pp6_set_aux_button_released(void);
uint8_t pp6_aux_button_pressed(void);
uint8_t pp6_aux_button_released(void);

void pp6_inc_physical_notes_on(void);
void pp6_dec_physical_notes_on(void);
uint8_t pp6_get_physical_notes_on(void);

// midi start stop events
void pp6_set_midi_start(void);
uint8_t pp6_get_midi_start(void);
void pp6_set_midi_stop(void);
uint8_t pp6_get_midi_stop(void);

/// MIDI clock stuff
void pp6_midi_clock_tick(void);
void pp6_check_for_midi_clock(void) ;
uint8_t pp6_midi_clock_present(void);
uint32_t pp6_get_midi_clock_period(void);
uint8_t pp6_get_midi_clock_tick(void);
void pp6_clear_midi_clock_tick(void);
uint8_t pp6_get_midi_clock_count(void) ;

// the note interface for the piano
uint8_t pp6_note_on_flag();
uint8_t pp6_note_off_flag();
void pp6_set_note_off(uint8_t note);
void pp6_set_note_on(uint8_t note);
uint8_t pp6_get_note_state(uint8_t note);
uint8_t pp6_get_note_state_last(uint8_t);
void pp6_set_current_note_state_to_last(void);
void pp6_turn_off_all_on_notes(void);


#endif /* PP6_H_ */