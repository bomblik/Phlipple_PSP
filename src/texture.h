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

#ifndef TEXTURE_H_
#define TEXTURE_H_

#ifdef _WIN32
#include <windows.h>
#include "glew.h"
#endif

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

GLuint texture_load(char *res);
void texture_apply(GLuint id);
void texture_destroy(GLuint id);


#endif /* TEXTURE_H_ */
