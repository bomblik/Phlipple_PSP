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

#ifndef LEVELS_H_
#define LEVELS_H_

struct _LevelDesc
{
	char phname[32];
	char *tutDesc;
	int tutPic;
};

typedef struct _LevelDesc LevelDesc;

extern LevelDesc **levels;
extern int nLevels;

void levels_load();
void levels_free();

#define TUTDESC_1	"Use arrows to flip, A & D to rotate"
#define TUTDESC_2	"Red joints work as hinges, they cannot be broken"
#define TUTDESC_3	"Flipping can be done both ways"

#endif /* LEVELS_H_ */
