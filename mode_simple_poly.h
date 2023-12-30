/*
 * mode_simple_poly.h
 *
 *  Created on: Oct 18, 2013
 *      Author: owen
 */

#ifndef MODE_SIMPLE_POLY_H_
#define MODE_SIMPLE_POLY_H_

#include "arm_math.h"

void mode_simple_poly_init (void);

float32_t mode_simple_poly_sample_process (void);

void mode_simple_poly_control_process (void);

#endif /* MODE_SIMPLE_POLY_H_ */
