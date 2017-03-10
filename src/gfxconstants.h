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

#ifndef GFXCONSTANTS_H_
#define GFXCONSTANTS_H_

#define GFX_FAR_PLANE					30

#define GFX_TXT_SHADOW_OFFSET			0.05
#define GFX_ROT_BUTT_DISPLACEMENT		.5
#define GFX_ROT_BUTT_SCALE				2.0
#define GFX_ROT_BUTT_IDLE_ALPHA			.5
#define GFX_ROT_BUTT_PRESSED_ALPHA		1.0
#define GFX_PAUSE_BUTT_ALPHA			.5
#define GFX_PAUSE_BUTT_PRESSED_ALPHA	1.0
#define GFX_PAUSE_BUTT_SIZE				1.0


#define GFX_BUTT_X_OFFS					2.0
#define GFX_BUTT_Y_OFFS					1.0
#define GFX_BUTT_SIZE					1.5


#define COPYRIGHT						"Copyright (C) Remigiusz Dybka 2012"
#define SEL_LEV_TXT						"Select level"
#define LINUX_BROWSER_ANDROID			"xdg-open http://market.android.com/details?id=com.phuzzboxmedia.phlipple.android"
#define WIN32_BROWSER_ANDROID			"http://market.android.com/details?id=com.phuzzboxmedia.phlipple.android"


//#define SHOW_SPLASH


extern float gradCol0[4];
extern float gradCol1[4];
extern float mainGradCol0[4];
extern float mainGradCol1[4];

extern float splashBackRgb[4];

extern float txtColSuccess[3];
extern float txtColFailed[3];

extern float hardLinkCol[3];

extern float activeQuadCol[3];
extern float quadCol[3];
extern float lineCol[3];
extern float unaaccessibleCol[3];

extern float lineAlphaFar;
extern float lineAlphaNear;
extern float quadAlphaFar;
extern float quadAlphaNear;
extern float activeQuadAlpha;

extern int screenRatioX;
extern int screenRatioY;
extern int screenGLHeight;

extern float txtColSuccess[3];
extern float txtColFailed[3];

#endif /* GFXCONSTANTS_H_ */
