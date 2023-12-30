/*
 * mode_adsr.h
 *
 *  Created on: Dec 11, 2014
 *      Author: owen
 */

#ifndef MODE_ADSR_H_
#define MODE_ADSR_H_


#include "arm_math.h"

void mode_adsr_init (void);

float32_t mode_adsr_sample_process (void);

void mode_adsr_control_process (void);


#endif /* MODE_ADSR_H_ */
