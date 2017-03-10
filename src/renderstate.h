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

#ifndef RENDERSTATE_H_
#define RENDERSTATE_H_

struct _RenderState
{
	// camera
	float eye[3];
	float centre[3];
	float up[3];
	Quad **zOrderList;
	int nQuads;

	int flipAnim;
	float flipTranslate[3];
	float flipRotate[4];
	int flipRotateExtra;
};

typedef struct _RenderState RenderState;


#endif /* RENDERSTATE_H_ */
