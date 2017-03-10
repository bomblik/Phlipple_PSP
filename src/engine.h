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

#ifndef PHLIPPLE_H_
#define PHLIPPLE_H_

#include <stdio.h>
#include "vertex.h"
#include "quad.h"
#include "camera.h"

#define ENGINE_DEFAULT_FPS			100

#define HARDLINK_NORTH 				0
#define HARDLINK_SOUTH 				1
#define HARDLINK_WEST 				2
#define HARDLINK_EAST 				3

#define DIRECTION_UP				0
#define DIRECTION_DOWN				1
#define DIRECTION_NORTH				2
#define DIRECTION_EAST				3
#define DIRECTION_WEST				4
#define DIRECTION_SOUTH				5

#define AXIS_X						0
#define AXIS_Y						1
#define AXIS_Z						2

#define VERTEX_DRAW_INNER_OFFSET 	0.15f

char debugOutput[1024];

#include "renderstate.h"

struct _PhlippleEngine
{
	int size;
	int nQuads;
	int nLinks;
	int nVerts;
	int startInverted;
	int startQuad;
	float startZoom;
	int startRotation;
	Vertex **verts;
	Quad **quads;
	Quad **quadsIsland;
	int traceOffset;
	int activeQuad;
	Camera *camera;

	int inverted;

	float *vertBuffer;
	size_t vertBufferSize;
	unsigned short *indBuffer;
	size_t indBufferSize;

	Quad *zeroQuad;

	// flip anim related
	Vertex *flipv1;
	Vertex *flipv2;
	int flipcw;
	int flipanim;
	double flipr;
	int flipaxs;
	double flipstart;
	double flipend;

	// read those after animating
	int failed;
	int finished;
	int stuck;
	int buzzSound;
	int flippedSound;

};

typedef struct _PhlippleEngine PhlippleEngine;

#define FILE_MAGIC "PHLIPPLELPPILHP"

PhlippleEngine *phlipple_engine_create(char *resname);
void phlipple_engine_render(PhlippleEngine *, RenderState *);
void phlipple_engine_restart(PhlippleEngine *);
// time is given in double seconds
void phlipple_engine_animate(PhlippleEngine *, double);
void phlipple_engine_rol(PhlippleEngine *);
void phlipple_engine_ror(PhlippleEngine *);
void phlipple_engine_up(PhlippleEngine *);
void phlipple_engine_down(PhlippleEngine *);
void phlipple_engine_left(PhlippleEngine *);
void phlipple_engine_right(PhlippleEngine *);


void phlipple_engine_destroy(PhlippleEngine *);

#endif /* PHLIPPLE_H_ */
