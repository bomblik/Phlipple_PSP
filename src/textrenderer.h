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

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef PLATFORM_PSP
#include "GL/glew.h"
#endif

#include <GL/gl.h>

struct _TextRenderer
{
	GLuint texture;
	GLuint vbos[2];
	void *ibOffsets[256];
	float xa[256];
	int texOffs;
	unsigned char valid[256];
};

typedef struct _TextRenderer TextRenderer;

TextRenderer *text_renderer_create(char *resTex, char *resDesc);
void text_renderer_destroy(TextRenderer *);
void text_renderer_begin(TextRenderer *);
void text_renderer_renderChar(TextRenderer *, char);
void text_renderer_render(TextRenderer *, char *);
void text_renderer_renderCentre(TextRenderer *, char *);
float text_renderer_measure(TextRenderer *, char *);


#endif /* TEXTRENDERER_H_ */
