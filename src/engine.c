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
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "engine.h"
#include "osinterface.h"

void buildVBOs(PhlippleEngine *eng)
{
	eng->vertBufferSize = (1 + eng->nQuads) * sizeof(float) * 24;
	eng->vertBuffer = malloc(eng->vertBufferSize);

	eng->indBufferSize = (1 + eng->nQuads) * sizeof(short) * 14;
	eng->indBuffer = malloc(eng->indBufferSize);

	float *vb = (float *) eng->vertBuffer;
	unsigned short *ib = (unsigned short *) eng->indBuffer;
	int vbo, ibo;

	vbo = ibo = 0;

	short offs = 0;
	short ioffs = 0;

	float lineEntry[12];

	// fix zero_quad
	eng->zeroQuad = quad_create(0, 0, 0, 1, 1, 0);
	Quad *q = eng->zeroQuad;

	q->lineVBOoffset = (short) (ioffs);
	ioffs += 8;
	q->quadVBOoffset = (short) (ioffs);
	ioffs += 6;

	int l = 0;
	for (int v = 0; v < 4; v++)
	{
		lineEntry[l++] = q->tmpVerts[v]->x;
		lineEntry[l++] = q->tmpVerts[v]->y;
		lineEntry[l++] = q->tmpVerts[v]->z;
	}

	ib[ibo++] = offs + 0;
	ib[ibo++] = offs + 1;

	ib[ibo++] = offs + 1;
	ib[ibo++] = offs + 2;

	ib[ibo++] = offs + 2;
	ib[ibo++] = offs + 3;

	ib[ibo++] = offs + 3;
	ib[ibo++] = offs + 0;

	offs += 4;

	ib[ibo++] = offs + 0;
	ib[ibo++] = offs + 1;
	ib[ibo++] = offs + 2;

	ib[ibo++] = offs + 2;
	ib[ibo++] = offs + 3;
	ib[ibo++] = offs + 0;

	offs += 4;

	for (int v = 0; v < 12; v++)
		vb[vbo++] = lineEntry[v];

	for (int v = 0; v < 12; v++)
		vb[vbo++] = q->vertsForDrawing[v];

	// fix rest of quads
	for (int qq = 0; qq < eng->nQuads; qq++)
	{
		Quad *q = eng->quads[qq];

		q->lineVBOoffset = (short) (ioffs);
		ioffs += 8;
		q->quadVBOoffset = (short) (ioffs);
		ioffs += 6;

		l = 0;
		for (int v = 0; v < 4; v++)
		{
			lineEntry[l++] = q->verts[v]->x;
			lineEntry[l++] = q->verts[v]->y;
			lineEntry[l++] = q->verts[v]->z;
		}

		ib[ibo++] = offs + 0;
		ib[ibo++] = offs + 1;

		ib[ibo++] = offs + 1;
		ib[ibo++] = offs + 2;

		ib[ibo++] = offs + 2;
		ib[ibo++] = offs + 3;

		ib[ibo++] = offs + 3;
		ib[ibo++] = offs + 0;

		offs += 4;

		ib[ibo++] = offs + 0;
		ib[ibo++] = offs + 1;
		ib[ibo++] = offs + 2;

		ib[ibo++] = offs + 2;
		ib[ibo++] = offs + 3;
		ib[ibo++] = offs + 0;

		offs += 4;

		for (int v = 0; v < 12; v++)
			vb[vbo++] = lineEntry[v];

		for (int v = 0; v < 12; v++)
			vb[vbo++] = q->vertsForDrawing[v];
	}
}

int b2i(void *b)
{
	char buff[4];

	for (int f = 0; f < 4; f++)
	{
		buff[3 - f] = ((char *) b)[f];
	}

	int32_t i;
	memcpy(&i, buff, 4);
	return i;
}

// translate x, y, z to offset in eng->verts
int getVertAddr(PhlippleEngine *eng, int x, int y, int z)
{
	int i = x;
	i += y * eng->size;
	i += z * (eng->size * eng->size);

	return i;
}

