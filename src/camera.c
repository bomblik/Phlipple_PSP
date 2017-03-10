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

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "engine.h"
#include "camera.h"

double dir2eye[6][4][3] = { { { 0 } } };

void buildDir2Eye()
{
	// up
	double y = EUS;
	int dir = 0;
	for (int a = 0; a < 4; a++)
	{
		dir2eye[dir][a][0] = cos(((3 - a) * PI / 2) - EYS);
		dir2eye[dir][a][1] = y;
		dir2eye[dir][a][2] = sin(((3 - a) * PI / 2) - EYS);
	}

	// down
	y = EUS;
	dir = 1;
	for (int a = 0; a < 4; a++)
	{
		dir2eye[dir][a][0] = cos(((a) * PI / 2) + EYS);
		dir2eye[dir][a][1] = -y;
		dir2eye[dir][a][2] = sin(((a) * PI / 2) + EYS);
	}

	// north
	dir = 2;
	for (int a = 0; a < 4; a++)
	{
		dir2eye[dir][a][0] = cos(((3 - a) * PI / 2) - EYS);
		dir2eye[dir][a][1] = sin(((3 - a) * PI / 2) - EYS);
		dir2eye[dir][a][2] = -y;
	}

	// south
	dir = 3;
	for (int a = 0; a < 4; a++)
	{
		dir2eye[dir][a][0] = cos(((a) * PI / 2) + EYS);
		dir2eye[dir][a][1] = sin(((a) * PI / 2) + EYS);
		dir2eye[dir][a][2] = y;
	}

	// west
	dir = 4;
	for (int a = 0; a < 4; a++)
	{
		dir2eye[dir][a][0] = -y;
		dir2eye[dir][a][1] = sin(((a) * PI / 2) + EYS);
		dir2eye[dir][a][2] = cos(((a) * PI / 2) + EYS);
	}

	// east
	dir = 5;
	for (int a = 0; a < 4; a++)
	{
		dir2eye[dir][a][0] = y;
		dir2eye[dir][a][1] = sin(((3 - a) * PI / 2) - EYS);
		dir2eye[dir][a][2] = cos(((3 - a) * PI / 2) - EYS);
	}
}

Camera *camera_create(PhlippleEngine *eng)
{
	Camera *cam = malloc(sizeof(Camera));

	cam->eng = eng;
	if (dir2eye[0][0][0] == 0)
	{
		buildDir2Eye();
	}

	interpolator_init(&cam->dist, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS, 0.4);
	interpolator_init(&cam->eyeX, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS, 0.4);
	interpolator_init(&cam->eyeY, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS, 0.4);
	interpolator_init(&cam->eyeZ, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS, 0.4);
	interpolator_init(&cam->centreX, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS,
	                  0.4);
	interpolator_init(&cam->centreY, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS,
	                  0.4);
	interpolator_init(&cam->centreZ, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS,
	                  0.4);
	interpolator_init(&cam->upX, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS, 0.4);
	interpolator_init(&cam->upY, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS, 0.4);
	interpolator_init(&cam->upZ, 0, CAMERA_SPEED, ENGINE_DEFAULT_FPS, 0.4);

	interpolator_init(&cam->lockedMotion, 0, LOCKED_MOTION_SPEED, ENGINE_DEFAULT_FPS, 0.4);

	cam->referenceCoords[0] = NULL;

	for (int f = 0; f < 8; f++)
		cam->zOrderLists[f] = NULL;

	cam->activeQuad = 0;
	cam->inverted = 0;
	cam->currentEyeLookup = 0;
	cam->eyeLookupForMovement = 0;
	cam->rotation = 0;
	cam->rotationForMovement = 0;
	cam->isLockedMotion = 0;
	cam->canLockMotion = 0;
	cam->canMove = 0;
	cam->idle = 0;
	cam->nextCentreQuad = -1;
	cam->nextInverted = 0;

	cam->revFlipping = 0;
	cam->revFlipStep = 0;

	interpolator_setVal(&cam->dist, CAMERA_DEFAULT_DISTANCE);

	return cam;
}

