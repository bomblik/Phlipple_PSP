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

#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <string.h>

#include "osinterface.h"
#include "gfxconstants.h"
#include "engine.h"
#include "scenes.h"
#include "quadrenderer.h"
#include "texture.h"
#include "textrenderer.h"
#include "gradientrenderer.h"
#include "levels.h"
#include "phlipplescenes.h"

Scene game_scene = { game_logic, game_render, game_reshape };

static PhlippleEngine *eng;

static int initialized = 0;

static GLuint vbos[2];
static int vbosCreated = 0;

static int width;
static int height;
static float left;
static float right;
static float top;
static float bottom;

static int levelToLoad;
static char phname[256];

static Interpolator msgAlpha;
static Interpolator leftButtAlpha;
static Interpolator rightButtAlpha;
static Interpolator middleButtAlpha;
static Interpolator tutPicAlpha;
static Interpolator gameAlpha;
static Interpolator tutMsgAlpha;
static Interpolator pauseButtAlpha;
static Interpolator rotLeftAlpha;
static Interpolator rotRightAlpha;

static int tutPic = 0;

static char centreMsg[256];
static char *tutMsg;
static float *msgCol;

static int currLev = 0;

// states 0 - play, 1 - finished, 2 - failed, 3 - stuck, 4 - next level, 5 -
// menu, 6 - restart, 7 - tut, 8 - ???, 9 - pause
static int gameState = 0;

static int leftButt;
static int middleButt;
static int rightButt;

static char *wellDoneMsgs[7] = { "Well done!", "Level finished", "Not bad!",
                                 "There goes another...", "Perfect!", "Amazing!", "Marvellous!"
                               };

void game_start(int l)
{
	initialized = 0;
	levelToLoad = l;
}

void setCentreMsg(int fail)
{
	interpolator_setValImmediate(&msgAlpha, 0);
	interpolator_setVal(&msgAlpha, 2);
	if (fail)
	{
		msgCol = txtColFailed;
	}
	else
	{
		msgCol = txtColSuccess;
	}
}

