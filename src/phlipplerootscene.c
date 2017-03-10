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

#include "phlipplescenes.h"
#include "phlipplerootscene.h"
#include "levels.h"

Scene phlipple_root_scene = { phlipple_root_logic, phlipple_root_render,
                              phlipple_root_reshape
                            };

GLuint texLogo;
GLuint texPhbx;
GLuint texButtonMenu;
GLuint texButtonAudioOn;
GLuint texButtonAudioOff;
GLuint texButtonNext;
GLuint texButtonMenu;
GLuint texButtonQuit;
GLuint texButtonReset;
GLuint texButtonPause;
GLuint texButtonStart;
GLuint texButtonLeftRight;
GLuint texTut1;
GLuint texTut2;
GLuint texTut3;
GLuint texSupp1;
GLuint texSupp2;

TextRenderer *txtRend;
GradientRenderer *grdRendMain;
GradientRenderer *grdRendGame;

int root_initialized = 0;
int state = 0;

void phlipple_root_free();

void phlipple_root_logic(float timeElapsed, SceneEvents *evt)
{
	if (state == 0 && root_initialized)
	{
#ifndef SHOW_SPLASH
		state = 2;
		SceneManager_branch(&mainScreen_scene);
#endif

#ifdef SHOW_SPLASH
		state = 1;
		SceneManager_branch(&splash_scene);
#endif
		return;
	}

	if (state == 1)
	{
		state = 2;
		SceneManager_branch(&mainScreen_scene);
		return;
	}

	if (state == 2)
	{
		phlipple_root_free();
		SceneManager_return();
		state = 666;
	}
}

void phlipple_root_render()
{

}

void phlipple_root_initialize()
{
	texLogo = texture_load("logo.png");
	//texPhbx = texture_load("phbx.png");
	texButtonMenu = texture_load("menu.png");
	texButtonAudioOn = texture_load("audioon.png");
	texButtonAudioOff = texture_load("audiooff.png");
	texButtonNext = texture_load("next.png");
	texButtonMenu = texture_load("menu.png");
	texButtonQuit = texture_load("quit.png");
	texButtonReset = texture_load("reset.png");
	texButtonPause = texture_load("pause.png");
	texButtonStart = texture_load("start.png");
	texButtonLeftRight = texture_load("leftright.png");
	texTut1 = texture_load("tut1.png");
	texTut2 = texture_load("tut2.png");
	texTut3 = texture_load("tut3.png");
	texSupp1 = texture_load("support_android.png");
	texSupp2 = texture_load("support_apple.png");

	txtRend = text_renderer_create("ubuntu_0.png", "ubuntu.fnt");
	grdRendMain = gradient_renderer_create(mainGradCol0, mainGradCol1);
	grdRendGame = gradient_renderer_create(gradCol0, gradCol1);

	levels_load();
}

void phlipple_root_free()
{
	texture_destroy(texLogo);
	texture_destroy(texPhbx);
	texture_destroy(texButtonMenu);
	texture_destroy(texButtonAudioOn);
	texture_destroy(texButtonAudioOff);
	texture_destroy(texButtonNext);
	texture_destroy(texButtonMenu);
	texture_destroy(texButtonQuit);
	texture_destroy(texButtonReset);
	texture_destroy(texButtonPause);
	texture_destroy(texButtonStart);
	texture_destroy(texButtonLeftRight);
	texture_destroy(texTut1);
	texture_destroy(texTut2);
	texture_destroy(texTut3);
	texture_destroy(texSupp1);
	texture_destroy(texSupp2);

	text_renderer_destroy(txtRend);
	gradient_renderer_destroy(grdRendMain);
	gradient_renderer_destroy(grdRendGame);
	levels_free();
}

void phlipple_root_reshape(int w, int h)
{
	if (!root_initialized)
	{
		phlipple_root_initialize();
		root_initialized = 1;
	}
}
