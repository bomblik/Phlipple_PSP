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

#ifndef QUAD_H_
#define QUAD_H_

#include "vertex.h"

#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2

struct _Quad
{
	struct _Vertex **verts;
	struct _Vertex *tmpVerts[4];
	float vertsForDrawing[12];
	float centre[3];
	int plane;
	int hardLinks[4];
	int live;
	int accessible;
	short lineVBOoffset;
	short quadVBOoffset;
	int stuck;

	float camDistance;
};

typedef struct _Quad Quad;

Quad *quad_create(int x1, int y1, int z1, int x3, int y3, int z3);
void quad_destroy(Quad *);
void quad_assignToEngine(Quad *, struct _Vertex **realVerts);
void quad_updateHardlinks(Quad *);
void quad_updateDistanceFromCamera(Quad *, float x, float y, float z);
void quad_getAdherentQuads(Quad *, int dir, Quad **);
int quad_isMoveFree(Quad *, int dir);
void quad_makeActive(Quad *);
int quad_hasAdherentQuads(Quad *, int dir);

#endif /* QUAD_H_ */