Quad *add_quad(PhlippleEngine *eng, Quad *q)
{
	// make sure all verts exist
	int nAddedVerts = 0;
	Vertex *addedVerts[4];

	for (int v = 0; v < 4; v++)
	{
		int x = q->tmpVerts[v]->x;
		int y = q->tmpVerts[v]->y;
		int z = q->tmpVerts[v]->z;

		int addr = getVertAddr(eng, x, y, z);
		if (!eng->verts[addr])
		{
			eng->verts[addr] = vertex_create(x, y, z);
			//eng->verts[addr]->quads[eng->verts[addr]->nLinkedQuads++] = q;
		}

		addedVerts[nAddedVerts++] = eng->verts[addr];
	}

	quad_assignToEngine(q, addedVerts);
	for (int f = 0; f < nAddedVerts; f++)
		addedVerts[f]->quads[addedVerts[f]->nLinkedQuads++] = q;

	return q;
}

void engine_addLink(PhlippleEngine *eng, int x1, int y1, int z1, int x3,
                    int y3, int z3)
{
	Vertex *v1 = eng->verts[getVertAddr(eng, x1, y1, z1)];
	Vertex *v2 = eng->verts[getVertAddr(eng, x3, y3, z3)];

	if (!v1)
	{
		v1 = vertex_create(x1, y1, z1);
		eng->verts[getVertAddr(eng, x1, y1, z1)] = v1;
	}

	if (!v2)
	{
		v2 = vertex_create(x3, y3, z3);
		eng->verts[getVertAddr(eng, x3, y3, z3)] = v2;
	}

	vertex_link(v1, v2);

	for (int f = 0; f < v1->nLinkedQuads; f++)
	{
		if (v1->quads[f] != 0)
			quad_updateHardlinks(v1->quads[f]);
	}

	for (int f = 0; f < v2->nLinkedQuads; f++)
	{
		if (v2->quads[f] != 0)
			quad_updateHardlinks(v2->quads[f]);
	}
}

void rd(void *buff, int size, int count, char **ptr)
{
	memcpy(buff, *ptr, size * count);
	*ptr+=(size * count);
}

