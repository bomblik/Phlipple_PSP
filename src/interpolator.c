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

#include <malloc.h>
#include <math.h>
#include <stdlib.h>

#include "interpolator.h"
#include "engine.h"

void interpolator_initDefault(Interpolator *i)
{
	interpolator_init(i, 0, INTERPOLATOR_DEFAULT_SPEED, ENGINE_DEFAULT_FPS,
	                  INTERPOLATOR_DEFAULT_EASE);
}

void interpolator_setVal(Interpolator *i, double val)
{
	i->dest = val;
}

void interpolator_setValImmediate(Interpolator *i, double val)
{
	i->dest = i->curr = val;
	i->acc = i->vel = 0;
}

double interpolator_getVal(Interpolator *i)
{
	return i->curr;
}

void animateSingleFrame(Interpolator *i)
{
	i->idle = 0;

	if (fabs(i->curr - i->dest) < i->step)
	{
		i->curr = i->dest;
		i->vel = 0;
		i->idle = 1;
		return;
	}

	i->acc = 0;

	if (i->dest > i->curr)
	{
		i->acc = 1;
	}

	if (i->dest < i->curr)
	{
		i->acc = -1;
	}

	if (i->vel > 0)
	{
		if (i->curr + i->velocities[i->vel].easeLength > i->dest)
			i->acc = -2;
	}

	if (i->vel < 0)
	{
		if (i->curr - i->velocities[abs(i->vel)].easeLength < i->dest)
			i->acc = 2;
	}

	i->vel += i->acc;
	if (i->vel > i->steps)
		i->vel = i->steps;

	if (i->vel < -i->steps)
		i->vel = -i->steps;

	if (i->vel > 0)
		i->curr += i->velocities[i->vel].velocity;

	if (i->vel < 0)
		i->curr -= i->velocities[abs(i->vel)].velocity;

}

void interpolator_animate(Interpolator *i, double timeElapsed)
{
	int fr = (int) floor((double) i->fps * timeElapsed);

	if (fr < 1)
		fr = 1;

	for (int f = 0; f < fr; f++)
		animateSingleFrame(i);
}

void interpolator_init(Interpolator *i, double val, double speed, int fps,
                       double ease)
{
	i->fps = fps;
	i->speed = speed;
	i->curr = i->dest = val;
	i->steps = (int) (fps * ease);
	i->step = i->speed / (double) i->steps;
	i->nVelocities = i->steps + 1;
	i->vel = 0;

	double e = 0;

	i->velocities = malloc(sizeof(struct _VelocityInfo) * i->nVelocities);

	for (int s = 0; s < i->steps + 1; s++)
	{
		e += s * i->step;
		i->velocities[s].velocity = s * i->step;
		i->velocities[s].easeLength = e;
	}
}

void interpolator_free(Interpolator *i)
{
	free(i->velocities);
}

