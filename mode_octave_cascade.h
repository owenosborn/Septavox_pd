/*
 * mode_octave_cascade.h
 *
 *  Created on: Jan 6, 2015
 *      Author: owen
 */

#ifndef MODE_OCTAVE_CASCADE_H_
#define MODE_OCTAVE_CASCADE_H_

#include "arm_math.h"

void mode_octave_cascade_init(void);

float32_t mode_octave_cascade_sample_process (void) ;

void mode_octave_cascade_control_process (void);




#endif /* MODE_OCTAVE_CASCADE_H_ */
