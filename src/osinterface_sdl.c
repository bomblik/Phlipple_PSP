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

#ifdef WIN32
#include <windows.h>
#endif

#ifndef WIN32
#include <limits.h>
#define MAX_PATH PATH_MAX
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "osinterface.h"
#include "GL/glew.h"
#include "engine.h"
#include "settings.h"

void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void mouse(int butt, int state, int x, int y);

void display(void);
void reshape(int width, int height);
void gameLoop();

static int recordVideo = 0;
static GLbyte *buff = 0;
FILE *videoFile = 0;
static int width;
static int height;

double ltime;
double timenow;
double frameTime;

static int hasQuit = 0;
static int audioInitialized = 0;

static Mix_Music *music_theme = NULL;
static Mix_Music *music_success = NULL;
static Mix_Music *music_failure = NULL;

static Mix_Chunk *snd_phlip = NULL;
static Mix_Chunk *snd_click = NULL;
static Mix_Chunk *snd_click2 = NULL;
static Mix_Chunk *snd_buzz = NULL;

SDL_Surface *screen;
SceneEvents *evts;
const SDL_VideoInfo *vi;

int OS_sound_On = 0;
int mouseX;
int mouseY;

double getNanoTime()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return (double) tv.tv_sec + ((double) tv.tv_usec / 1000000.0L);
}

void OS_init(int *argc, char **argv)
{
	settings_load();
	frameTime = 1.0 / ((double) ENGINE_DEFAULT_FPS);

	evts = SceneManager_eventsInit();
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		fprintf(stderr, "Could not initialize SDL:%s\n", SDL_GetError());
		exit(-1);
	}

	if (Mix_OpenAudio(44100, AUDIO_S16, 2, 4096) < 0)
	{
		fprintf(stderr, "No audio: %s\n", SDL_GetError());
	}
	else
	{
		audioInitialized = 1;

		music_theme = Mix_LoadMUS(OS_getFileName("theme.ogg"));
		music_success = Mix_LoadMUS(OS_getFileName("success.ogg"));
		music_failure = Mix_LoadMUS(OS_getFileName("failure.ogg"));

		snd_phlip = Mix_LoadWAV(OS_getFileName("phlip.ogg"));
		snd_click = Mix_LoadWAV(OS_getFileName("click.ogg"));
		snd_click2 = Mix_LoadWAV(OS_getFileName("click2.ogg"));
		snd_buzz = Mix_LoadWAV(OS_getFileName("glitch.ogg"));

		Mix_VolumeMusic(MIX_MAX_VOLUME);
	}

	vi = SDL_GetVideoInfo();
	int bpp = vi->vfmt->BitsPerPixel;

	switch (bpp)
	{
	case 16:
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 4 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 4 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 4 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		break;

	case 24:
	case 32:
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		break;

	default:
		fprintf(stderr, "This program requires at least 16bit display");
		exit(-1);
	}

	screen = SDL_SetVideoMode(640, 480, bpp, SDL_OPENGL | SDL_RESIZABLE);
	width = screen->w;
	height = screen->h;

	char wmcapt[256];
	sprintf(wmcapt, "Phlipple ver %s", PACKAGE_VERSION);

	SDL_WM_SetCaption(wmcapt, wmcapt);
}

int OS_mainLoop()
{
	GLenum err = glewInit();
	if (err == GLEW_OK)
	{
		SceneManager_reshape(screen->w, screen->h);
		while(!hasQuit)
			gameLoop();
	}

	return EXIT_SUCCESS;
}

void OS_quit()
{
	SceneManager_destroy();
	SceneManager_eventsDestroy(evts);
	hasQuit = 1;

	if (audioInitialized)
	{
		if (Mix_PlayingMusic())
			Mix_FadeOutMusic(200);

		if (music_theme)
			Mix_FreeMusic(music_theme);

		if (music_success)
			Mix_FreeMusic(music_success);

		if (music_failure)
			Mix_FreeMusic(music_failure);

		Mix_CloseAudio();
	}

	SDL_Quit();
	settings_save();
}

void keyboard(unsigned char key, int x, int y)
{
	SceneManager_eventsAdd(evts, SCENEEVENT_TYPE_KEYDOWN, key, x, y);
}

void reshape(int _width, int _height)
{
	width = _width;
	height = _height;
	screen = SDL_SetVideoMode(width, height, vi->vfmt->BitsPerPixel, SDL_OPENGL | SDL_RESIZABLE);
	SceneManager_reshape(width, height);
}


