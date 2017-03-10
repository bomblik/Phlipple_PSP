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

#include "texture.h"
#include "osinterface.h"
#include <math.h>
#include <malloc.h>

GLuint texture_load(char *res)
{
	SDL_Surface *surface;
	GLenum texture_format;
	GLint  nOfColors;

	surface = IMG_Load(OS_getFileName(res));

	if (surface)
	{
		nOfColors = surface->format->BytesPerPixel;
		if (nOfColors == 4)     // contains an alpha channel
		{
#ifdef  __amigaos4__
			texture_format = GL_RGBA;
#else
			if (surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGBA;
			else
				texture_format = GL_BGRA;
#endif
		}
		else if (nOfColors == 3)     // no alpha channel
		{
#ifdef  __amigaos4__
			texture_format = GL_RGB;
#else
			if (surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
#endif
		}
		else
		{
			fprintf(stderr, "warning: the image is not truecolor..  this will probably break\n");
			return 0;
		}

		GLuint t;
		glGenTextures(1, &t);

		glBindTexture(GL_TEXTURE_2D, t);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
		              texture_format, GL_UNSIGNED_BYTE, surface->pixels );
		return t;
	}

	fprintf(stderr, "Failed to load texture: %s\n", res);
	return 0;
}

void texture_apply(GLuint id)
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void texture_destroy(GLuint id)
{
	glDeleteTextures(1, &id);
}