PhlippleEngine *phlipple_engine_create(char *resname)
{
	char *inph;
	char *tofree;

	size_t l = OS_getResource(resname, &inph);
	if (l == 0)
		return NULL;

	tofree = inph;

	PhlippleEngine* eng = malloc(sizeof(PhlippleEngine));

	// zero all allocables here...
	eng->verts = 0;
	eng->quads = 0;
	eng->quadsIsland = 0;
	eng->camera = 0;
	eng->zeroQuad = 0;
	eng->vertBuffer = 0;
	eng->indBuffer = 0;
	eng->buzzSound = 0;
	eng->flippedSound = 0;
	eng->flipanim = 0;

	// load the file
	int fmLen = strlen(FILE_MAGIC);
	char magic[fmLen * 2];
	char buff[256];

	for (int f = 0; f < fmLen; f++)
	{
		magic[f * 2] = 0;
		magic[(f * 2) + 1] = FILE_MAGIC[f];
	}

	rd(buff, fmLen, 2, &inph);

	if (memcmp(buff, magic, fmLen * 2) != 0)
	{
		phlipple_engine_destroy(eng);
		return NULL;
	}

	// magic is ok - let's read it
	rd(buff, sizeof(int32_t), 1, &inph);
	eng->size = b2i(buff);
	eng->nVerts = eng->size * eng->size * eng->size;

	rd(buff, sizeof(int32_t), 1, &inph);
	eng->nQuads = b2i(buff);

	eng->verts = malloc(sizeof(Vertex *) * eng->nVerts);
	for (int v = 0; v < eng->nVerts; v++)
		eng->verts[v] = 0;

	eng->quads = malloc(sizeof(Quad *) * eng->nQuads);
	eng->quadsIsland = malloc(sizeof(Quad *) * eng->nQuads);

	for (int q = 0; q < eng->nQuads; q++)
	{
		int x1, y1, z1, x3, y3, z3;
		rd(buff, sizeof(int32_t), 1, &inph);
		x1 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		y1 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		z1 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		x3 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		y3 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		z3 = b2i(buff);

		eng->quads[q] = add_quad(eng, quad_create(x1, y1, z1, x3, y3, z3));
	}

	rd(buff, sizeof(int32_t), 1, &inph);
	eng->nLinks = b2i(buff);

	// alloc links
	for (int l = 0; l < eng->nLinks; l++)
	{
		int x1, y1, z1, x3, y3, z3;
		rd(buff, sizeof(int32_t), 1, &inph);
		x1 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		y1 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		z1 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		x3 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		y3 = b2i(buff);
		rd(buff, sizeof(int32_t), 1, &inph);
		z3 = b2i(buff);

		engine_addLink(eng, x1, y1, z1, x3, y3, z3);
	}

	rd(buff, 1, 1, &inph);
	eng->inverted = eng->startInverted = *(char *) buff;
	rd(buff, sizeof(int32_t), 1, &inph);
	eng->startQuad = b2i(buff);

	eng->startRotation = 0;
	eng->startZoom = CAMERA_DEFAULT_DISTANCE;

	if (inph - tofree < l)
	{
		rd(buff, sizeof(int32_t), 1, &inph);
		eng->startRotation = b2i(buff);
	}

	if (inph - tofree < l)
	{
		rd(buff, 4, 1, &inph);
		char b[4];

		for (int f = 0; f < 4; f++)
		{
			b[3 - f] = ((char *) buff)[f];
		}

		float fl;
		memcpy(&fl, b, 4);
		eng->startZoom = fl;
	}

	free(tofree);

	buildVBOs(eng);
	phlipple_engine_restart(eng);
	return eng;
}

void phlipple_engine_destroy(PhlippleEngine *eng)
{
	if (eng->verts)
	{
		for (int v = 0; v < eng->nVerts; v++)
			vertex_destroy(eng->verts[v]);

		free(eng->verts);
	}

	if (eng->quads)
	{
		for (int q = 0; q < eng->nQuads; q++)
			quad_destroy(eng->quads[q]);

		free(eng->quads);
	}

	if (eng->quadsIsland)
		free(eng->quadsIsland);

	if (eng->vertBuffer)
		free(eng->vertBuffer);

	if (eng->indBuffer)
		free(eng->indBuffer);

	camera_destroy(eng->camera);

	if (eng->zeroQuad)
	{
		quad_destroy(eng->zeroQuad);
	}

	free(eng);
}

void phlipple_engine_restart(PhlippleEngine *eng)
{
	if (eng->camera)
		camera_destroy(eng->camera);

	eng->camera = camera_create(eng);

	for (int q = 0; q < eng->nQuads; q++)
	{
		eng->quads[q]->accessible = 1;
		eng->quads[q]->live = 1;
	}

	for (int q = 0; q < eng->nQuads; q++)
	{
		quad_updateHardlinks(eng->quads[q]);
	}

	eng->inverted = eng->startInverted;
	eng->activeQuad = eng->startQuad;
	eng->failed = 0;
	eng->finished = 0;
	eng->stuck = 0;

	camera_rebuildZOrderLists(eng->camera);
	camera_setRotation(eng->camera, eng->startRotation, 1);
	camera_setActiveQuad(eng->camera, eng->activeQuad, eng->inverted, 1);

}