void gameLoop()
{
	SDL_Event event;

	/* Grab all the events off the queue. */
	while( SDL_PollEvent( &event ) )
	{
		int k = -1;

		switch( event.type )
		{
		case SDL_MOUSEMOTION:
			mouseX = event.motion.x;
			mouseY = event.motion.y;
			break;

		case SDL_MOUSEBUTTONDOWN:
			SceneManager_eventsAdd(evts, SCENEEVENT_TYPE_FINGERDOWN, 0, event.button.x, event.button.y);
			break;

		case SDL_MOUSEBUTTONUP:
			SceneManager_eventsAdd(evts, SCENEEVENT_TYPE_FINGERUP, 0, event.button.x, event.button.y);
			break;

		case SDL_VIDEORESIZE:
			reshape(event.resize.w, event.resize.h);
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
				k = OS_key_up;
				break;
			case SDLK_DOWN:
				k = OS_key_down;
				break;
			case SDLK_LEFT:
				k = OS_key_left;
				break;
			case SDLK_RIGHT:
				k = OS_key_right;
				break;
			case SDLK_a:
				k = OS_key_rol;
				break;
			case SDLK_d:
				k = OS_key_ror;
				break;
			case SDLK_RETURN:
			case SDLK_KP_ENTER:
				k = OS_key_ok;
				break;
			case SDLK_ESCAPE:
				k = OS_key_cancel;
				break;
			default:
				break;
			}

			if (k != -1)
				SceneManager_eventsAdd(evts, SCENEEVENT_TYPE_KEYDOWN, k, 0, 0);
			break;

		case SDL_QUIT:
			OS_quit();
			break;

		default:
			break;
		}
	}


	timenow = getNanoTime();

	while (timenow - ltime < (frameTime))
	{
		timenow = getNanoTime();
	}

	display();
	ltime = timenow;
}

void display()
{
	if (recordVideo)
	{
		SceneManager_pump(1.0 / 25.0, evts);
	}
	else
	{
		SceneManager_pump(timenow - ltime, evts);
	}
	if (hasQuit)
		return;

	SceneManager_eventsZero(evts);
	if (recordVideo)
	{
		if (buff == 0)
			buff = malloc(width * height * 3);


		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buff);
		fwrite(buff, width * height *3, 1, videoFile);
	}

	SDL_GL_SwapBuffers();
}

const char *OS_getFileName(const char *fileName)
{
	static char ret[MAX_PATH];
#ifdef WIN32
	wchar_t defDirW[MAX_PATH *2];
	char defDir[MAX_PATH];

	GetCurrentDirectoryW(MAX_PATH, defDirW);
	WideCharToMultiByte(CP_UTF8, 0, defDirW, -1, defDir, MAX_PATH * 2, NULL, NULL);

	sprintf(ret, "%s%sdata%s%s", defDir, SEPARATOR, SEPARATOR, fileName);
	return w32_getFileName(ret);
#endif
#ifndef WIN32
	sprintf(ret, "%s/%s/%s", PACKAGE_DATA_DIR, PACKAGE, fileName);
#endif
	return ret;
}

FILE *OS_getFile(const char *fileName)
{
	FILE *ph = fopen(OS_getFileName(fileName), "rb");
	return ph;

}

size_t OS_getResource(const char *resName, char **data)
{
	FILE *inph = OS_getFile(resName);

	if (inph == NULL)
		return 0;

	fseek(inph, 0, SEEK_END);
	size_t len = ftell(inph);
	fseek(inph, 0, SEEK_SET);

	*data = malloc(len);
	size_t ret = fread(*data, 1, len, inph);
	fclose(inph);
	return ret;
}

const char *OS_getPref(const char *name, const char *defVal)
{
	return settings_get(name, defVal);
}

void OS_setPref(const char *name, const char *value)
{
	settings_set(name, value);
}


void OS_playSound(int id)
{
	if(!OS_sound_On)
		return;

	if (id == OS_snd_phlip)
		Mix_PlayChannel(0, snd_phlip, 0);

	if (id == OS_snd_click)
		Mix_PlayChannel(0, snd_click, 0);

	if (id == OS_snd_click2)
		Mix_PlayChannel(0, snd_click2, 0);

	if (id == OS_snd_buzz)
		Mix_PlayChannel(0, snd_buzz, 0);


}

void OS_playMusic(int id)
{
	if(!OS_sound_On)
		return;

	if (id == OS_music_theme)
		Mix_FadeInMusic(music_theme, -1, 0);

	if (id == OS_music_success)
		Mix_FadeInMusic(music_success, 0, 0);

	if (id == OS_music_failure)
		Mix_FadeInMusic(music_failure, 0, 0);

}

void OS_stopMusic(int id)
{
	Mix_FadeOutMusic(200);
}

int OS_getMouseX()
{
	return mouseX;
}

int OS_getMouseY()
{
	return mouseY;
}
