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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "phlipplescenes.h"
#include "phlipplerootscene.h"
#include "gfxconstants.h"
#include "quadrenderer.h"
#include "interpolator.h"
#include "levels.h"
#include "engine.h"
#include "osinterface.h"

Scene mainScreen_scene = { mainScreen_logic, mainScreen_render,
                           mainScreen_reshape
                         };

void browserLogic(SceneEvents *evt);
void browserLoadLevel(int);
void browserRender();
void browserRestart();

void game_start(int l);

static int width;
static int height;
static float left;
static float right;
static float top;
static float bottom;

static int mainScreenFirstRun = 1;

static int mainScreenReady = 0;

static float buttSpread = 2;
static float buttSize = 1.5f;
static float buttY = -2.7f;

static Interpolator logoAlpha;
static Interpolator buttAlpha;
static Interpolator sndButtAlpha;
static Interpolator leftRightAlpha;
static Interpolator playButtAlpha;
static Interpolator txtAlpha;
static Interpolator iGameAlpha;
static Interpolator browserAlpha;
static Interpolator pageSwap;

static float gameAlpha;

static PhlippleEngine *eng = NULL;

static int vbosCreated = 0;
static GLuint vbos[2];

static float moveDelay = 1;
static float waitForMove = 1;
static int showBrowser = 0;
static int fadeOut = 0;

static int highlightSupp1 = 0;

// 0 none, 1 quit, 2 play, 3 browser
static int fadeAction = 0;

// browser
static int browserSelected = 0;
static int browserAllowPlay = 0;
static int browserLlevel;
static int browserNPages;
static int browserDispPage;
static int browserPage;
static int browserTouched;
static char browserLevTxt[256];
static float browserButtSize = 1.0;

void loadLevel(int l)
{
	char buff[256];
	if (eng != NULL)
		phlipple_engine_destroy(eng);

	if (vbosCreated)
	{
		glDeleteBuffers(2, vbos);
		vbosCreated = 0;
	}

	sprintf(buff, "%s", levels[l]->phname);
	eng = phlipple_engine_create(buff);
}

