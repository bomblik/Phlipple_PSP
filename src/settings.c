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
#include <shlobj.h>
#include <shlwapi.h>
#include <direct.h>
#endif

#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>

#include "settings.h"

static Setting *settings = 0;

#ifdef WIN32
const char *w32_getFileName(const char *phname)
{
	wchar_t wfn[MAX_PATH];
	wchar_t pthname[MAX_PATH];
	static char shrt[MAX_PATH];

	MultiByteToWideChar(CP_UTF8, 0, phname, -1, wfn, MAX_PATH);
	if (GetShortPathNameW(wfn, pthname, MAX_PATH) > 0)
	{
		WideCharToMultiByte(CP_UTF8, 0, pthname, -1, shrt, MAX_PATH, NULL, NULL);
		return shrt;
	}
	else
	{
		return 0;
	}
}

void w32_createFile(const char *phname)
{
	const char *ret = w32_getFileName(phname);

	if (ret == 0)
	{
		wchar_t wfn[MAX_PATH];
		wchar_t wb[16];

		MultiByteToWideChar(CP_UTF8, 0, phname, -1, wfn, MAX_PATH);
		MultiByteToWideChar(CP_UTF8, 0, "wb", -1, wb, 16);

		FILE *ph = _wfopen(wfn, wb);
		fclose(ph);
	}
}
#endif //WIN32


const char *getDefaultDir()
{
	static char defDir[MAX_PATH];

#ifdef PLATFORM_PSP
	return "phlipple";
#endif

#ifndef WIN32
	char *dir = getenv(ENVVAR);
	sprintf(defDir, "%s%s%s", dir, SEPARATOR, DEFAULT_DIRECTORY);
	return defDir;
#endif //WIN32

#ifdef PORTABLE_APPS
	wchar_t defDirW[MAX_PATH *2];
	static char ret[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, defDirW);
	WideCharToMultiByte(CP_UTF8, 0, defDirW, -1, defDir, MAX_PATH * 2, NULL, NULL);

	sprintf(ret, "%s%s..%s..%sdata%ssettings", defDir, SEPARATOR, SEPARATOR, SEPARATOR, SEPARATOR);

	return ret;
#endif

#ifdef WIN32
	char mbpath[MAX_PATH *2];
	wchar_t path[MAX_PATH *2];
	if (SUCCEEDED(SHGetFolderPathW(NULL,
	                               CSIDL_APPDATA,
	                               NULL,
	                               0,
	                               path)))
	{
		if (WideCharToMultiByte(CP_UTF8, 0, path, -1, mbpath, MAX_PATH * 2, NULL, NULL))
		{
			sprintf(defDir, "%s%s%s", mbpath, SEPARATOR, DEFAULT_DIRECTORY);
			return defDir;
		}
		else
		{
			return 0;
		}

	}
	else
	{
		return 0;
	}
#endif //WIN32

}

const char *getSettingsFileName()
{
	static char fn[1024];
	sprintf(fn, "%s%s%s", getDefaultDir(), SEPARATOR, DEFAULT_FILENAME);
	return fn;
}

void strip(char *b)
{
	while(!isalnum(b[0]) && strlen(b))
	{
		int ll = strlen(b);
		for (int l = 0; l < ll - 1; l++)
		{
			b[l] = b[l + 1];
		}
	}

	while(!isalnum(b[strlen(b) - 1]) && strlen(b))
		b[strlen(b) - 1] = 0;
}

void parseLine(const char *buff)
{
	// do we have a colon?
	int ll = strlen(buff);
	int sep = -1;
	for (int l = 0; l < ll; l++)
	{
		if (buff[l] == ':')
			sep = l;
	}

	if (sep <= 0)
		return;

	char *key = malloc(sep);
	char *val = malloc(ll - sep);

	strncpy(key, buff, sep);
	key[sep] = 0;
	strcpy(val, buff + sep + 1);
	strip(key);
	strip(val);

	if (strlen(key) && strlen(val))
		settings_set(key, val);
}

void settings_load()
{
	// make sure the settings directory exists
#ifndef WIN32
	mkdir(getDefaultDir(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif
#ifdef WIN32
	wchar_t wd[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, getDefaultDir(), -1, wd, MAX_PATH);
	_wmkdir(wd);
#endif

	FILE *inph = fopen(getSettingsFileName(), "rb");
	if (inph)
	{
		char buff[1024];
		while(fgets(buff, 1024, inph))
		{
			parseLine(buff);
		}
		fclose(inph);
	}
}

void settings_save()
{
#ifndef WIN32
	FILE *outph = fopen(getSettingsFileName(), "wb");
#endif
#ifdef WIN32
	w32_createFile(getSettingsFileName());
	FILE *outph = fopen(w32_getFileName(getSettingsFileName()), "wb");
#endif
	Setting *next = settings;
	while(next != 0)
	{
		fprintf(outph, "%s:%s\n", next->key, next->value);
		next = next->next;
	}
	fclose(outph);
}

void settings_clear()
{
	if (!settings)
		return;

	Setting *next = settings;
	while(next != 0)
	{
		Setting *curr = next;
		next = next->next;
		free(curr->key);
		free(curr->value);
		free(curr);
	}

	settings = 0;
}

void append(Setting **s, const char *key, const char *value)
{
	*s = malloc(sizeof(Setting));
	(*s)->next = 0;
	(*s)->key = malloc(strlen(key) + 1);
	(*s)->value = malloc(strlen(value) + 1);
	strcpy((*s)->key, key);
	strcpy((*s)->value, value);
}

void settings_set(const char *key, const char *value)
{
	if (settings == 0)
	{
		append(&settings, key, value);
		return;
	}

	Setting *found = 0;
	Setting *next = settings;
	Setting *last = settings;

	while(next != 0)
	{
		if (strcmp(next->key, key) == 0)
		{
			found = next;
			next = 0;
			continue;
		}

		last = next;
		next = next->next;
	}

	if (found)
	{
		found->value = realloc(found->value, strlen(value) + 1);
		strcpy(found->value, value);
	}
	else
	{
		append(&(last->next), key, value);
	}
}

const char *settings_get(const char *key, const char *defValue)
{
	Setting *next = settings;
	while(next != 0)
	{
		if (strcmp(next->key, key) == 0)
		{
			return next->value;
		}

		next = next->next;
	}

	return defValue;
}