int zorderComparator(const void * elem1, const void * elem2)
{
	Quad * const * q1 = elem1;
	Quad * const * q2 = elem2;

	if ((*q2)->camDistance - (*q1)->camDistance < 0)
		return -1;

	if ((*q2)->camDistance == (*q1)->camDistance)
		return 0;

	return 1;
}

void buildZOrderList(Camera *cam, int ref)
{
	if (cam->zOrderLists[ref] != NULL)
		free(cam->zOrderLists[ref]);

	cam->zOrderLists[ref] = malloc(sizeof(Quad *) * cam->eng->nQuads);

	memcpy(cam->zOrderLists[ref], cam->eng->quads,
	       sizeof(Quad *) * cam->eng->nQuads);

	float camX = cam->referenceCoords[ref]->x;
	float camY = cam->referenceCoords[ref]->y;
	float camZ = cam->referenceCoords[ref]->z;

	for (int q = 0; q < cam->eng->nQuads; q++)
		quad_updateDistanceFromCamera(cam->zOrderLists[ref][q], camX, camY,
		                              camZ);

	qsort(cam->zOrderLists[ref], cam->eng->nQuads, sizeof(Quad *),
	      zorderComparator);
}

void camera_rebuildZOrderLists(Camera *cam)
{
	if (cam->referenceCoords[0] == NULL)
	{
		cam->referenceCoords[0] = vertex_create(0, cam->eng->size, 0);
		cam->referenceCoords[1] = vertex_create(0, cam->eng->size,
		                                        cam->eng->size);
		cam->referenceCoords[2] = vertex_create(cam->eng->size, cam->eng->size,
		                                        cam->eng->size);
		cam->referenceCoords[3] = vertex_create(cam->eng->size, cam->eng->size,
		                                        0);
		cam->referenceCoords[4] = vertex_create(0, 0, 0);
		cam->referenceCoords[5] = vertex_create(0, 0, cam->eng->size);
		cam->referenceCoords[6] = vertex_create(cam->eng->size, 0,
		                                        cam->eng->size);
		cam->referenceCoords[7] = vertex_create(cam->eng->size, 0, 0);
	}

	for (int f = 0; f < 8; f++)
		buildZOrderList(cam, f);
}

void normalizeVector(double x, double y, double z, double *ret)
{
	double l = (sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)));
	ret[0] = x / l;
	ret[1] = y / l;
	ret[2] = z / l;
}

int dir2Ref[2][2][2] = { { { 2, 3 }, { 6, 7 } }, { { 1, 0 }, { 5, 4 } } };

void camera_render(Camera *cam, RenderState *rend)
{
	double norm[3];

	// update GL coords
	rend->centre[0] = (float) cam->centreX.curr;
	rend->centre[1] = (float) cam->centreY.curr;
	rend->centre[2] = (float) cam->centreZ.curr;

	normalizeVector(cam->eyeX.curr, cam->eyeY.curr, cam->eyeZ.curr, norm);

	rend->eye[0] = (float) (cam->centreX.curr + norm[0] * cam->dist.curr);
	rend->eye[1] = (float) (cam->centreY.curr + norm[1] * cam->dist.curr);
	rend->eye[2] = (float) (cam->centreZ.curr + norm[2] * cam->dist.curr);

	rend->up[0] = (float) cam->upX.curr;
	rend->up[1] = (float) cam->upY.curr;
	rend->up[2] = (float) cam->upZ.curr;

	// fix zorder
	int xv = 0;
	int yv = 0;
	int zv = 0;

	if (rend->eye[0] < rend->centre[0])
		xv = 1;

	if (rend->eye[1] < rend->centre[1])
		yv = 1;

	if (rend->eye[2] < rend->centre[2])
		zv = 1;

	rend->zOrderList = cam->zOrderLists[dir2Ref[xv][yv][zv]];
}

static double dir2up[6][3] = { { 0, 1, 0 }, // UP
	{ 0, -1, 0 }, // DOWN
	{ 0, 0, -1 }, // NORTH
	{ 0, 0, 1 }, // EAST
	{ -1, 0, 0 }, // WEST
	{ 1, 0, 0 } // SOUTH
};