void mainScreen_logic(float timeElapsed, SceneEvents *evt)
{
	if (mainScreenFirstRun)
	{
		interpolator_initDefault(&logoAlpha);
		interpolator_initDefault(&buttAlpha);
		interpolator_initDefault(&sndButtAlpha);
		interpolator_initDefault(&playButtAlpha);
		interpolator_initDefault(&txtAlpha);
		interpolator_initDefault(&iGameAlpha);
		interpolator_initDefault(&browserAlpha);
		interpolator_initDefault(&leftRightAlpha);

		interpolator_init(&pageSwap, 0, 0.5, ENGINE_DEFAULT_FPS, 0.4);

		interpolator_setVal(&logoAlpha, 1);
		interpolator_setVal(&buttAlpha, 1);
		interpolator_setVal(&sndButtAlpha, 1);
		interpolator_setVal(&playButtAlpha, 1);
		interpolator_setVal(&txtAlpha, 1);
		interpolator_setValImmediate(&iGameAlpha, -2);
		interpolator_setVal(&iGameAlpha, .1);

		time_t t = time(NULL);
		srand((int) t);
		loadLevel(rand() % nLevels);

		mainScreenFirstRun = 0;
		OS_sound_On = atoi(OS_getPref("sound", "1"));

		OS_playMusic(OS_music_theme);
	}

	if (fadeAction == 2 && !fadeOut)
	{
		OS_playMusic(OS_music_theme);
		fadeAction = 0;
	}

	float te = timeElapsed / 40.0;
	interpolator_animate(&logoAlpha, te);
	interpolator_animate(&buttAlpha, te);
	interpolator_animate(&sndButtAlpha, te);
	interpolator_animate(&playButtAlpha, te);
	interpolator_animate(&txtAlpha, te);
	interpolator_animate(&iGameAlpha, te);
	interpolator_animate(&leftRightAlpha, te);
	interpolator_animate(&browserAlpha, te);
	interpolator_animate(&pageSwap, te);

	gameAlpha = interpolator_getVal(&iGameAlpha);

	waitForMove -= timeElapsed;
	if (waitForMove < 0)
	{
		waitForMove = moveDelay;

		if (showBrowser)
		{
			phlipple_engine_rol(eng);
		}

		if (eng->failed || eng->stuck || eng->finished)
		{
			loadLevel(rand() % nLevels);
		}

		if (!showBrowser)
		{
			switch (rand() % 6)
			{
			case 0:
				phlipple_engine_up(eng);
				break;
			case 1:
				phlipple_engine_down(eng);
				break;
			case 2:
				phlipple_engine_left(eng);
				break;
			case 3:
				phlipple_engine_right(eng);
				break;
			case 4:
				phlipple_engine_rol(eng);
				break;
			case 5:
				phlipple_engine_ror(eng);
				break;

			}
		}
	}

	// detect mouse over support buttonz
	highlightSupp1 = 0;

	float mx = (((float) OS_getMouseX() / (float) width) * (right - left))
	           + left;
	float my = (((float) OS_getMouseY() / (float) height) * (bottom - top))
	           + top;

	if (fabs(my - (bottom + buttSize / 4)) < buttSize / 4.0)
	{
		if (fabs(mx - (left + buttSize / 2.0)) < buttSize / 2.0)
			highlightSupp1 = 1;
	}

	int buttPressed = 0;
	for (int e = 0; e < evt->nevt; e++)
	{
		SceneEvent v = evt->evts[e];

		if (v.type == SCENEEVENT_TYPE_KEYDOWN)
		{
			switch (v.keyVal)
			{
			case OS_key_ok:
				buttPressed = 3;
				break;
			case OS_key_cancel:
				buttPressed = 1;
				break;
			}
		}

		if (v.type == SCENEEVENT_TYPE_FINGERDOWN)
		{
			float downX = (((float) v.x / (float) width) * (right - left))
			              + left;
			float downY = (((float) v.y / (float) height) * (bottom - top))
			              + top;


			if (highlightSupp1)
			{
#ifndef WIN32
				int i = system(LINUX_BROWSER_ANDROID);
				i++;
#endif
#ifdef WIN32
				ShellExecute(NULL, "open", WIN32_BROWSER_ANDROID, NULL, NULL, SW_SHOWNORMAL);
#endif
			}

			if (fabs(buttY - downY) < (buttSize / 2.0))
			{
				if (fabs(-buttSpread - downX) < (buttSize / 2.0))
				{
					buttPressed = 1;
					OS_playSound(OS_snd_click);
				}

				if (fabs(downX) < (buttSize / 2.0))
				{
					buttPressed = 2;
				}

				if (fabs(buttSpread - downX) < (buttSize / 2.0))
				{
					buttPressed = 3;
				}

			}
		}
	}

	if (showBrowser)
	{
		if (buttPressed == 1)
		{
			showBrowser = 0;
		}

		if (browserAllowPlay)
			if (buttPressed == 3)
			{
				showBrowser = 0;
				fadeAction = 2;
				fadeOut = 1;
				interpolator_setVal(&iGameAlpha, 0);
				interpolator_setVal(&browserAlpha, 0);
				interpolator_animate(&browserAlpha, 0);
				interpolator_setVal(&buttAlpha, 0);
				interpolator_setVal(&leftRightAlpha, 0);
				interpolator_setVal(&playButtAlpha, 0);
				OS_playSound(OS_snd_click);
				return;
			}

		interpolator_setVal(&iGameAlpha, .4);

		phlipple_engine_animate(eng, te);

		browserLogic(evt);

		if (!showBrowser)
		{
			interpolator_setVal(&logoAlpha, 1);
			interpolator_setVal(&sndButtAlpha, 1);
			interpolator_setVal(&txtAlpha, 1);
			interpolator_setVal(&playButtAlpha, 1);
			interpolator_setVal(&iGameAlpha, .1f);
			interpolator_setVal(&leftRightAlpha, 0);
			interpolator_setVal(&browserAlpha, 0);
			interpolator_animate(&browserAlpha, 0);

		}
		return;
	}

	if (buttPressed == 1)
	{
		fadeAction = 1;
		fadeOut = 1;
		interpolator_setVal(&logoAlpha, 0);
		interpolator_setVal(&buttAlpha, 0);
		interpolator_setVal(&playButtAlpha, 0);
		interpolator_setVal(&sndButtAlpha, 0);
		interpolator_setVal(&leftRightAlpha, 0);
		interpolator_setVal(&txtAlpha, 0);
		interpolator_setVal(&iGameAlpha, 0);
		return;
	}

	if (buttPressed == 2)
	{
		if (OS_sound_On)
		{
			OS_sound_On = 0;
			OS_setPref("sound", "0");
			OS_stopMusic();
		}
		else
		{
			OS_sound_On = 1;
			OS_setPref("sound", "1");
			OS_playMusic(OS_music_theme);
			OS_playSound(OS_snd_click);
		}
	}

	if (!fadeOut)
		if (buttPressed == 3)
		{
			fadeAction = 3;
			fadeOut = 1;

			interpolator_setVal(&logoAlpha, 0);
			interpolator_setVal(&txtAlpha, 0);
			interpolator_setVal(&iGameAlpha, 0);
			interpolator_setVal(&sndButtAlpha, 0);
			interpolator_setVal(&browserAlpha, .8);
			interpolator_setVal(&leftRightAlpha, .8);
			browserRestart();
			browserLoadLevel(browserSelected);

			showBrowser = 1;
			OS_playSound(OS_snd_click);
			return;
		}

	if (fadeOut)
	{
		if (buttAlpha.idle)
		{
			fadeOut = 0;
			if (fadeAction == 1)
			{
				OS_stopMusic();
				SceneManager_return();
				return;
			}

			if (fadeAction == 2)
			{
				OS_stopMusic();
				game_start(browserSelected);
				SceneManager_branch(&game_scene);
				interpolator_setVal(&logoAlpha, 1);
				interpolator_setVal(&buttAlpha, 1);
				interpolator_setVal(&sndButtAlpha, 1);
				interpolator_setVal(&playButtAlpha, 1);
				interpolator_setVal(&txtAlpha, 1);
				interpolator_setValImmediate(&iGameAlpha, -2);
				interpolator_setValImmediate(&iGameAlpha, .1);
				return;
			}
		}
	}

	phlipple_engine_animate(eng, timeElapsed / 2.0);
	mainScreenReady = 1;
}

