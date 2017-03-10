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

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef PLATFORM_PSP
#include "GL/glew.h"
#endif

#include <GL/gl.h>

#include "quadrenderer.h"

int quad_renderer_initialized = 0;

GLuint qvbos[2];

float verts[20] =
{
	-.5f, .5f, 0,
	-.5f, -.5f, 0,
	.5f, -.5f, 0,
	.5f, .5f, 0,
	0, 0,
	0, 1.0,
	1.0f, 1.0f,
	1.0f, 0
};

short ind[6] =
{
	0, 1, 2,
	0, 2, 3
};


void quad_renderer_init()
{
	if (quad_renderer_initialized)
		return;

	glGenBuffers(2, qvbos);

	glBindBuffer(GL_ARRAY_BUFFER, qvbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts,
	             GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qvbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind),
	             ind, GL_STATIC_DRAW);

	quad_renderer_initialized = 1;

}

void quad_renderer_destroy()
{
	if (!quad_renderer_initialized)
		return;

	glDeleteBuffers(2, qvbos);
}

void quad_renderer_begin()
{
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, qvbos[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qvbos[1]);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glTexCoordPointer(2, GL_FLOAT, 0, (void *)(12 * sizeof(float)));

}

void render_quad()
{
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}