void game_logic(float timeElapsed, SceneEvents *evt)
{
	if (!initialized)
	{
		sprintf(phname, "%s", levels[levelToLoad]->phname);

		eng = phlipple_engine_create(phname);
		if (!eng)
		{
			SceneManager_return();
			return;
		}

		currLev = levelToLoad;

		interpolator_initDefault(&msgAlpha);
		interpolator_initDefault(&leftButtAlpha);
		interpolator_initDefault(&rightButtAlpha);
		interpolator_initDefault(&middleButtAlpha);
		interpolator_initDefault(&tutPicAlpha);
		interpolator_initDefault(&gameAlpha);
		interpolator_initDefault(&tutMsgAlpha);
		interpolator_initDefault(&pauseButtAlpha);
		interpolator_initDefault(&rotLeftAlpha);
		interpolator_initDefault(&rotRightAlpha);

		interpolator_setValImmediate(&gameAlpha, 0);
		interpolator_setVal(&gameAlpha, 1);
		interpolator_setValImmediate(&rotLeftAlpha, 0);
		interpolator_setVal(&rotLeftAlpha, GFX_ROT_BUTT_IDLE_ALPHA);
		interpolator_setValImmediate(&rotRightAlpha, 0);
		interpolator_setVal(&rotRightAlpha, GFX_ROT_BUTT_IDLE_ALPHA);
		interpolator_setValImmediate(&pauseButtAlpha, 0);

		gameState = 0;

		if (levels[currLev]->tutPic > 0)
		{
			tutPic = levels[currLev]->tutPic;
			interpolator_setValImmediate(&gameAlpha, 0);
			interpolator_setVal(&rotRightAlpha, 0);
			interpolator_setVal(&rotLeftAlpha, 0);
			interpolator_setVal(&pauseButtAlpha, 0);
			interpolator_setVal(&rightButtAlpha, 1);
			interpolator_setValImmediate(&tutPicAlpha, 0);
			interpolator_setVal(&tutPicAlpha, 1);
			gameState = 7;
		}

		if (levels[currLev]->tutDesc != NULL)
		{
			tutMsg = levels[currLev]->tutDesc;
			interpolator_setValImmediate(&tutMsgAlpha, 0);
			interpolator_setVal(&tutMsgAlpha, 1);
			msgCol = txtColSuccess;
		}

		//if (gameState == 0) {
		sprintf(centreMsg, "Level %d", currLev + 1);
		setCentreMsg(0);
		//}

		leftButt = middleButt = rightButt = 0;

		initialized = 1;
	}

	float alphAnim = timeElapsed;

	// fix interpolators
	interpolator_animate(&rotLeftAlpha, alphAnim / 2.0);
	interpolator_animate(&rotRightAlpha, alphAnim / 2.0);
	interpolator_animate(&msgAlpha, alphAnim);
	interpolator_animate(&tutMsgAlpha, alphAnim);
	interpolator_animate(&tutPicAlpha, alphAnim);
	interpolator_animate(&gameAlpha, alphAnim);
	interpolator_animate(&pauseButtAlpha, alphAnim / 2.0);

	if (gameState == 0)
	{
		if (rotLeftAlpha.idle)
			interpolator_setVal(&rotLeftAlpha, GFX_ROT_BUTT_IDLE_ALPHA);

		if (rotRightAlpha.idle)
			interpolator_setVal(&rotRightAlpha, GFX_ROT_BUTT_IDLE_ALPHA);

		leftButt = middleButt = rightButt = 0;
	}

	if (pauseButtAlpha.idle)
	{
		if (gameState == 9)
		{
			interpolator_setVal(&pauseButtAlpha, 0);
		}
		else
		{
			if (gameState == 0)
				interpolator_setVal(&pauseButtAlpha, GFX_PAUSE_BUTT_ALPHA);
		}
	}

	if (gameState == 0 && msgAlpha.idle)
		interpolator_setVal(&msgAlpha, 0);

	for (int e = 0; e < evt->nevt; e++)
	{
		SceneEvent v = evt->evts[e];

		if (!(gameState == 2 || gameState == 3 || gameState == 7))
			if (v.type == SCENEEVENT_TYPE_KEYDOWN)
			{
				switch (v.keyVal)
				{
				case OS_key_up:
					phlipple_engine_up(eng);
					break;

				case OS_key_down:
					phlipple_engine_down(eng);
					break;

				case OS_key_left:
					phlipple_engine_left(eng);
					break;

				case OS_key_right:
					phlipple_engine_right(eng);
					break;

				case OS_key_rol:
					phlipple_engine_rol(eng);
					break;

				case OS_key_ror:
					phlipple_engine_ror(eng);
					break;

				case 27:
					SceneManager_return();
					return;
				}
			}
	}

	if (eng->buzzSound)
	{
		OS_playSound(OS_snd_buzz);
		eng->buzzSound = 0;
	}

	if (eng->flippedSound)
	{
		OS_playSound(OS_snd_phlip);
		eng->flippedSound = 0;
	}

	phlipple_engine_animate(eng, timeElapsed);

	// butt presses go here!

	int buttPressed = 0;

	for (int e = 0; e < evt->nevt; e++)
	{
		SceneEvent v = evt->evts[e];
		float downX = (((float) v.x / (float) width) * (right - left)) + left;
		float downY = (((float) v.y / (float) height) * (bottom - top)) + top;

		if (v.type == SCENEEVENT_TYPE_KEYDOWN)
		{
			switch (v.keyVal)
			{
			case OS_key_ok:
				buttPressed = 2;
				break;
			case OS_key_cancel:
				if (gameState == 0)
					buttPressed = 4;
				if (gameState > 0)
					buttPressed = 1;
				if (gameState == 9)
					buttPressed = 2;
				break;
			}
		}

		if (v.type == SCENEEVENT_TYPE_FINGERDOWN)
		{
			if (gameState == 0)
			{
				if (downX > (right - (GFX_ROT_BUTT_DISPLACEMENT) - 1))
				{
					if (downY > -(GFX_ROT_BUTT_SCALE / 2.0) && downY
					        < (GFX_ROT_BUTT_SCALE / 2.0))
					{

						interpolator_setVal(&rotRightAlpha,
						                    GFX_ROT_BUTT_PRESSED_ALPHA);
						phlipple_engine_rol(eng);
					}
				}

				if (downX < (left + (GFX_ROT_BUTT_DISPLACEMENT)) + 1)
				{
					if (downY > -(GFX_ROT_BUTT_SCALE / 2.0) && downY
					        < (GFX_ROT_BUTT_SCALE / 2.0))
					{
						interpolator_setVal(&rotLeftAlpha,
						                    GFX_ROT_BUTT_PRESSED_ALPHA);
						phlipple_engine_ror(eng);
					}
				}

				if (downX > right - GFX_PAUSE_BUTT_SIZE)
					if (downY < bottom + GFX_PAUSE_BUTT_SIZE)
					{
						buttPressed = 4;
					}
			}

			if (fabs(-GFX_BUTT_Y_OFFS - downY) < (GFX_BUTT_SIZE / 2.0))
			{
				if (fabs(GFX_BUTT_X_OFFS - downX) < (GFX_BUTT_SIZE / 2.0))
				{
					buttPressed = 2;
				}

				if (fabs(-GFX_BUTT_X_OFFS - downX) < (GFX_BUTT_SIZE / 2.0))
				{
					buttPressed = 1;
				}

				if (fabs(downX) < (GFX_BUTT_SIZE / 2.0))
				{
					buttPressed = 3;
				}
			}
		}
	}

	if (gameState == 0)
	{
		if (buttPressed == 4)
		{
			interpolator_setVal(&pauseButtAlpha,
			                    GFX_PAUSE_BUTT_PRESSED_ALPHA);
			gameState = 9;
			OS_playSound(OS_snd_click);
		}


		if (eng->finished)
		{
			gameState = 1;

			strcpy(centreMsg, wellDoneMsgs[rand() % 7]);
			setCentreMsg(0);

			interpolator_setValImmediate(&leftButtAlpha, 0);
			interpolator_setValImmediate(&rightButtAlpha, 0);
			interpolator_setVal(&pauseButtAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 1);
			interpolator_setVal(&rightButtAlpha, 1);
			interpolator_setVal(&rotLeftAlpha, 0);
			interpolator_setVal(&rotRightAlpha, 0);

			int ll = atoi(OS_getPref("last_level", "-1"));
			if (ll < currLev)
			{
				char buff[256];
				sprintf(buff, "%d", currLev);
				OS_setPref("last_level", buff);
			}

			OS_playMusic(OS_music_success);
			return;
		}

		if (eng->failed)
		{
			gameState = 2;
			sprintf(centreMsg, "Failed!");
			setCentreMsg(1);

			interpolator_setValImmediate(&leftButtAlpha, 0);
			interpolator_setValImmediate(&rightButtAlpha, 0);
			interpolator_setVal(&pauseButtAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 1);
			interpolator_setVal(&rightButtAlpha, 1);
			interpolator_setVal(&rotLeftAlpha, 0);
			interpolator_setVal(&rotRightAlpha, 0);

			OS_playMusic(OS_music_failure);
		}

		if (eng->stuck)
		{
			gameState = 2;
			sprintf(centreMsg, "Stuck!");
			setCentreMsg(1);

			interpolator_setValImmediate(&leftButtAlpha, 0);
			interpolator_setValImmediate(&rightButtAlpha, 0);
			interpolator_setVal(&pauseButtAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 1);
			interpolator_setVal(&rightButtAlpha, 1);
			interpolator_setVal(&rotLeftAlpha, 0);
			interpolator_setVal(&rotRightAlpha, 0);

			OS_playMusic(OS_music_failure);
		}

	}

	// pause menu
	if (gameState == 9)
	{
		interpolator_setVal(&rotLeftAlpha, 0);
		interpolator_setVal(&rotRightAlpha, 0);
		interpolator_setVal(&leftButtAlpha, 1);
		interpolator_setVal(&middleButtAlpha, 1);
		interpolator_setVal(&rightButtAlpha, 1);

		interpolator_setVal(&gameAlpha, .2);

		leftButt = 1;
		middleButt = 3;
		rightButt = 4;

		if (buttPressed == 3)
		{
			interpolator_setVal(&msgAlpha, 0);
			interpolator_setVal(&rightButtAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 0);
			interpolator_setVal(&middleButtAlpha, 0);
			interpolator_setVal(&gameAlpha, 1);

			phlipple_engine_restart(eng);
			gameState = 6;
			OS_playSound(OS_snd_click);
		}

		if (buttPressed == 2)
		{
			interpolator_setVal(&msgAlpha, 0);
			interpolator_setVal(&rightButtAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 0);
			interpolator_setVal(&middleButtAlpha, 0);
			interpolator_setVal(&gameAlpha, 1);

			gameState = 6;
			OS_playSound(OS_snd_click);
		}

		if (buttPressed == 1)
		{
			interpolator_setVal(&msgAlpha, 0);
			interpolator_setVal(&rightButtAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 0);
			interpolator_setVal(&middleButtAlpha, 0);
			interpolator_setVal(&gameAlpha, 0);
			interpolator_setVal(&rotRightAlpha, 0);
			interpolator_setVal(&rotLeftAlpha, 0);
			interpolator_setVal(&pauseButtAlpha, 0);

			gameState = 5;
			OS_playSound(OS_snd_click);
		}
	}

	if (gameState > 0)
	{
		interpolator_animate(&leftButtAlpha, alphAnim);
		interpolator_animate(&rightButtAlpha, alphAnim);
		interpolator_animate(&middleButtAlpha, alphAnim);
	}

	if (gameState == 8 || gameState == 7)
	{
		rightButt = 2;
		interpolator_animate(&rightButtAlpha, alphAnim);

		if (tutPicAlpha.idle && gameState == 8)
		{
			gameState = 0;
			tutPic = -1;
			tutMsg = NULL;
		}
	}

	if (gameState == 7)
	{
		if (buttPressed == 2)
		{
			interpolator_setVal(&rightButtAlpha, 0);
			interpolator_setVal(&gameAlpha, 1);
			interpolator_setVal(&rotLeftAlpha, GFX_ROT_BUTT_IDLE_ALPHA);
			interpolator_setVal(&rotRightAlpha, GFX_ROT_BUTT_IDLE_ALPHA);
			interpolator_setVal(&tutMsgAlpha, 0);
			interpolator_setVal(&tutPicAlpha, 0);
			interpolator_setValImmediate(&msgAlpha, 0);

			sprintf(centreMsg, "Level %d", currLev + 1);
			setCentreMsg(0);
			gameState = 8;
			OS_playSound(OS_snd_click);
		}
	}

	if (gameState == 2 || gameState == 3)
	{
		leftButt = 1;
		rightButt = 3;

		if (buttPressed == 2)
		{
			interpolator_setVal(&msgAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 0);
			interpolator_setVal(&middleButtAlpha, 0);
			interpolator_setVal(&rightButtAlpha, 0);

			interpolator_setVal(&rotLeftAlpha, GFX_ROT_BUTT_IDLE_ALPHA);
			interpolator_setVal(&rotRightAlpha, GFX_ROT_BUTT_IDLE_ALPHA);

			phlipple_engine_restart(eng);

			gameState = 6;
			OS_playSound(OS_snd_click);
			return;
		}

		if (buttPressed == 1)
		{
			interpolator_setVal(&msgAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 0);
			interpolator_setVal(&middleButtAlpha, 0);
			interpolator_setVal(&rightButtAlpha, 0);
			interpolator_setVal(&gameAlpha, 0);

			gameState = 5;
			OS_playSound(OS_snd_click);
			return;
		}
	}

	if (gameState == 1)
	{
		// level finished
		leftButt = 1;
		rightButt = 2;

		if (buttPressed == 2)
		{
			interpolator_setVal(&msgAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 0);
			interpolator_setVal(&rightButtAlpha, 0);
			interpolator_setVal(&middleButtAlpha, 0);
			interpolator_setVal(&rotLeftAlpha, 0);
			interpolator_setVal(&rotRightAlpha, 0);
			interpolator_setVal(&gameAlpha, 0);

			gameState = 4;
			OS_playSound(OS_snd_click);
			return;
		}

		if (buttPressed == 1)
		{
			interpolator_setVal(&msgAlpha, 0);
			interpolator_setVal(&leftButtAlpha, 0);
			interpolator_setVal(&rightButtAlpha, 0);
			interpolator_setVal(&middleButtAlpha, 0);
			interpolator_setVal(&rotLeftAlpha, 0);
			interpolator_setVal(&rotRightAlpha, 0);
			interpolator_setVal(&gameAlpha, 0);

			gameState = 5;
			OS_playSound(OS_snd_click);
			return;
		}
	}

	if (gameState == 4) // nextLevel
	{
		if (msgAlpha.idle)
		{
			currLev++;
			if (currLev >= nLevels)
			{
				interpolator_setVal(&gameAlpha, 0);
				gameState = 5;
				return;
			}

			sprintf(phname, "%s", levels[currLev]->phname);
			phlipple_engine_destroy(eng);
			eng = phlipple_engine_create(phname);
			glDeleteBuffers(2, vbos);
			vbosCreated = 0;

			interpolator_setVal(&gameAlpha, 1);
			gameState = 0;

			if (levels[currLev]->tutPic > 0)
			{
				tutPic = levels[currLev]->tutPic;
				interpolator_setValImmediate(&gameAlpha, 0);
				interpolator_setVal(&rotRightAlpha, 0);
				interpolator_setVal(&rotLeftAlpha, 0);
				interpolator_setVal(&pauseButtAlpha, 0);
				interpolator_setVal(&rightButtAlpha, 1);
				interpolator_setValImmediate(&tutPicAlpha, 0);
				interpolator_setVal(&tutPicAlpha, 1);
				gameState = 7;
			}

			if (levels[currLev]->tutDesc != NULL)
			{
				tutMsg = levels[currLev]->tutDesc;
				interpolator_setValImmediate(&tutMsgAlpha, 0);
				interpolator_setVal(&tutMsgAlpha, 1);
				msgCol = txtColSuccess;
			}

			//if (gameState == 0) {
			sprintf(centreMsg, "Level %d", currLev + 1);
			setCentreMsg(0);
			//}

			return;
		}
	}

	if (gameState == 5)
	{
		if (gameAlpha.idle && leftButtAlpha.idle)
		{
			phlipple_engine_destroy(eng);
			glDeleteBuffers(2, vbos);
			vbosCreated = 0;
			SceneManager_return();
			return;
		}
	}

	if (gameState == 6)
	{
		if (msgAlpha.idle && leftButtAlpha.idle)
		{
			gameState = 0;
		}
	}
}

