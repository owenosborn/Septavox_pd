/*
 * mode_slider.h
 *
 *  Created on: Jan 7, 2015
 *      Author: owen
 */

#ifndef MODE_SLIDER_H_
#define MODE_SLIDER_H_

#include "arm_math.h"

void mode_slider_init (void);

float32_t mode_slider_sample_process (void);

void mode_slider_control_process (void);

void mode_slider_stop_notes(void);

#endif /* MODE_SLIDER_H_ */