static int lookupRotationLookup2Rotation[6][4][6] = { { // ___UP___
		{
			// rot0
			0, // up
			3, // down
			0, // north
			1, // south
			2, // west
			1 // east
		}, { // rot1
			1, // up
			2, // down
			1, // north
			2, // south
			3, // west
			2 // east
		}, { // rot2
			2, // up
			1, // down
			2, // north
			3, // south
			0, // west
			3 // east
		}, { // rot3
			3, // up
			0, // down
			3, // north
			0, // south
			1, // west
			0 // east
		}
	},

	{
		// ___DOWN___
		{
			// rot0
			3, // up
			0, // down
			3, // north
			0, // south
			3, // west
			2 // east
		}, { // rot1
			2, // up
			1, // down
			0, // north
			1, // south
			0, // west
			3 // east
		}, { // rot2
			1, // up
			2, // down
			1, // north
			2, // south
			1, // west
			0 // east
		}, { // rot3
			0, // up
			3, // down
			2, // north
			3, // south
			2, // west
			1 // east
		}
	},

	{
		// ___NORTH___
		{
			// rot0
			0, // up
			1, // down
			0, // north
			3, // south
			3, // west
			0 // east
		}, { // rot1
			1, // up
			2, // down
			1, // north
			2, // south
			0, // west
			1 // east
		}, { // rot2
			2, // up
			3, // down
			2, // north
			1, // south
			1, // west
			2 // east
		}, { // rot3
			3, // up
			0, // down
			3, // north
			0, // south
			2, // west
			3 // east
		}
	},

	{
		// ___SOUTH___
		{
			// rot0
			3, // up
			0, // down
			3, // north
			0, // south
			0, // west
			1 // east
		}, { // rot1
			0, // up
			1, // down
			2, // north
			1, // south
			1, // west
			2 // east
		}, { // rot2
			1, // up
			2, // down
			1, // north
			2, // south
			2, // west
			3 // east
		}, { // rot3
			2, // up
			3, // down
			0, // north
			3, // south
			3, // west
			0 // east
		}
	},

	{
		// ___WEST___
		{
			// rot0
			2, // up
			1, // down
			1, // north
			0, // south
			0, // west
			3 // east
		}, { // rot1
			3, // up
			2, // down
			2, // north
			1, // south
			1, // west
			2 // east
		}, { // rot2
			0, // up
			3, // down
			3, // north
			2, // south
			2, // west
			1 // east
		}, { // rot3
			1, // up
			0, // down
			0, // north
			3, // south
			3, // west
			0 // east
		}
	},

	{
		// ___EAST___
		{
			// rot0
			3, // up
			2, // down
			0, // north
			3, // south
			3, // west
			0 // east
		}, { // rot1
			0, // up
			3, // down
			1, // north
			0, // south
			2, // west
			1 // east
		}, { // rot2
			1, // up
			0, // down
			2, // north
			1, // south
			1, // west
			2 // east
		}, { // rot3
			2, // up
			1, // down
			3, // north
			2, // south
			0, // west
			3 // east
		}
	}
};

int rotLookup[3][4] = { { 0, 2, 1, 3 }, // x - top, north, down, south
	{ 2, 4, 3, 5 }, // y - north, west, south, east
	{ 0, 4, 1, 5 } // z - top, west, down, east
};