void phlipple_engine_render(PhlippleEngine *eng, RenderState *rend)
{
	int nQuads = 0;
	for (int q = 0; q < eng->nQuads; q++)
	{
		if (eng->quads[q]->live)
			nQuads++;
	}

	rend->nQuads = nQuads;
	camera_render(eng->camera, rend);

	if (eng->flipanim)
	{
		double fl = eng->flipstart + (eng->flipcw ? -eng->flipr : eng->flipr);
		rend->flipRotateExtra = 0;

		if (eng->flipaxs == AXIS_X)
		{
			if (eng->flipv1->x < eng->flipv2->x)
			{
				rend->flipTranslate[0] = eng->flipv1->x;
				rend->flipTranslate[1] = eng->flipv1->y;
				rend->flipTranslate[2] = eng->flipv1->z;
			}
			else
			{
				rend->flipTranslate[0] = eng->flipv2->x;
				rend->flipTranslate[1] = eng->flipv2->y;
				rend->flipTranslate[2] = eng->flipv2->z;
			}

			rend->flipRotate[0] = ((float) (fl * (180.0 / PI)));
			rend->flipRotate[1] = 1;
			rend->flipRotate[2] = 0;
			rend->flipRotate[3] = 0;
		}

		if (eng->flipaxs == AXIS_Y)
		{
			if (eng->flipv1->y < eng->flipv2->y)
			{
				rend->flipTranslate[0] = eng->flipv1->x;
				rend->flipTranslate[1] = eng->flipv1->y;
				rend->flipTranslate[2] = eng->flipv1->z;
			}
			else
			{
				rend->flipTranslate[0] = eng->flipv2->x;
				rend->flipTranslate[1] = eng->flipv2->y;
				rend->flipTranslate[2] = eng->flipv2->z;
			}
			rend->flipRotate[0] = ((float) (fl * (180.0 / PI)));
			rend->flipRotate[1] = 0;
			rend->flipRotate[2] = -1;
			rend->flipRotate[3] = 0;
		}

		if (eng->flipaxs == AXIS_Z)
		{
			if (eng->flipv1->z < eng->flipv2->z)
			{
				rend->flipTranslate[0] = eng->flipv1->x;
				rend->flipTranslate[1] = eng->flipv1->y;
				rend->flipTranslate[2] = eng->flipv1->z;
			}
			else
			{
				rend->flipTranslate[0] = eng->flipv2->x;
				rend->flipTranslate[1] = eng->flipv2->y;
				rend->flipTranslate[2] = eng->flipv2->z;
			}

			// rend.rotate(90, 1, 0, 0);
			rend->flipRotateExtra = 1;

			rend->flipRotate[0] = ((float) (fl * (180.0 / PI)));
			rend->flipRotate[1] = 0;
			rend->flipRotate[2] = 1;
			rend->flipRotate[3] = 0;
		}
	}

	rend->flipAnim = eng->flipanim;
}

void traceQuads(PhlippleEngine *eng, Quad *curr)
{
	Quad *adh[12];
	int found = 0;

	if (!curr->live)
		return;

	for (int f = 0; f < eng->nQuads; f++)
	{
		if (eng->quadsIsland[f] == curr)
			found = 1;
	}

	if (!found)
	{
		eng->quadsIsland[eng->traceOffset++] = curr;
	}
	else
	{
		return;
	}

	for (int d = 0; d < 4; d++)
	{
		quad_getAdherentQuads(curr, d, adh);
		for (int dd = 0; dd < 12; dd++)
		{
			if (adh[dd] != NULL)
			{
				if (adh[dd]->live)
					traceQuads(eng, adh[dd]);
			}
		}
	}
}

void switchedQuad(PhlippleEngine *eng)
{
	quad_makeActive(eng->quads[eng->activeQuad]);

	// is the level finished?
	int liveQuads = 0;
	for (int f = 0; f < eng->nQuads; f++)
	{
		Quad *q = eng->quads[f];
		q->accessible = 1;

		if (q->live)
			liveQuads++;
	}

	if (liveQuads == 1)
	{
		eng->finished = 1;
		return;
	}

	// if level is not finished and we're stuck, we're stuck :)
	if (eng->quads[eng->activeQuad]->stuck)
	{
		eng->failed = 1;
		eng->stuck = 1;
	}

	// trace quads from activequad
	for (int q = 0; q < eng->nQuads; q++)
	{
		eng->quadsIsland[q] = NULL;
	}

	eng->traceOffset = 0;
	traceQuads(eng, eng->quads[eng->activeQuad]);

	if (eng->traceOffset != liveQuads)
	{
		eng->failed = 1;

		for (int qq = 0; qq < eng->nQuads; qq++)
		{
			Quad *q = eng->quads[qq];

			int accessible = 0;
			for (int f = 0; f < eng->traceOffset; f++)
			{
				if (eng->quadsIsland[f] == q)
				{
					accessible = 1;
					break;
				}
			}

			if (!accessible)
				q->accessible = 0;
		}
	}

}

