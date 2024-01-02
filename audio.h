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


#define SR 24000.0f
#define KR 10000.0f
#define TWO_PI 6.283185307f

float c_to_f(float c);
float c_to_f_ratio(float c);

#endif /* AUDIO_H_ */
