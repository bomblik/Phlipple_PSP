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
#include "scenes.h"
#include "osinterface.h"
#include "quadrenderer.h"

static SceneManager scnMgr;
static int destroyed;

int lastw = 0;
int lasth = 0;
int branched = 0;

SceneEvents * SceneManager_eventsInit()
{
	SceneEvents *evt = malloc(sizeof(SceneEvents));
	evt->size = 8;
	evt->nevt = 0;
	evt->evts = malloc(sizeof(SceneEvent) * evt->size);

	return evt;
}

void SceneManager_eventsDestroy(SceneEvents *evt)
{
	free(evt->evts);
	free(evt);
}

void SceneManager_eventsZero(SceneEvents *evt)
{
	evt->nevt = 0;
}

void SceneManager_eventsAdd(SceneEvents *evt, int type, int key, int x, int y)
{
	if (evt->nevt == evt->size)
	{
		evt->size*= 2;
		evt->evts = realloc(evt->evts, sizeof(SceneEvent) * evt->size);
	}

	evt->evts[evt->nevt].type = type;
	evt->evts[evt->nevt].keyVal = key;
	evt->evts[evt->nevt].x = x;
	evt->evts[evt->nevt].y = y;

	evt->nevt++;
}

void SceneManager_init(Scene *root, int *argc, char **argv)
{
	destroyed = 0;

	scnMgr.current = 0;
	scnMgr.size = 8;
	scnMgr.scenes = malloc(sizeof (Scene) * scnMgr.size);

	scnMgr.scenes[scnMgr.current] = *root;
	OS_init(argc, argv);
}

void SceneManager_branch(Scene *scene)
{
	scnMgr.current++;
	scnMgr.scenes[scnMgr.current] = *scene;
	branched = 1;
}

int SceneManager_run()
{
	//scnMgr.scenes[scnMgr.current].reshape(w, h);
	return OS_mainLoop();
}

void SceneManager_reshape(int w, int h)
{
	if(destroyed)
		return;

	lastw = w;
	lasth = h;

	// opengl should be initialized here - perfect place to
	// init quad_renderer
	quad_renderer_init();
	scnMgr.scenes[scnMgr.current].reshape(w, h);
}

void SceneManager_return()
{
	scnMgr.current--;
	if (scnMgr.current < 0)
	{
		quad_renderer_destroy();
		OS_quit();
		return;
	}

	scnMgr.scenes[scnMgr.current].reshape(lastw, lasth);
}

void SceneManager_pump(float time, SceneEvents *evt)
{
	if(destroyed)
		return;

	if (branched)
	{
		scnMgr.scenes[scnMgr.current].reshape(lastw, lasth);
		branched = 0;
	}

	scnMgr.scenes[scnMgr.current].logic(time, evt);

	if (destroyed)
		return;

	if (branched)
	{
		SceneManager_pump(time, evt);
		return;
	}

	scnMgr.scenes[scnMgr.current].render();
}

void SceneManager_destroy()
{
	if (destroyed)
		return;

	free(scnMgr.scenes);
	destroyed = 1;
}

void SceneManager_pause()
{

}

void SceneManager_resume()
{

}

