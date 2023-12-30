/*
 * mode_slicer.h
 *
 *  Created on: Jan 6, 2015
 *      Author: owen
 */

#ifndef MODE_SLICER_H_
#define MODE_SLICER_H_

#include "arm_math.h"

void mode_slicer_init (void);

float32_t mode_slicer_sample_process (void);

void mode_slicer_control_process (void);

#endif /* MODE_SLICER_H_ */