void mainScreen_render()
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
	gradient_renderer_render(grdRendMain);

	if (!mainScreenReady)
		return;

	// render level
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

		glColor4f(lineCol[0], lineCol[1], lineCol[2], lineAlpha * gameAlpha);
		if (!qq->accessible)
		{
			glColor4f(unaaccessibleCol[0], unaaccessibleCol[1],
			          unaaccessibleCol[2], lineAlpha * gameAlpha);
		}

		glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT,
		               (void*) (long) (qq->lineVBOoffset * sizeof(unsigned short)));

		glLineWidth(4);
		glColor4f(hardLinkCol[0], hardLinkCol[1], hardLinkCol[2],
		          lineAlpha * gameAlpha);

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

		glColor4f(quadCol[0], quadCol[1], quadCol[2], quadAlpha * gameAlpha);
		if (!qq->accessible)
		{
			glColor4f(unaaccessibleCol[0], unaaccessibleCol[1],
			          unaaccessibleCol[2], quadAlpha * gameAlpha);
		}

		if (eng->quads[eng->activeQuad] == qq && !eng->flipanim)
			glColor4f(activeQuadCol[0], activeQuadCol[1], activeQuadCol[2],
			          activeQuadAlpha * gameAlpha);

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
			glColor4f(lineCol[0], lineCol[1], lineCol[2], lineAlpha * gameAlpha);
			glDrawElements(
			    GL_LINES,
			    8,
			    GL_UNSIGNED_SHORT,
			    (void*) (long) (eng->zeroQuad->lineVBOoffset
			                    * sizeof(unsigned short)));

			glColor4f(activeQuadCol[0], activeQuadCol[1], activeQuadCol[2],
			          activeQuadAlpha * gameAlpha);
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

	//render flat stuff over level
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(left, right, bottom, top);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	texture_apply(texLogo);

	glColor4f(1, 1, 1, interpolator_getVal(&logoAlpha));
	glTranslatef(0, 2, 0);
	glScalef(8, 2, 1);
	quad_renderer_begin();
	render_quad();
	glPopMatrix();

	glPushMatrix();
	text_renderer_begin(txtRend);

	glColor4f(1, 1, 1, interpolator_getVal(&txtAlpha));
	glTranslatef(0, bottom, 0);
	glScalef(.34f, .34f, 0);
	text_renderer_renderCentre(txtRend, COPYRIGHT);
	glPopMatrix();

	// render buttons
	quad_renderer_begin();
	texture_apply(texButtonQuit);
	glPushMatrix();
	glColor4f(1, 1, 1, interpolator_getVal(&buttAlpha));
	glTranslatef(-buttSpread, buttY, 0);
	glScalef(buttSize, buttSize, 0);
	render_quad();
	glPopMatrix();

	glPushMatrix();
	glColor4f(1, 1, 1, interpolator_getVal(&playButtAlpha));
	glTranslatef(buttSpread, buttY, 0);
	glScalef(buttSize, buttSize, 0);
	texture_apply(texButtonStart);
	render_quad();
	glPopMatrix();

	glPushMatrix();
	glColor4f(1, 1, 1, interpolator_getVal(&sndButtAlpha));
	glTranslatef(0, buttY, 0);
	glScalef(buttSize, buttSize, 0);
	if (OS_sound_On)
	{
		texture_apply(texButtonAudioOn);
	}
	else
	{
		texture_apply(texButtonAudioOff);
	}

	render_quad();
	glPopMatrix();


	float alpha = interpolator_getVal(&buttAlpha);
	alpha *= 0.5;
	if (highlightSupp1)
		alpha = 1.0;

	// render support buttons
	texture_apply(texSupp1);
	glPushMatrix();
	glColor4f(1, 1, 1, alpha);
	glTranslatef(left + buttSize / 2, bottom + buttSize / 4, 0);
	glScalef(buttSize, buttSize / 2.0, 0);
	render_quad();
	glPopMatrix();

	if (!browserAlpha.idle || showBrowser)
		browserRender();
}

