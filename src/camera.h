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

#ifndef CAMERA_H_
#define CAMERA_H_

#include "engine.h"
#include "interpolator.h"
#include "renderstate.h"

#define CAMERA_SPEED 			0.15
#define LOCKED_MOTION_SPEED		0.15
#define CAMERA_DEFAULT_DISTANCE	5
// eye yaw shift
#define EYS						0.3926990816987241
// eye up shift
#define EUS						.5
#define PI						3.14159265358979323846

struct _Camera
{
	Interpolator dist;
	Interpolator eyeX;
	Interpolator eyeY;
	Interpolator eyeZ;
	Interpolator centreX;
	Interpolator centreY;
	Interpolator centreZ;
	Interpolator upX;
	Interpolator upY;
	Interpolator upZ;
	Interpolator lockedMotion;

	int currentEyeLookup;
	int rotation;
	int rotationForMovement;
	int eyeLookupForMovement;
	int activeQuad;
	int inverted;

	int nextCentreQuad;
	int nextInverted;

	int revFlipping;
	int revFlipStep;
	int revFlipLookups[12];

	int isLockedMotion;
	int canLockMotion;
	int canMove;
	int idle;

	float lockedXVel;
	float lockedYVel;
	float lockedZVel;
	float lockedXStart;
	float lockedYStart;
	float lockedZStart;

	struct _PhlippleEngine *eng;
	Vertex *referenceCoords[8];
	Quad **zOrderLists[8];

	void *vertVBO;
	void *indVBO;
};

typedef struct _Camera Camera;
Camera *camera_create(struct _PhlippleEngine *);
void camera_animate(Camera *, double timeElapsed);
void camera_rebuildZOrderLists(Camera *);
void camera_setActiveQuad(Camera *, int quad, int inverted, int immediate);
void camera_setRotation(Camera *, int rotation, int immediate);
void camera_render(Camera *, RenderState *);
void camera_rol(Camera *);
void camera_ror(Camera *);

void camera_destroy(Camera *);


#endif /* CAMERA_H_ */