void game_render()
{
	if (!vbosCreated)
	{
		glGenBuffers(2, vbos);

		glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
		glBufferData(GL_ARRAY_BUFFER, eng->vertBufferSize, eng->vertBuffer,
		             GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, eng->indBufferSize,
		             eng->indBuffer, GL_STATIC_DRAW);

		vbosCreated = 1;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, 1, -1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);
	glScalef(right - left, top - bottom, 1);
	gradient_renderer_render(grdRendGame);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, (float) width / (float) height, 1, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	RenderState rend;
	phlipple_engine_render(eng, &rend);

	gluLookAt(rend.eye[0], rend.eye[1], rend.eye[2], rend.centre[0],
	          rend.centre[1], rend.centre[2], rend.up[0], rend.up[1], rend.up[2]);

	glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glLineWidth(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);

	glVertexPointer(3, GL_FLOAT, 0, 0);

	float lineAlpha = lineAlphaFar;
	float lineAlphaDelta = (lineAlphaNear - lineAlphaFar) / (float) rend.nQuads;

	float quadAlpha = quadAlphaFar;
	float quadAlphaDelta = (quadAlphaNear - quadAlphaFar) / (float) rend.nQuads;

	for (int q = 0; q < eng->nQuads; q++)
	{
		Quad *qq = rend.zOrderList[q];

		if (!qq->live)
			continue;

		glColor4f(lineCol[0], lineCol[1], lineCol[2],
		          lineAlpha * gameAlpha.curr);
		if (!qq->accessible)
		{
			glColor4f(unaaccessibleCol[0], unaaccessibleCol[1],
			          unaaccessibleCol[2], lineAlpha * gameAlpha.curr);
		}

		glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT,
		               (void*) (long) (qq->lineVBOoffset * sizeof(unsigned short)));

		glLineWidth(4);
		glColor4f(hardLinkCol[0], hardLinkCol[1], hardLinkCol[2],
		          lineAlpha * gameAlpha.curr);

		if (qq->hardLinks[0])
			glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT,
			               (void*) (long) (qq->lineVBOoffset * sizeof(unsigned short)));

		if (qq->hardLinks[1])
			glDrawElements(
			    GL_LINES,
			    2,
			    GL_UNSIGNED_SHORT,
			    (void*) (long) ((qq->lineVBOoffset + 4)
			                    * sizeof(unsigned short)));

		if (qq->hardLinks[2])
			glDrawElements(
			    GL_LINES,
			    2,
			    GL_UNSIGNED_SHORT,
			    (void*) (long) ((qq->lineVBOoffset + 6)
			                    * sizeof(unsigned short)));

		if (qq->hardLinks[3])
			glDrawElements(
			    GL_LINES,
			    2,
			    GL_UNSIGNED_SHORT,
			    (void*) (long) ((qq->lineVBOoffset + 2)
			                    * sizeof(unsigned short)));

		glLineWidth(2);

		glColor4f(quadCol[0], quadCol[1], quadCol[2],
		          quadAlpha * gameAlpha.curr);
		if (!qq->accessible)
		{
			glColor4f(unaaccessibleCol[0], unaaccessibleCol[1],
			          unaaccessibleCol[2], quadAlpha * gameAlpha.curr);
		}

		if (eng->quads[eng->activeQuad] == qq && !eng->flipanim)
			glColor4f(activeQuadCol[0], activeQuadCol[1], activeQuadCol[2],
			          activeQuadAlpha * gameAlpha.curr);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,
		               (void*) (long) (qq->quadVBOoffset * sizeof(unsigned short)));

		if (eng->quads[eng->activeQuad] == qq && eng->flipanim)
		{
			glPushMatrix();
			glTranslatef(rend.flipTranslate[0], rend.flipTranslate[1],
			             rend.flipTranslate[2]);

			if (rend.flipRotateExtra)
				glRotatef(90, 1, 0, 0);

			glRotatef(rend.flipRotate[0], rend.flipRotate[1],
			          rend.flipRotate[2], rend.flipRotate[3]);

			// render zeroquad
			glColor4f(lineCol[0], lineCol[1], lineCol[2],
			          lineAlpha * gameAlpha.curr);
			glDrawElements(
			    GL_LINES,
			    8,
			    GL_UNSIGNED_SHORT,
			    (void*) (long) (eng->zeroQuad->lineVBOoffset
			                    * sizeof(unsigned short)));

			glColor4f(activeQuadCol[0], activeQuadCol[1], activeQuadCol[2],
			          activeQuadAlpha * gameAlpha.curr);
			glDrawElements(
			    GL_TRIANGLES,
			    6,
			    GL_UNSIGNED_SHORT,
			    (void*) (long) (eng->zeroQuad->quadVBOoffset
			                    * sizeof(unsigned short)));

			glPopMatrix();
		}

		lineAlpha += lineAlphaDelta;
		quadAlpha += quadAlphaDelta;
	}

	// flat stuff
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, 1, -1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	quad_renderer_begin();

	// render pauseButt
	texture_apply(texButtonPause);

	glPushMatrix();
	glTranslatef(right - (GFX_PAUSE_BUTT_SIZE / 2.0),
	             bottom + (GFX_PAUSE_BUTT_SIZE / 2.0), 0);
	glScalef(GFX_PAUSE_BUTT_SIZE, GFX_PAUSE_BUTT_SIZE, GFX_PAUSE_BUTT_SIZE);
	glColor4f(1, 1, 1, pauseButtAlpha.curr);
	render_quad();
	glPopMatrix();

	// render tut
	if (tutPic > 0)
	{
		GLuint t = 0;
		switch (tutPic)
		{
		case 1:
			t = texTut1;
			break;
		case 2:
			t = texTut2;
			break;
		case 3:
			t = texTut3;
			break;
		}

		texture_apply(t);
		glPushMatrix();
		glTranslatef(0, 1, 0);
		glScalef(4, 4, 1);
		glColor4f(1, 1, 1, tutPicAlpha.curr);
		render_quad();
		glPopMatrix();
	}

	// render tut message
	if (tutMsg != NULL)
	{
		text_renderer_begin(txtRend);
		glPushMatrix();
		glTranslatef(GFX_TXT_SHADOW_OFFSET, -3.0 - GFX_TXT_SHADOW_OFFSET, 0);
		glScalef(0.5, 0.5, 1);
		glColor4f(0, 0, 0, tutMsgAlpha.curr);
		text_renderer_renderCentre(txtRend, tutMsg);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0, -3.0, 0);
		glScalef(0.5, 0.5, 1);
		glColor4f(msgCol[0], msgCol[1], msgCol[2], tutMsgAlpha.curr);
		text_renderer_renderCentre(txtRend, tutMsg);
		glPopMatrix();
	}

	if (gameState != 7)
		if (centreMsg[0] != 0)
		{
			float alph = msgAlpha.curr;
			if (alph > 1)
				alph = 1;

			text_renderer_begin(txtRend);
			glPushMatrix();
			glTranslatef(GFX_TXT_SHADOW_OFFSET, 0 - GFX_TXT_SHADOW_OFFSET, 0);
			glColor4f(0, 0, 0, alph);
			text_renderer_renderCentre(txtRend, centreMsg);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0, 0, 0);
			glColor4f(msgCol[0], msgCol[1], msgCol[2], alph);
			text_renderer_renderCentre(txtRend, centreMsg);
			glPopMatrix();
		}

	quad_renderer_begin();
	// render buttons
	if (leftButt > 0)
	{
		GLuint t = 0;

		switch (leftButt)
		{
		case 1:
			t = texButtonMenu;
			break;
		case 2:
			t = texButtonNext;
			break;
		case 3:
			t = texButtonReset;
			break;
		case 4:
			t = texButtonStart;
			break;
		}

		texture_apply(t);

		glPushMatrix();
		glTranslatef(-GFX_BUTT_X_OFFS, -GFX_BUTT_Y_OFFS, 0);
		glScalef(GFX_BUTT_SIZE, GFX_BUTT_SIZE, 1);
		glColor4f(1, 1, 1, leftButtAlpha.curr);
		render_quad();
		glPopMatrix();
	}

	if (rightButt > 0)
	{
		GLuint t = 0;

		switch (rightButt)
		{
		case 1:
			t = texButtonMenu;
			break;
		case 2:
			t = texButtonNext;
			break;
		case 3:
			t = texButtonReset;
			break;
		case 4:
			t = texButtonStart;
			break;
		}

		texture_apply(t);

		glPushMatrix();
		glTranslatef(GFX_BUTT_X_OFFS, -GFX_BUTT_Y_OFFS, 0);
		glScalef(GFX_BUTT_SIZE, GFX_BUTT_SIZE, 1);
		glColor4f(1, 1, 1, rightButtAlpha.curr);
		render_quad();
		glPopMatrix();
	}

	if (middleButt > 0)
	{
		GLuint t = 0;

		switch (middleButt)
		{
		case 1:
			t = texButtonMenu;
			break;
		case 2:
			t = texButtonNext;
			break;
		case 3:
			t = texButtonReset;
			break;
		case 4:
			t = texButtonStart;
			break;
		}

		texture_apply(t);

		glPushMatrix();
		glTranslatef(0, -GFX_BUTT_Y_OFFS, 0);
		glScalef(GFX_BUTT_SIZE, GFX_BUTT_SIZE, 1);
		glColor4f(1, 1, 1, middleButtAlpha.curr);
		render_quad();
		glPopMatrix();
	}
}

void game_reshape(int w, int h)
{
	// fit screen into anything the user threw at us
	float screenRatio = (float) w / (float) h;
	float desiredRatio = (float) screenRatioX / (float) screenRatioY;

	float fH = screenGLHeight;
	float fW = fH * desiredRatio;

	fW = fH * screenRatio;

	left = -fW / 2;
	right = fW / 2;
	top = fH / 2;
	bottom = -fH / 2;
	width = w;
	height = h;

	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glLineWidth(2);
	glViewport(0, 0, width, height);
	glClearColor(0, 0, 1, 0);
}