void mainScreen_reshape(int w, int h)
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
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(left, right, bottom, top);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0, 0, 0, 1);
}

// browser stuff
struct xy
{
	float x;
	float y;
};

struct xy coords[4][5];

void zeroCoords()
{
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 5; x++)
		{
			coords[y][x].x = (x - 2) * 2;
			coords[y][x].y = (float) (2.7 - (y * 1.3));
		}
}

int coord2butt(float x, float y)
{
	int ret = -1;
	for (int yy = 0; yy < 4 && ret == -1; yy++)
	{
		if (fabs(coords[yy][0].y - y) < browserButtSize / 2.0)
			for (int xx = 0; xx < 5 && ret == -1; xx++)
			{
				if (fabs(coords[yy][xx].x - x) < browserButtSize / 2.0)
					ret = (browserPage * 20) + (yy * 5) + xx;
			}
	}

	return ret;
}

void browserLoadLevel(int l)
{
	loadLevel(l);
	sprintf(browserLevTxt, "%03d", l + 1);

	if (l <= browserLlevel + 1)
	{

		interpolator_setVal(&playButtAlpha, 1);
		browserAllowPlay = 1;
	}
	else
	{
		interpolator_setVal(&playButtAlpha, 0);
		browserAllowPlay = 0;
	}

}

void browserRestart()
{
	browserLlevel = atoi(OS_getPref("last_level", "-1"));

	//browserLlevel = nLevels - 2;
	browserNPages = nLevels / 20;
	if (nLevels % 20 > 0)
		browserNPages++;
	browserDispPage = browserLlevel / 20;
	browserPage = browserDispPage;

	browserSelected = browserLlevel + 1;
	browserTouched = browserSelected;

	zeroCoords();
}

