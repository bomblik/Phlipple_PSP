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
#include <string.h>
#include <math.h>

#include "quad.h"
#include "vertex.h"
#include "engine.h"

void quad_assignToEngine(Quad *q, struct _Vertex **v)
{
	q->verts = malloc(sizeof(Vertex *) * 4);
	memcpy(q->verts, v, sizeof(Vertex *) * 4);

}

void quad_updateHardlinks(Quad *q)
{
	q->hardLinks[HARDLINK_EAST] = vertex_isLinked(q->verts[1], q->verts[2]);
	q->hardLinks[HARDLINK_NORTH] = vertex_isLinked(q->verts[0], q->verts[1]);
	q->hardLinks[HARDLINK_SOUTH] = vertex_isLinked(q->verts[2], q->verts[3]);
	q->hardLinks[HARDLINK_WEST] = vertex_isLinked(q->verts[0], q->verts[3]);
}

void quad_updateDistanceFromCamera(Quad *q, float cx, float cy, float cz)
{
	q->camDistance = (float) (fabs(pow(cx - q->centre[0], 2)) + fabs(
	                              pow(cy - q->centre[1], 2)) + fabs(pow(cz - q->centre[2], 2)));
}

void calculateVertsFromVector(Quad *q, int x1, int y1, int z1, int x3, int y3,
                              int z3)
{
	int nPlanes = 0;
	int plane = 0;

	if (x1 == x3)
	{
		plane = PLANE_X;
		nPlanes++;
	}

	if (y1 == y3)
	{
		plane = PLANE_Y;
		nPlanes++;
	}

	if (z1 == z3)
	{
		plane = PLANE_Z;
		nPlanes++;
	}

	int swapY = 0;
	int swapX = 0;
	int swapZ = 0;

	if (plane == PLANE_X)
	{
		if (y1 < y3)
			swapY = 1;

		if (z1 < z3)
			swapZ = 1;
	}

	if (plane == PLANE_Y)
	{
		if (x1 < x3)
			swapX = 1;

		if (z1 < z3)
			swapZ = 1;
	}

	if (plane == PLANE_Z)
	{
		if (x1 < x3)
			swapX = 1;

		if (y1 < y3)
			swapY = 1;
	}

	if (swapX)
	{
		int tx = x1;
		x1 = x3;
		x3 = tx;
	}

	if (swapY)
	{
		int ty = y1;
		y1 = y3;
		y3 = ty;
	}

	if (swapZ)
	{
		int tz = z1;
		z1 = z3;
		z3 = tz;
	}

	// calculate remaining points

	Vertex **p = q->tmpVerts;

	p[0] = vertex_create(x1, y1, z1);
	p[2] = vertex_create(x3, y3, z3);

	if (plane == PLANE_X)
	{
		p[1] = vertex_create(x1, y1, z3);
		p[3] = vertex_create(x1, y3, z1);
	}

	if (plane == PLANE_Y)
	{
		p[1] = vertex_create(x1, y1, z3);
		p[3] = vertex_create(x3, y1, z1);
	}

	if (plane == PLANE_Z)
	{
		p[1] = vertex_create(x3, y1, z1);
		p[3] = vertex_create(x1, y3, z1);
	}
}

void calculateCentre(Quad *q)
{
	q->centre[0] = (q->tmpVerts[0]->x + q->tmpVerts[2]->x) / 2.0f;
	q->centre[1] = (q->tmpVerts[0]->y + q->tmpVerts[2]->y) / 2.0f;
	q->centre[2] = (q->tmpVerts[0]->z + q->tmpVerts[2]->z) / 2.0f;
}

void calculatePlane(Quad *q)
{
	if (q->tmpVerts[0]->x == q->tmpVerts[2]->x)
	{
		q->plane = PLANE_X;
	}

	if (q->tmpVerts[0]->y == q->tmpVerts[2]->y)
	{
		q->plane = PLANE_Y;
	}

	if (q->tmpVerts[0]->z == q->tmpVerts[2]->z)
	{
		q->plane = PLANE_Z;
	}
}

void calculateVertsForDrawing(Quad *q)
{

	float *r = q->vertsForDrawing;
	float o = VERTEX_DRAW_INNER_OFFSET;

	r[0] = q->tmpVerts[0]->x;
	r[1] = q->tmpVerts[0]->y;
	r[2] = q->tmpVerts[0]->z;

	r[3] = q->tmpVerts[1]->x;
	r[4] = q->tmpVerts[1]->y;
	r[5] = q->tmpVerts[1]->z;

	r[6] = q->tmpVerts[2]->x;
	r[7] = q->tmpVerts[2]->y;
	r[8] = q->tmpVerts[2]->z;

	r[9] = q->tmpVerts[3]->x;
	r[10] = q->tmpVerts[3]->y;
	r[11] = q->tmpVerts[3]->z;

	if (q->plane == PLANE_X)
	{
		r[1] -= o;
		r[2] -= o;

		r[4] -= o;
		r[5] += o;

		r[7] += o;
		r[8] += o;

		r[10] += o;
		r[11] -= o;
	}

	if (q->plane == PLANE_Y)
	{
		r[0] -= o;
		r[2] -= o;

		r[3] -= o;
		r[5] += o;

		r[6] += o;
		r[8] += o;

		r[9] += o;
		r[11] -= o;
	}

	if (q->plane == PLANE_Z)
	{
		r[0] -= o;
		r[1] -= o;

		r[3] += o;
		r[4] -= o;

		r[6] += o;
		r[7] += o;

		r[9] -= o;
		r[10] += o;
	}
}

