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

#include "vertex.h"

int vertex_isLinked(Vertex *v1, Vertex *v2)
{
	if (v1->nHardLinked == 0)
		return 0;

	for (int f = 0; f < v1->nHardLinked; f++)
	{
		if (v1->hardLinked[f] == v2)
			return 1;
	}

	return 0;
}

void vertex_link(Vertex *v1, Vertex *v2)
{
	if (!vertex_isLinked(v1, v2))
		v1->hardLinked[v1->nHardLinked++] = v2;

	if (!vertex_isLinked(v2, v1))
		v2->hardLinked[v2->nHardLinked++] = v1;
}

Vertex *vertex_create(int x, int y, int z)
{
	Vertex *vert = malloc(sizeof(Vertex));
	vert->x = x;
	vert->y = y;
	vert->z = z;

	vert->nHardLinked = 0;
	vert->nLinkedQuads = 0;

	for (int f = 0; f < MAX_HARDLINKED_VERTS; f++)
	{
		vert->hardLinked[f] = NULL;
	}

	for (int f = 0; f < MAX_QUADS; f++)
	{
		vert->quads[f] = NULL;
	}


	return vert;
}

int vertex_isEqualVertex(Vertex *v1, Vertex *v2)
{
	if (v2->x != v1->x)
		return 0;

	if (v2->y != v1->y)
		return 0;

	if (v2->z != v1->z)
		return 0;

	return 1;
}

int vertex_isEqualInt(Vertex *v1, int *v2)
{
	if (v1->x != v2[0])
		return 0;

	if (v1->y != v2[1])
		return 0;

	if (v1->z != v2[2])
		return 0;

	return 1;
}

void vertex_destroy(Vertex *vert)
{
	if (vert == NULL)
		return;

	free(vert);
}