void browserLogic(SceneEvents *evt)
{
	if (pageSwap.idle)
		browserDispPage = browserPage;

	//if (pageSwap.idle) {
	/*if (swiped_horizontal > 0)
		if (browserPage > 0) {
			browserPage--;

			interpolator_setValImmediate(&pageSwap, 0);
			interpolator_setVal(&pageSwap, 1);
		}

	if (swiped_horizontal < 0)
		if (browserPage < browserNPages - 1) {
			browserPage++;

			interpolator_setValImmediate(&pageSwap, 0);
			interpolator_setVal(&pageSwap, -1);
		}
	*/
	for (int e = 0; e < evt->nevt; e++)
	{
		SceneEvent v = evt->evts[e];

		float ex = (((float) v.x / (float) width) * (right - left)) + left;
		float ey = (((float) v.y / (float) height) * (bottom - top)) + top;

		if (v.type == SCENEEVENT_TYPE_FINGERDOWN)
		{
			int t = coord2butt(ex, ey);
			if ((t > -1) && (t < nLevels) && (t != browserSelected))
			{
				browserTouched = t;
				browserSelected = t;
				browserLoadLevel(t);
				OS_playSound(OS_snd_click2);
			}

			if ((fabs(ey) < (buttSize / 2.0)) && pageSwap.idle)
			{
				if (fabs(left - ex) < (buttSize / 2.0))
				{
					if (browserPage > 0)
					{
						browserPage--;

						interpolator_setValImmediate(&pageSwap, 0);
						interpolator_setVal(&pageSwap, 1);
						OS_playSound(OS_snd_phlip);
					}
				}

				if (fabs(right - ex) < (buttSize / 2.0))
				{
					if (browserPage < browserNPages - 1)
					{
						browserPage++;

						interpolator_setValImmediate(&pageSwap, 0);
						interpolator_setVal(&pageSwap, -1);
						OS_playSound(OS_snd_phlip);
					}
				}
			}
		}

	}
}

