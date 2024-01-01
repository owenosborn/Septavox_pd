/*
 * wave_synth.h
 *
 *  Created on: Jan 1, 2015
 *      Author: owen
 */

#ifndef WAVE_SYNTH_H_
#define WAVE_SYNTH_H_

typedef struct {
    sin_oscillator sins[4];
    bl_saw saws[4];
    bl_square squares[4];
	float freqs[4];
	float amps[4];
    uint8_t wavetable_selector;
    float tune;
} wave_synth;

void wave_synth_init(wave_synth *ws);

float wave_synth_process(wave_synth *ws);

#endif /* WAVE_SYNTH_H_ */
