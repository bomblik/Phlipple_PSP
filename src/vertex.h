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

#ifndef VERTEX_H_
#define VERTEX_H_

#include "quad.h"

#define MAX_HARDLINKED_VERTS 	6
#define MAX_QUADS				12

struct _Vertex
{
	int x;
	int y;
	int z;
	struct _Vertex *hardLinked[MAX_HARDLINKED_VERTS];
	struct _Quad *quads[MAX_QUADS];
	int nLinkedQuads;
	int nHardLinked;
};

typedef struct _Vertex Vertex;

Vertex *vertex_create(int x, int y, int z);

void vertex_destroy(Vertex *);
void vertex_link(Vertex *, Vertex *);
int vertex_isLinked(Vertex *, Vertex *);
int vertex_isEqualVertex(Vertex *, Vertex *);
int vertex_isEqualInt(Vertex *, int []);


#endif /* VERTEX_H_ */