void camera_setActiveQuad(Camera *cam, int quad, int inverted, int immediate)
{
	Quad *q = (cam->eng->quads[quad]);
	if (!cam->idle && !immediate) // another move queued while moving
	{
		cam->nextCentreQuad = quad;
		cam->nextInverted = inverted;

		int dir = 0;

		if (q->plane == PLANE_Y)
			dir = DIRECTION_UP;

		if (q->plane == PLANE_X)
			dir = DIRECTION_WEST;

		if (q->plane == PLANE_Z)
			dir = DIRECTION_NORTH;

		if (cam->inverted)
			dir++;

		/*if (revFlipping && revFlippingForMovement)
		 {
		 dir = revFlipLookups[0];
		 revFlippingForMovement = false;
		 System.out.println("THis is the case!");
		 }
		 */
		//rotationForMovement = lookupRotationLookup2Rotation[dir][rotationForMovement][dir];
		cam->rotationForMovement
		= lookupRotationLookup2Rotation[cam->eyeLookupForMovement][cam->rotationForMovement][dir];
		cam->eyeLookupForMovement = dir;

		return;
	}

	Quad *centreQuad = cam->eng->quads[cam->activeQuad];

	if (!cam->revFlipping)
		if (centreQuad->plane == q->plane && inverted != cam->inverted)
		{
			// reflipped
			int rfdir = PLANE_X;
			int rotAx = PLANE_X;

			switch (q->plane)
			{
			case PLANE_X:
				if (q->centre[1] != centreQuad->centre[1])
				{
					rfdir = PLANE_Y;
					rotAx = PLANE_Z;
				}
				else
				{
					rfdir = PLANE_Z;
					rotAx = PLANE_Y;
				}

				break;
			case PLANE_Y:
				if (q->centre[0] != centreQuad->centre[0])
				{
					rfdir = PLANE_X;
					rotAx = PLANE_Z;
				}
				else
				{
					rfdir = PLANE_Z;
					rotAx = PLANE_X;
				}

				break;
			case PLANE_Z:
				if (q->centre[1] != centreQuad->centre[1])
				{
					rfdir = PLANE_Y;
					rotAx = PLANE_X;
				}
				else
				{
					rfdir = PLANE_X;
					rotAx = PLANE_Y;
				}

				break;
			}

			int rfcw = 0;
			if (q->centre[rfdir] > centreQuad->centre[rfdir])
				rfcw = 1;

			cam->revFlipping = 1;
			cam->revFlipStep = 0;

			int rotOffs = 0;
			for (int f = 0; f < 4; f++)
			{
				if (rotLookup[rotAx][f] == cam->currentEyeLookup)
					rotOffs = f;
			}

			if (rfcw)
			{
				rotOffs++;
			}
			else
				rotOffs--;

			if (rotOffs == 4)
				rotOffs = 0;

			if (rotOffs == -1)
				rotOffs = 3;

			cam->revFlipLookups[0] = rotLookup[rotAx][rotOffs];

			if (rfcw)
			{
				rotOffs++;
			}
			else
				rotOffs--;

			if (rotOffs == 4)
				rotOffs = 0;

			if (rotOffs == -1)
				rotOffs = 3;

			cam->revFlipLookups[1] = rotLookup[rotAx][rotOffs];

			cam->rotationForMovement
			= lookupRotationLookup2Rotation[cam->eyeLookupForMovement][cam->rotationForMovement][cam->revFlipLookups[1]];
			cam->eyeLookupForMovement = cam->revFlipLookups[1];
		}

	cam->activeQuad = quad;
	cam->inverted = inverted;

	cam->canLockMotion = 0;
	cam->isLockedMotion = 0;

	if (!immediate)
	{
		interpolator_setVal(&cam->centreX, q->centre[0]);
		interpolator_setVal(&cam->centreY, q->centre[1]);
		interpolator_setVal(&cam->centreZ, q->centre[2]);
	}
	else
	{
		interpolator_setValImmediate(&cam->centreX, q->centre[0]);
		interpolator_setValImmediate(&cam->centreY, q->centre[1]);
		interpolator_setValImmediate(&cam->centreZ, q->centre[2]);
	}

	// eye
	int dir = 0;

	if (q->plane == PLANE_Y)
		dir = DIRECTION_UP;

	if (q->plane == PLANE_X)
		dir = DIRECTION_WEST;

	if (q->plane == PLANE_Z)
		dir = DIRECTION_NORTH;

	if (inverted)
		dir++;

	if (cam->revFlipping)
		dir = cam->revFlipLookups[0];

	int
	rot =
	    lookupRotationLookup2Rotation[cam->currentEyeLookup][cam->rotation][dir];

	if (!immediate)
	{
		interpolator_setVal(&cam->eyeX, dir2eye[dir][rot][0]);
		interpolator_setVal(&cam->eyeY, dir2eye[dir][rot][1]);
		interpolator_setVal(&cam->eyeZ, dir2eye[dir][rot][2]);

		interpolator_setVal(&cam->upX, dir2up[dir][0]);
		interpolator_setVal(&cam->upY, dir2up[dir][1]);
		interpolator_setVal(&cam->upZ, dir2up[dir][2]);

	}
	else
	{
		interpolator_setValImmediate(&cam->eyeX, dir2eye[dir][rot][0]);
		interpolator_setValImmediate(&cam->eyeY, dir2eye[dir][rot][1]);
		interpolator_setValImmediate(&cam->eyeZ, dir2eye[dir][rot][2]);

		interpolator_setValImmediate(&cam->upX, dir2up[dir][0]);
		interpolator_setValImmediate(&cam->upY, dir2up[dir][1]);
		interpolator_setValImmediate(&cam->upZ, dir2up[dir][2]);
	}

	cam->rotation = rot;

	if (!cam->revFlipping)
	{
		cam->rotationForMovement = cam->rotation;
		cam->eyeLookupForMovement = dir;
	}

	cam->currentEyeLookup = dir;
}