void phlipple_engine_animate(PhlippleEngine *eng, double timeElapsed)
{
	camera_animate(eng->camera, timeElapsed);

	eng->flipr += timeElapsed * 15.0;

	if (eng->flipanim)
		if (eng->flipr > fabs(eng->flipend - eng->flipstart))
		{
			switchedQuad(eng);

			eng->flipanim = 0;
		}

}

void phlipple_engine_rol(PhlippleEngine *eng)
{
	camera_rol(eng->camera);
}

void phlipple_engine_ror(PhlippleEngine *eng)
{
	camera_ror(eng->camera);
}

int camLR2Qdir[6][4][4] = { { // ____up____
		{
			// rot 0
			2, // north
			3, // south
			0, // west
			1, // east
		}, { // rot 1
			0, // north
			1, // south
			3, // west
			2, // east
		}, { // rot 2
			3, // north
			2, // south
			1, // west
			0, // east
		}, { // rot 3
			1, // north
			0, // south
			2, // west
			3, // east
		}
	}, { // ____down____
		{
			// rot 0
			1, // north
			0, // south
			3, // west
			2, // east
		}, { // rot 1
			3, // north
			2, // south
			0, // west
			1, // east
		}, { // rot 2
			0, // north
			1, // south
			2, // west
			3, // east
		}, { // rot 3
			2, // north
			3, // south
			1, // west
			0, // east
		}
	}, { // ____north____
		{
			// rot 0
			0, // north
			1, // south
			2, // west
			3, // east
		}, { // rot 1
			2, // north
			3, // south
			1, // west
			0, // east
		}, { // rot 2
			1, // north
			0, // south
			3, // west
			2, // east
		}, { // rot 3
			3, // north
			2, // south
			0, // west
			1, // east
		}
	}, { // ____south____
		{
			// rot 0
			3, // north
			2, // south
			1, // west
			0, // east
		}, { // rot 1
			1, // north
			0, // south
			2, // west
			3, // east
		}, { // rot 2
			2, // north
			3, // south
			0, // west
			1, // east
		}, { // rot 3
			0, // north
			1, // south
			3, // west
			2, // east
		}
	}, { // ____west____
		{
			// rot 0
			3, // north
			2, // south
			1, // west
			0, // east
		}, { // rot 1
			1, // north
			0, // south
			2, // west
			3, // east
		}, { // rot 2
			2, // north
			3, // south
			0, // west
			1, // east
		}, { // rot 3
			0, // north
			1, // south
			3, // west
			2, // east
		}
	}, { // ____east____
		{
			// rot 0
			0, // north
			1, // south
			2, // west
			3, // east
		}, { // rot 1
			2, // north
			3, // south
			1, // west
			0, // east
		}, { // rot 2
			1, // north
			0, // south
			3, // west
			2, // east
		}, { // rot 3
			3, // north
			2, // south
			0, // west
			1, // east
		}
	}

};

// [plane][dir] = flipcw
int flipCW[3][4] = { { 1, 0, 1, 0 }, { 1, 0, 0, 1 }, { 0, 1, 0, 1 } };

// [plane][dir] = flipstart angle
double flipStart[3][4] = { { 1.5, .5, 1.5, .5 }, { 1, 0, 1.5, .5 }, {
		1, 0, 1,
		0
	}
};

