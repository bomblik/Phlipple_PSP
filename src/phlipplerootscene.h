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

#ifndef PHLIPPLEROOTSCENE_H_
#define PHLIPPLEROOTSCENE_H_

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef PLATFORM_PSP
#include "GL/glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "gfxconstants.h"
#include "scenes.h"
#include "quadrenderer.h"
#include "texture.h"
#include "textrenderer.h"
#include "gradientrenderer.h"

extern GLuint texLogo;
extern GLuint texPhbx;
extern GLuint texButtonMenu;
extern GLuint texButtonAudioOn;
extern GLuint texButtonAudioOff;
extern GLuint texButtonNext;
extern GLuint texButtonMenu;
extern GLuint texButtonQuit;
extern GLuint texButtonReset;
extern GLuint texButtonPause;
extern GLuint texButtonStart;
extern GLuint texButtonLeftRight;
extern GLuint texTut1;
extern GLuint texTut2;
extern GLuint texTut3;
extern GLuint texSupp1;
extern GLuint texSupp2;

extern TextRenderer *txtRend;
extern GradientRenderer *grdRendMain;
extern GradientRenderer *grdRendGame;

#endif /* PHLIPPLEROOTSCENE_H_ */
