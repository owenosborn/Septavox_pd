/*
 * mode_mega_wave.h
 *
 *  Created on: Jan 8, 2015
 *      Author: owen
 */

#ifndef MODE_MEGA_WAVE_H_
#define MODE_MEGA_WAVE_H_

#include "arm_math.h"

void mode_mega_wave_init (void);

float32_t mode_mega_wave_sample_process (void);

void mode_mega_wave_control_process (void);

#endif /* MODE_MEGA_WAVE_H_ */
