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

#include "gfxconstants.h"

float gradCol0[4] = 					{.4, .4, .9, 1};
float gradCol1[4] = 					{0, 0, .2, 1};
float mainGradCol0[4] = 				{.3, 0, .3, 1};
float mainGradCol1[4] = 				{.0, .0, .0, 1};

float splashBackRgb[4] = {139.0/256.0, 172.0/256.0, 15.0/256.0, 1.0};
float txtColSuccess[3] = 				{1, 1, 1};
float txtColFailed[3] =					{1, 0, 0};
float hardLinkCol[3] = 					{1, 0, 0};

float activeQuadCol[3] = 				{0, 1, 1};
float quadCol[3] = 						{.5, .5, 1};
float lineCol[3] = 						{1, 1, 1};
float unaaccessibleCol[3] = 			{1, 0, 0};

float lineAlphaFar = 					.7;
float lineAlphaNear = 					.4;
float quadAlphaFar = 					.7;
float quadAlphaNear = 					.4;
float activeQuadAlpha = 				.8;

int screenRatioX = 18;
int screenRatioY = 12;
int screenGLHeight = 8;
