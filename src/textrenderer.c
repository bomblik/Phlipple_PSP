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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "textrenderer.h"
#include "texture.h"
#include "osinterface.h"

#define MAXLINE 1024

char *getLine(char **buff, char *end)
{
	static char line[MAXLINE];
	line[0] = 0;

	int l = 0;
	while (l < MAXLINE && (*buff)[l] != '\r' && (*buff)[l] != '\n')
		l++;

	if (l < MAXLINE)
	{
		memcpy(line, *buff, l);
	}

	line[l] = 0;

	*buff += l;
	while ((*buff < end) && ((*buff)[0] == '\r' || (*buff)[0] == '\n'))
		*buff = *buff + 1;

	return line;
}

int ints[256];

int getInts(char *line)
{
	int nints = 0;

	for (int c = 0; line[c] != 0; c++)
	{
		if (isdigit(line[c]))
		{
			ints[nints++] = atoi(line + c);
			while (isdigit(line[c]))
				c++;
		}
	}

	return nints;
}

TextRenderer *text_renderer_create(char *resTex, char *resDesc)
{
	TextRenderer *txt = malloc(sizeof(TextRenderer));
	// load texture
	txt->texture = texture_load(resTex);
	if (txt->texture == 0)
	{
		free(txt);
		return NULL;
	}

	// load descs
	char *buff;
	char *buffOrig;
	char *end;
	size_t l = OS_getResource(resDesc, &buff);

	if (l == 0)
	{
		texture_destroy(txt->texture);
		free(txt);
		return NULL;
	}

	glGenBuffers(2, txt->vbos);

	buffOrig = buff;
	end = buffOrig + l;
	char *line = getLine(&buff, end);
	line = getLine(&buff, end);

	getInts(line);
	float scaleh = ints[3];

	float lh = (float) ints[0] / scaleh;

	line = getLine(&buff, end);
	line = getLine(&buff, end);
	getInts(line);
	int nchars = ints[0];

	float *vbuff = malloc(nchars * 20 * sizeof(float));
	short *ibuff = malloc(nchars * 6 * sizeof(short));

	txt->texOffs = nchars * 12;

	for (int f = 0; f < 256; f++)
		txt->valid[f] = 0;

	for (int f = 0; f < nchars; f++)
	{
		line = getLine(&buff, end);
		getInts(line);

		int ch = ints[0];
		float x = (float) ints[1] / scaleh;
		float y = (float) ints[2] / scaleh;
		float w = (float) ints[3] / scaleh;
		float h = (float) ints[4] / scaleh;
		float xo = (float) ints[5] / scaleh;
		float yo = (float) ints[6] / scaleh;
		float xa = (float) ints[7] / scaleh;
		xa = xa / lh;

		float verts[12] = { xo / lh, (-yo + lh) / lh, 0, xo / lh, (-(yo + h)
		                    + lh) / lh, 0, (xo + w) / lh, (-(yo + h) + lh) / lh, 0,
		                    (xo + w) / lh, (-yo + lh) / lh, 0
		                  };

		/*		float verts[12] = {
		 -.5, -.5, 0,
		 -.5, .5, 0,
		 .5, .5, 0,
		 .5, -.5, 0
		 };
		 */

		float tex[8] = { x, y, x, y + h, x + w, y + h, x + w, y };
		//float tex[8] = { 0, 0, 0, 1, 1, 1, 1, 0 };

		txt->valid[ch] = 1;
		txt->xa[ch] = xa;

		for (int v = 0; v < 12; v++)
		{
			vbuff[(f * 12) + v] = verts[v];
		}

		for (int t = 0; t < 8; t++)
		{
			vbuff[(f * 8) + txt->texOffs + t] = tex[t];
		}

		int mIndexOffset = f * 4;

		short ind[6] = { (short) (0 + mIndexOffset),
		                 (short) (1 + mIndexOffset), (short) (2 + mIndexOffset),
		                 (short) (0 + mIndexOffset), (short) (2 + mIndexOffset),
		                 (short) (3 + mIndexOffset)
		               };

		for (int i = 0; i < 6; i++)
			ibuff[f * 6 + i] = ind[i];

		txt->ibOffsets[ch] = (void *) (f * 6 * sizeof(short));
	}

	glBindBuffer(GL_ARRAY_BUFFER, txt->vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, nchars * 20 * sizeof(float), vbuff,
	             GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, txt->vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nchars * 6 * sizeof(short), ibuff,
	             GL_STATIC_DRAW);

	free(vbuff);
	free(ibuff);
	free(buffOrig);
	return txt;
}

void text_renderer_destroy(TextRenderer *txt)
{
	texture_destroy(txt->texture);
	glDeleteBuffers(2, txt->vbos);
	free(txt);
}

void text_renderer_begin(TextRenderer *txt)
{
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, txt->vbos[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glTexCoordPointer(2, GL_FLOAT, 0, (void *) (txt->texOffs * sizeof(float)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, txt->vbos[1]);

	texture_apply(txt->texture);
}

void text_renderer_renderChar(TextRenderer *txt, char ch)
{
	int c = ch;
	if (txt->valid[c])
	{
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, txt->ibOffsets[c]);
		glTranslatef(txt->xa[c], 0, 0);
	}
}

void text_renderer_render(TextRenderer *txt, char *s)
{
	for (int f = 0; s[f] != 0; f++)
	{
		int c = s[f];
		if (txt->valid[c])
		{
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
			               txt->ibOffsets[c]);
			glTranslatef(txt->xa[c], 0, 0);
		}
	}
}

void text_renderer_renderCentre(TextRenderer *txt, char *s)
{
	float w = text_renderer_measure(txt, s);
	glTranslatef(-w/2.0, 0, 0);
	for (int f = 0; s[f] != 0; f++)
	{
		int c = s[f];
		if (txt->valid[c])
		{
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
			               txt->ibOffsets[c]);
			glTranslatef(txt->xa[c], 0, 0);
		}
	}
}

float text_renderer_measure(TextRenderer *txt, char *s)
{
	float w = 0;

	for (int f = 0; s[f] != 0; f++)
	{
		int c = s[f];
		if (txt->valid[c])
		{
			w += txt->xa[c];
		}
	}

	return w;
}