void camera_setRotation(Camera *cam, int rot, int immediate)
{
	cam->rotation = rot;
	cam->rotationForMovement = cam->rotation;
	if (!immediate)
	{
		interpolator_setVal(&cam->eyeX, dir2eye[cam->currentEyeLookup][rot][0]);
		interpolator_setVal(&cam->eyeY, dir2eye[cam->currentEyeLookup][rot][1]);
		interpolator_setVal(&cam->eyeZ, dir2eye[cam->currentEyeLookup][rot][2]);
	}
	else
	{
		interpolator_setValImmediate(&cam->eyeX, dir2eye[cam->currentEyeLookup][rot][0]);
		interpolator_setValImmediate(&cam->eyeY, dir2eye[cam->currentEyeLookup][rot][1]);
		interpolator_setValImmediate(&cam->eyeZ, dir2eye[cam->currentEyeLookup][rot][2]);
	}

}

void camera_animate(Camera *cam, double timeElapsed)
{
	interpolator_animate(&cam->dist, timeElapsed);
	interpolator_animate(&cam->eyeX, timeElapsed);
	interpolator_animate(&cam->eyeY, timeElapsed);
	interpolator_animate(&cam->eyeZ, timeElapsed);
	interpolator_animate(&cam->centreX, timeElapsed);
	interpolator_animate(&cam->centreY, timeElapsed);
	interpolator_animate(&cam->centreZ, timeElapsed);
	interpolator_animate(&cam->upX, timeElapsed);
	interpolator_animate(&cam->upY, timeElapsed);
	interpolator_animate(&cam->upZ, timeElapsed);
	interpolator_animate(&cam->lockedMotion, timeElapsed);

	cam->canMove = 1;

	if (cam->revFlipping && cam->revFlipStep == 0)
	{
		cam->canMove = 0;
	}

	if (cam->upX.idle && cam->upY.idle && cam->upZ.idle && cam->eyeX.idle
	        && cam->eyeY.idle && cam->eyeZ.idle)
	{
		cam->idle = 1;
	}
	else
	{
		cam->idle = 0;
	}

	if (cam->revFlipping && cam->idle)
	{
		if (cam->revFlipStep == 0)
		{
			//System.out.println("Step2");
			cam->revFlipStep++;
			int dir = cam->revFlipLookups[1];
			int
			rot =
			    lookupRotationLookup2Rotation[cam->currentEyeLookup][cam->rotation][dir];

			interpolator_setVal(&cam->eyeX, dir2eye[dir][rot][0]);
			interpolator_setVal(&cam->eyeY, dir2eye[dir][rot][1]);
			interpolator_setVal(&cam->eyeZ, dir2eye[dir][rot][2]);

			interpolator_setVal(&cam->upX, dir2up[dir][0]);
			interpolator_setVal(&cam->upY, dir2up[dir][1]);
			interpolator_setVal(&cam->upZ, dir2up[dir][2]);

			cam->rotation = rot;
			cam->rotationForMovement = cam->rotation;
			cam->eyeLookupForMovement = dir;
			cam->currentEyeLookup = dir;
		}
		else
		{
			cam->revFlipping = 0;
		}
	}

	if (!cam->revFlipping)
		if (cam->idle && cam->nextCentreQuad != -1)
		{
			camera_setActiveQuad(cam, cam->nextCentreQuad, cam->nextInverted, 0);
			cam->nextCentreQuad = -1;
		}

	if (cam->isLockedMotion)
	{
		interpolator_setValImmediate(&cam->eyeX,
		                             cam->lockedXStart + cam->lockedXVel * cam->lockedMotion.curr);
		interpolator_setValImmediate(&cam->eyeY,
		                             cam->lockedYStart + cam->lockedYVel * cam->lockedMotion.curr);
		interpolator_setValImmediate(&cam->eyeZ,
		                             cam->lockedZStart + cam->lockedZVel * cam->lockedMotion.curr);

	}

	if (!cam->canLockMotion)
		if (cam->eyeX.idle && cam->eyeY.idle && cam->eyeZ.idle)
		{
			cam->canLockMotion = 1;
		}

}

