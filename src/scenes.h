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

#ifndef SCENES_H_
#define SCENES_H_

#define SCENEEVENT_TYPE_KEYDOWN		1
#define SCENEEVENT_TYPE_KEYUP		2
#define SCENEEVENT_TYPE_FINGERDOWN	3
#define SCENEEVENT_TYPE_FINGERUP	4

struct _SceneEvent
{
	int type;
	int keyVal;
	float x;
	float y;
};

typedef struct _SceneEvent SceneEvent;

struct _SceneEvents
{
	int nevt;
	int size;
	SceneEvent *evts;
};

typedef struct _SceneEvents SceneEvents;

struct _Scene
{
	void (* logic) (float time, SceneEvents *);
	void (* render)();
	void (* reshape)(int, int);
};

typedef struct _Scene Scene;

struct _SceneManager
{
	Scene *scenes;
	int size;
	int current;
};

typedef struct _SceneManager SceneManager;

void SceneManager_reshape(int w, int h);
void SceneManager_pump(float time, SceneEvents *);
void SceneManager_pause();
void SceneManager_resume();
void SceneManager_destroy();

void SceneManager_init(Scene *root, int *argc, char **argv);
void SceneManager_return();
void SceneManager_branch(Scene *);
int SceneManager_run();

SceneEvents *SceneManager_eventsInit();
void SceneManager_eventsZero(SceneEvents *evt);
void SceneManager_eventsAdd(SceneEvents *evt, int type, int key, int x, int y);
void SceneManager_eventsDestroy(SceneEvents *evt);

#endif /* SCENES_H_ */
