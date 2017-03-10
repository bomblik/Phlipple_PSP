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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#ifdef WIN32
#define ENVVAR	  "APPDATA"
#define SEPARATOR "\\"
const char *w32_getFileName(const char *phname);
#else
#include <limits.h>
#define MAX_PATH PATH_MAX
#define ENVVAR	  "HOME"
#define SEPARATOR "/"
#endif

#define DEFAULT_FILENAME "settings"
#define DEFAULT_DIRECTORY ".phlipple"

struct _Setting
{
	char *key;
	char *value;
	struct _Setting *next;
};

typedef struct _Setting Setting;

void settings_load();
void settings_save();
void settings_set(const char *key, const char *value);
const char *settings_get(const char *key, const char *defValue);

#endif //__SETTINGS_H__
