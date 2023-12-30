/*
 * audio.h
 *
 *  Created on: Apr 1, 2012
 *      Author: owen
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#define ABS(a)	   (((a) < 0) ? -(a) : (a))

//#define BASE_FREQ 32.70f   // C1
//#define BASE_FREQ 65.4063913251f   // C1
#define BASE_FREQ 8.1757989156f   // midi note 0

//#define BASE_FREQ 220.f   // A


//#define SR 22374.0f
#define NYQUIST 5000.0f
#define SR 11200.0f
//#define KR 1378.125f // sr / 16
#define KR 11200.0f // sr / 16
#define TWO_PI 6.283185307f

float c_to_f(float c);
float c_to_f_ratio(float c);

#endif /* AUDIO_H_ */
