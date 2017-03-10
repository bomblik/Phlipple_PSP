/*
 * Phlipple
 * Copyright (C) Remigiusz Dybka 2011 <remigiusz.dybka@gmail.com>
 *
 Phlipple is free software: you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by the
 Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Phlipple is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INTERPOLATOR_H_
#define INTERPOLATOR_H_

#include <math.h>

#define INTERPOLATOR_DEFAULT_SPEED		0.1
#define INTERPOLATOR_DEFAULT_EASE		0.4

struct _VelocityInfo
{
	double velocity;
	double easeLength;
};

struct _Interpolator
{
	int idle;
	double curr;

	double dest;
	double step;
	double speed;
	int vel;
	int acc;
	int fps;
	int steps;
	struct _VelocityInfo *velocities;
	int nVelocities;
};

typedef struct _Interpolator Interpolator;

void interpolator_initDefault(Interpolator *);
void interpolator_init(Interpolator *, double val, double speed, int fps, double ease);
void interpolator_free(Interpolator *);
void interpolator_setVal(Interpolator *, double);
void interpolator_setValImmediate(Interpolator *, double);
double interpolator_getVal(Interpolator *);
void interpolator_animate(Interpolator *, double timeElapsed);


#endif /* INTERPOLATOR_H_ */