void move(PhlippleEngine *eng, int buttonDirection, int reflip)
{
	Quad *adherentQuadsTemp[12];
	int v1test[3];
	int v2test[3];
	//int reflipQuad[6];

	if (!eng->camera->canMove)
		return;

	if (eng->flipanim)
		return;

	eng->buzzSound = 0;
	eng->flippedSound = 0;

	int
	dir =
	    camLR2Qdir[eng->camera->eyeLookupForMovement][eng->camera->rotationForMovement][buttonDirection];

	Quad *activeQuad = eng->quads[eng->activeQuad];

	int fcw = flipCW[activeQuad->plane][dir]; // flippin' CW/CCW
	if (eng->inverted)
		fcw = !fcw;

	Vertex *fv1 = NULL; // verts to flip around
	Vertex *fv2 = NULL;

	switch (dir)
	{
	case HARDLINK_NORTH:
		fv1 = activeQuad->verts[0];
		fv2 = activeQuad->verts[1];
		break;

	case HARDLINK_SOUTH:
		fv1 = activeQuad->verts[2];
		fv2 = activeQuad->verts[3];
		break;

	case HARDLINK_EAST:
		fv1 = activeQuad->verts[1];
		fv2 = activeQuad->verts[2];
		break;

	case HARDLINK_WEST:
		fv1 = activeQuad->verts[0];
		fv2 = activeQuad->verts[3];
		break;
	}

	int faxis = AXIS_X; // assume fv1.x != fv2.x

	if (fv1->y != fv2->y)
		faxis = AXIS_Y;

	if (fv1->z != fv2->z)
		faxis = AXIS_Z;

	double fs = flipStart[activeQuad->plane][dir] * PI;

	quad_getAdherentQuads(activeQuad, dir, adherentQuadsTemp);

	if (adherentQuadsTemp[0] == NULL && reflip)
	{
		return;
	}

	if (adherentQuadsTemp[0] == NULL && !reflip)
	{
		// no quads on this side, let's try a reflip on opposite side
		int newdir = HARDLINK_NORTH;

		switch (buttonDirection)
		{
		case HARDLINK_NORTH:
			newdir = HARDLINK_SOUTH;
			break;

		case HARDLINK_EAST:
			newdir = HARDLINK_WEST;
			break;

		case HARDLINK_WEST:
			newdir = HARDLINK_EAST;
			break;
		}

		dir
		= camLR2Qdir[eng->camera->eyeLookupForMovement][eng->camera->rotationForMovement][newdir];

		quad_getAdherentQuads(activeQuad, dir, adherentQuadsTemp);

		if (!quad_isMoveFree(activeQuad, dir))
		{
			eng->buzzSound = 1;
			return;
		}

		if (adherentQuadsTemp[0] != NULL)
		{
			eng->inverted = !eng->inverted;
			move(eng, newdir, 1);
		}

		return;
	}

	// allright! there are quads to move to, but can we make that move?

	if (!quad_isMoveFree(activeQuad, dir))
	{
		eng->buzzSound = 1;
		return;
	}

	// find current
	int hit = -1;
	int tries = 0;
	double fe = fs;
	int matches = 0;

	while (hit == -1 && tries < 3)
	{
		fe += fcw ? -(PI / 2.0) : PI / 2.0;

		if (faxis == AXIS_X)
		{
			v1test[0] = fv1->x;
			v1test[1] = (int) round((fv1->y + cos(fe)));
			v1test[2] = (int) round((fv1->z + sin(fe)));

			v2test[0] = fv2->x;
			v2test[1] = v1test[1];
			v2test[2] = v1test[2];

		}

		if (faxis == AXIS_Y)
		{
			v1test[0] = (int) round((fv1->x + cos(fe)));
			v1test[1] = fv1->y;
			v1test[2] = (int) round((fv1->z + sin(fe)));

			v2test[0] = v1test[0];
			v2test[1] = fv2->y;
			v2test[2] = v1test[2];

		}

		if (faxis == AXIS_Z)
		{
			v1test[0] = (int) round((fv1->x + cos(fe)));
			v1test[1] = (int) round((fv1->y + sin(fe)));
			v1test[2] = fv1->z;

			v2test[0] = v1test[0];
			v2test[1] = v1test[1];
			v2test[2] = fv2->z;
		}

		int h = 0;

		for (int f = 0; f < 12; f++)
		{
			Quad *q = adherentQuadsTemp[f];

			if (q != NULL && hit == -1)
			{

				matches = 0;

				for (int g = 0; g < 4; g++)
				{
					Vertex *mv = q->verts[g];

					if (vertex_isEqualInt(mv, v1test))
						matches++;
					if (vertex_isEqualInt(mv, v2test))
						matches++;
				}

				if (matches == 2)
					hit = h;

				h++;
			}

		}

		/*if (tries == 0) {
			reflipQuad[0] = fv1->x;
			reflipQuad[1] = fv1->y;
			reflipQuad[2] = fv1->z;
			reflipQuad[3] = v2test[0];
			reflipQuad[4] = v2test[1];
			reflipQuad[5] = v2test[2];
		}
		*/
		if (hit == -1)
			tries++;
	}

	if (hit == -1)
	{
		return;
	}

	// hit a quad
	Quad *hq = adherentQuadsTemp[hit];

	// depending on which side we hit it from, invert view
	int inv = 0;

	if (hq->plane == PLANE_X)
	{
		if (fv1->y == fv2->y)   // horizontal
		{
			if (fv1->y < v1test[1])   // normal
			{
				inv = !fcw;
			}
			else
			{
				// inverted
				inv = fcw;
			}
		}
		else
		{
			// vertical
			if (fv1->z < v1test[2])
			{
				inv = !fcw;
			}
			else
			{
				inv = fcw;
			}
		}
	}

	if (hq->plane == PLANE_Y)
	{

		if (fv1->x == fv2->x)   // horizontal
		{
			if (fv1->x < v1test[0])   // normal
			{
				inv = !fcw;
			}
			else
			{
				// inverted
				inv = fcw;
			}
		}
		else
		{
			// vertical
			if (fv1->z < v1test[2])
			{
				inv = fcw;
			}
			else
			{
				inv = !fcw;
			}
		}
	}

	if (hq->plane == PLANE_Z)
	{

		if (fv1->x == fv2->x)   // horizontal
		{
			if (fv1->x < v1test[0])   // normal
			{
				inv = fcw;
			}
			else
			{
				// inverted
				inv = !fcw;
			}
		}
		else
		{
			// vertical
			if (fv1->y < v1test[1])
			{
				inv = fcw;
			}
			else
			{
				inv = !fcw;
			}
		}
	}

	activeQuad->live = 0;

	eng->inverted = inv;
	int iq = -1;
	for (int f = 0; f < eng->nQuads; f++)
	{
		if (eng->quads[f] == hq)
			iq = f;
	}

	if (iq != -1)
	{
		eng->activeQuad = iq;
		camera_setActiveQuad(eng->camera, iq, eng->inverted, 0);
	}

	// fix visualisation
	eng->flipv1 = fv1;
	eng->flipv2 = fv2;
	eng->flipr = 0;
	eng->flipaxs = faxis;
	eng->flipcw = fcw;
	eng->flipstart = fs;
	eng->flipend = fe;
	eng->flipanim = 1;

	eng->flippedSound = 1;
}

void phlipple_engine_up(PhlippleEngine *eng)
{
	move(eng, HARDLINK_NORTH, 0);
}

void phlipple_engine_down(PhlippleEngine *eng)
{
	move(eng, HARDLINK_SOUTH, 0);
}
void phlipple_engine_left(PhlippleEngine *eng)
{
	move(eng, HARDLINK_WEST, 0);
}
void phlipple_engine_right(PhlippleEngine *eng)
{
	move(eng, HARDLINK_EAST, 0);
}
