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

#ifndef OSINTERFACE_H_
#define OSINTERFACE_H_

#include <stdio.h>
#include "scenes.h"

void OS_init(int *argc, char **argv);
int OS_mainLoop();
void OS_quit();
extern int OS_sound_On;

const char *OS_getPref(const char *name, const char *defVal);
void OS_setPref(const char *name, const char *value);
size_t OS_getResource(const char *resName, char **data);
FILE *OS_getFile(const char *fileName);
const char *OS_getFileName(const char *fileName);
void OS_playSound(int id);
void OS_playMusic(int id);
void OS_stopMusic();
int OS_getMouseX();
int OS_getMouseY();

enum OS_keys {OS_key_left,
              OS_key_right,
              OS_key_up,
              OS_key_down,
              OS_key_rol,
              OS_key_ror,
              OS_key_ok,
              OS_key_cancel
             };

enum OS_music { OS_music_theme,
                OS_music_success,
                OS_music_failure
              };

enum OS_snd { OS_snd_phlip,
              OS_snd_click,
              OS_snd_click2,
              OS_snd_buzz
            };

#endif /* OSINTERFACE_H_ */
