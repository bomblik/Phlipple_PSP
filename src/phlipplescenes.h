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

#ifndef PHLIPPLESCENES_H_
#define PHLIPPLESCENES_H_

#include "scenes.h"
#include "phlipplerootscene.h"

void game_logic(float timeElapsed, SceneEvents *evt);
void game_render();
void game_reshape(int w, int h);

extern Scene game_scene;

void phlipple_root_logic(float timeElapsed, SceneEvents *evt);
void phlipple_root_render();
void phlipple_root_reshape(int w, int h);

extern Scene phlipple_root_scene;

void splash_logic(float timeElapsed, SceneEvents *evt);
void splash_render();
void splash_reshape(int w, int h);

extern Scene splash_scene;

void mainScreen_logic(float timeElapsed, SceneEvents *evt);
void mainScreen_render();
void mainScreen_reshape(int w, int h);

extern Scene mainScreen_scene;


#endif /* PHLIPPLESCENES_H_ */