void camera_destroy(Camera *cam)
{
	interpolator_free(&cam->dist);
	interpolator_free(&cam->eyeX);
	interpolator_free(&cam->eyeY);
	interpolator_free(&cam->eyeZ);
	interpolator_free(&cam->centreX);
	interpolator_free(&cam->centreY);
	interpolator_free(&cam->centreZ);
	interpolator_free(&cam->upX);
	interpolator_free(&cam->upY);
	interpolator_free(&cam->upZ);
	interpolator_free(&cam->lockedMotion);

	if (cam->referenceCoords[0] != NULL)   // zorder lists have been builf
	{
		for (int f = 0; f < 8; f++)
		{
			vertex_destroy(cam->referenceCoords[f]);
			free(cam->zOrderLists[f]);
		}
	}
	free(cam);
}

void fireLockedMotion(Camera *cam)
{
	cam->rotationForMovement = cam->rotation;
	if (cam->canLockMotion)
	{
		cam->isLockedMotion = 1;

		cam->lockedXVel = dir2eye[cam->currentEyeLookup][cam->rotation][0]
		                  - cam->eyeX.curr;
		cam->lockedYVel = dir2eye[cam->currentEyeLookup][cam->rotation][1]
		                  - cam->eyeY.curr;
		cam->lockedZVel = dir2eye[cam->currentEyeLookup][cam->rotation][2]
		                  - cam->eyeZ.curr;
		cam->lockedXStart = cam->eyeX.curr;
		cam->lockedYStart = cam->eyeY.curr;
		cam->lockedZStart = cam->eyeZ.curr;

		interpolator_setValImmediate(&cam->lockedMotion, 0);
		interpolator_setVal(&cam->lockedMotion, 1);
	}

	interpolator_setVal(&cam->eyeX,
	                    dir2eye[cam->currentEyeLookup][cam->rotation][0]);
	interpolator_setVal(&cam->eyeY,
	                    dir2eye[cam->currentEyeLookup][cam->rotation][1]);
	interpolator_setVal(&cam->eyeZ,
	                    dir2eye[cam->currentEyeLookup][cam->rotation][2]);
}

void camera_rol(Camera *cam)
{
	if (cam->isLockedMotion && !cam->lockedMotion.idle)
	{
		return;
	}

	cam->rotation--;
	if (cam->rotation < 0)
		cam->rotation = 3;

	fireLockedMotion(cam);
}

void camera_ror(Camera *cam)
{
	if (cam->isLockedMotion && !cam->lockedMotion.idle)
	{
		return;
	}

	cam->rotation++;
	if (cam->rotation > 3)
		cam->rotation = 0;

	fireLockedMotion(cam);
}
