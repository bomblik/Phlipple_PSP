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
#include <string.h>
#include "levels.h"
#include "osinterface.h"

LevelDesc **levels = NULL;
int nLevels = 0;

char *getLine(char **buff, char*);

void levels_load()
{
	char *buff;
	char *tofree;
	char *end;

	buff = tofree = 0;

	int l = OS_getResource("levels.txt", &buff);
	if (l == 0)
		return;

	tofree = buff;
	end = tofree + l;

	nLevels = 0;
	while (buff < (tofree + l))
	{
		char *line = getLine(&buff, end);
		if (strlen(line) > 2)
			nLevels++;
	}

	buff = tofree;

	levels = malloc(sizeof(LevelDesc *) * nLevels);

	for (int f = 0; f < nLevels; f++)
	{
		char *line = getLine(&buff, end);

		levels[f] = malloc(sizeof(LevelDesc));
		levels[f]->tutDesc = NULL;
		levels[f]->tutPic = 0;
		strcpy(levels[f]->phname, line);
	}

	free(tofree);

	levels[0]->tutDesc = TUTDESC_1;
	levels[2]->tutDesc = TUTDESC_2;
	levels[3]->tutDesc = TUTDESC_3;

	levels[0]->tutPic = 1;
	levels[2]->tutPic = 2;
	levels[3]->tutPic = 3;
}

void levels_free()
{
	if (nLevels == 0)
		return;

	for (int f = 0; f < nLevels; f++)
		free(levels[f]);

	nLevels = 0;

	free(levels);
}