void browserRender()
{
	float ralph = (float) interpolator_getVal(&browserAlpha);

	glLoadIdentity();
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	text_renderer_begin(txtRend);

	glEnable(GL_TEXTURE_2D);

	glTranslatef(0, 3.4, 0);
	glColor4f(1, 1, 1, ralph);
	glScalef(.5f, .5f, 1);
	text_renderer_renderCentre(txtRend, SEL_LEV_TXT);

	glLoadIdentity();

	glTranslatef(0, -3.2, 0);
	glColor4f(1, 1, 1, ralph);
	text_renderer_renderCentre(txtRend, browserLevTxt);

	glLoadIdentity();

	quad_renderer_begin();

	glDisable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glLineWidth(2);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);

	glVertexPointer(3, GL_FLOAT, 0, 0);

	glPushMatrix();
	glColor4f(1, 1, 1, ralph);

	if (!pageSwap.idle)
		glTranslatef((float) interpolator_getVal(&pageSwap) * (right - left),
		             0, 0);

	int curr = (browserDispPage * 20);
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 5; x++)
		{
			if (curr < nLevels)
			{
				glPushMatrix();
				// gl.glTranslatef((float) (x - 2) / 1.3f, ((float)(2 - y) /
				// 1.3f), 0);
				glTranslatef(coords[y][x].x, coords[y][x].y, 0);
				glScalef(browserButtSize, browserButtSize, browserButtSize);
				glTranslatef(-.5f, -.5f, 0);

				if (curr == browserSelected)
				{
					glLineWidth(4);
				}
				else
				{
					glLineWidth(1);
				}

				glColor4f(lineCol[0], lineCol[1], lineCol[2], ralph);

				glDrawElements(
				    GL_LINES,
				    8,
				    GL_UNSIGNED_SHORT,
				    (void*) (long) (eng->zeroQuad->lineVBOoffset
				                    * sizeof(unsigned short)));

				glColor4f(1, 0, 0, ralph);

				if (curr <= browserLlevel + 1)
				{
					glColor4f(0, 1, 0, ralph);
				}
				else
				{
					glColor4f(1, 1, 0, ralph);
				}

				glDrawElements(
				    GL_TRIANGLES,
				    6,
				    GL_UNSIGNED_SHORT,
				    (void*) (long) (eng->zeroQuad->quadVBOoffset
				                    * sizeof(unsigned short)));

				glPopMatrix();

			}

			curr++;
		}

	glPopMatrix();

	if (!pageSwap.idle)
	{
		glPushMatrix();
		if (browserDispPage < browserPage)
		{
			glTranslatef(
			    ((float) interpolator_getVal(&pageSwap) * (right - left))
			    + (right - left), 0, 0);
		}
		else
		{
			glTranslatef(
			    ((float) interpolator_getVal(&pageSwap) * (right - left))
			    - (right - left), 0, 0);
		}

		curr = (browserPage * 20);
		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 5; x++)
			{
				if (curr < nLevels)
				{
					glPushMatrix();
					// gl.glTranslatef((float) (x - 2) / 1.3f, ((float)(2 - y) /
					// 1.3f), 0);
					glTranslatef(coords[y][x].x, coords[y][x].y, 0);
					glScalef(browserButtSize, browserButtSize, browserButtSize);
					glTranslatef(-.5f, -.5f, 0);

					if (curr == browserSelected)
					{
						glLineWidth(4);
					}
					else
					{
						glLineWidth(1);
					}

					glColor4f(lineCol[0], lineCol[1], lineCol[2], ralph);

					glDrawElements(
					    GL_LINES,
					    8,
					    GL_UNSIGNED_SHORT,
					    (void*) (long) (eng->zeroQuad->lineVBOoffset
					                    * sizeof(unsigned short)));

					glColor4f(1, 0, 0, ralph);

					if (curr <= browserLlevel + 1)
					{
						glColor4f(0, 1, 0, ralph);
					}
					else
					{
						glColor4f(1, 1, 0, ralph);
					}

					glDrawElements(
					    GL_TRIANGLES,
					    6,
					    GL_UNSIGNED_SHORT,
					    (void*) (long) (eng->zeroQuad->quadVBOoffset
					                    * sizeof(unsigned short)));

					glPopMatrix();

				}

				curr++;
			}

		glPopMatrix();
	}

	// render page indicator
	for (int p = 0; p < browserNPages; p++)
	{
		glPushMatrix();
		glTranslatef(0, -3.9f, 0);
		glScalef(.4f, .5f, 1);
		glTranslatef((-(float) browserNPages / 2.0) + p, 0, 0);

		glColor4f(.8, .8, .8, ralph);

		if (p == browserDispPage)
			glColor4f(1, 1, 1, ralph);

		glDrawElements(
		    GL_TRIANGLES,
		    6,
		    GL_UNSIGNED_SHORT,
		    (void*) (long) (eng->zeroQuad->quadVBOoffset
		                    * sizeof(unsigned short)));

		glPopMatrix();
	}

	glPushMatrix();
	glLineWidth(1);
	glTranslatef(0, -3.9, 0);
	glScalef(.4, .5, 1);
	glColor4f(1, 1, 1, ralph);
	if (pageSwap.idle)
	{
		glTranslatef((float) (((-browserNPages / 2.0) + browserDispPage)), 0, 0);
	}
	else
	{
		glTranslatef(
		    (float) (((-browserNPages / 2.0) + browserDispPage)
		             - interpolator_getVal(&pageSwap)), 0, 0);
	}

	glDrawElements(
	    GL_LINES,
	    8,
	    GL_UNSIGNED_SHORT,
	    (void*) (long) (eng->zeroQuad->lineVBOoffset
	                    * sizeof(unsigned short)));

	glPopMatrix();

	quad_renderer_begin();
	texture_apply(texButtonLeftRight);
	glPushMatrix();
	glColor4f(1, 1, 1, interpolator_getVal(&leftRightAlpha));
	glTranslatef(left, 0, 0);
	glScalef(buttSize, buttSize, 0);
	render_quad();
	glPopMatrix();

	glPushMatrix();
	glColor4f(1, 1, 1, interpolator_getVal(&leftRightAlpha));
	glTranslatef(right, 0, 0);
	glScalef(buttSize, buttSize, 0);
	render_quad();
	glPopMatrix();
}
