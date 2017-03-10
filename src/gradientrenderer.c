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
#include "gradientrenderer.h"

GradientRenderer *gradient_renderer_create(float *col1, float *col2)
{
	GradientRenderer *grd = malloc(sizeof(GradientRenderer));

	float verts[28] =
	{
		-.5f, .5f, 0,
		-.5f, -.5f, 0,
		.5f, -.5f, 0,
		.5f, .5f, 0,
		col2[0], col2[1], col2[2], col2[3],
		col1[0], col1[1], col1[2], col1[3],
		col1[0], col1[1], col1[2], col1[3],
		col2[0], col2[1], col2[2], col2[3]
	};

	short ind[6] =
	{
		0, 1, 2,
		0, 2, 3
	};

	glGenBuffers(2, grd->vbos);

	glBindBuffer(GL_ARRAY_BUFFER, grd->vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts,
	             GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grd->vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind),
	             ind, GL_STATIC_DRAW);

	return grd;
}

void gradient_renderer_destroy(GradientRenderer *grd)
{
	glDeleteBuffers(2, grd->vbos);
	free(grd);
}

void gradient_renderer_render(GradientRenderer *grd)
{
	glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, grd->vbos[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grd->vbos[1]);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glColorPointer(4, GL_FLOAT, 0, (void *)(12 * sizeof(float)));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}