Quad *quad_create(int x1, int y1, int z1, int x3, int y3, int z3)
{
	Quad *q = malloc(sizeof(Quad));

	q->verts = 0;
	q->tmpVerts[0] = 0;
	q->tmpVerts[1] = 0;
	q->tmpVerts[2] = 0;
	q->tmpVerts[3] = 0;
	q->live = 1;
	q->accessible = 1;
	q->stuck = 0;

	calculateVertsFromVector(q, x1, y1, z1, x3, y3, z3);
	calculateCentre(q);
	calculatePlane(q);
	calculateVertsForDrawing(q);

	for (int f = 0; f < 4; f++)
		q->hardLinks[f] = 0;

	return q;
}

void quad_destroy(Quad *q)
{
	if (q->verts)
		free(q->verts);
	for (int f = 0; f < 4; f++)
		if (q->tmpVerts[f])
			vertex_destroy(q->tmpVerts[f]);

	free(q);
}

void quad_getAdherentQuads(Quad *q, int dir, Quad **ret)
{
	Vertex *v1 = NULL;
	Vertex *v2 = NULL;

	switch (dir)
	{
	case HARDLINK_NORTH:
		v1 = q->verts[0];
		v2 = q->verts[1];
		break;

	case HARDLINK_SOUTH:
		v1 = q->verts[2];
		v2 = q->verts[3];
		break;

	case HARDLINK_EAST:
		v1 = q->verts[1];
		v2 = q->verts[2];
		break;

	case HARDLINK_WEST:
		v1 = q->verts[0];
		v2 = q->verts[3];
		break;
	}

	for (int f = 0; f < 12; f++)
		ret[f] = NULL;

	if (v1 == NULL || v2 == NULL)
		return;

	int retInd = 0;
	for (int f = 0; f < v1->nLinkedQuads; f++)
	{
		if (v1->quads[f] != q)
			for (int g = 0; g < v2->nLinkedQuads; g++)
				if (v1->quads[f] == v2->quads[g])
				{
					if (v1->quads[f]->live)
						ret[retInd++] = v1->quads[f];
				}
	}
}

int quad_isMoveFree(Quad *q, int dir)
{
	// count hardlinks
	int hl = 0;

	hl += q->hardLinks[0] ? 1 : 0;
	hl += q->hardLinks[1] ? 1 : 0;
	hl += q->hardLinks[2] ? 1 : 0;
	hl += q->hardLinks[3] ? 1 : 0;

	if (hl == 0)
		return 1;

	if (hl > 1)
		return 0;

	// is other dir than this hardlinked?
	if (!q->hardLinks[dir])
		return 0;

	return 1;
}

int hasAdherentQuads(Quad *q, int direction)
{
	Vertex *v1 = NULL;
	Vertex *v2 = NULL;

	switch (direction)
	{
	case HARDLINK_NORTH:
		v1 = q->verts[0];
		v2 = q->verts[1];
		break;

	case HARDLINK_SOUTH:
		v1 = q->verts[2];
		v2 = q->verts[3];
		break;

	case HARDLINK_EAST:
		v1 = q->verts[1];
		v2 = q->verts[2];
		break;

	case HARDLINK_WEST:
		v1 = q->verts[0];
		v2 = q->verts[3];
		break;
	}

	if (v1 == NULL || v2 == NULL)
		return 0;

	int retInd = 0;
	for (int f = 0; f < v1->nLinkedQuads; f++)
	{
		if (v1->quads[f] != q)
			for (int g = 0; g < v2->nLinkedQuads; g++)
				if (v1->quads[f] == v2->quads[g])
				{
					if (v1->quads[f]->live)
						retInd++;
				}
	}

	if (retInd != 0)
		return 1;

	return 0;
}

int adh[4];

void quad_makeActive(Quad *q)
{

	q->stuck = 0;
	int hl = 0;

	quad_updateHardlinks(q);

	for (int f = 0; f < 4; f++)
		adh[f] = hasAdherentQuads(q, f);

	// update hardlinks
	for (int f = 0; f < 4; f++)
	{
		if (!adh[f])
			q->hardLinks[f] = 0;

		if (q->hardLinks[f])
			hl++;
	}

	// more than one hardlink - no way out !
	if (hl > 1)
		q->stuck = 1;

	// are we stranded ?
	int adhcnt = 0;
	for (int f = 0; f < 4; f++)
		adhcnt += adh[f] ? 1 : 0;

	if (adhcnt == 0)
		q->stuck = 1;
}
