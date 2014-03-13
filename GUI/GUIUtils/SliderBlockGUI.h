/*
 *  thin render - Mobile render engine based on OpenGL ES 2.0
 *  Copyright (C) 2013  Fundació i2CAT, Internet i Innovació digital a Catalunya
 *
 *  This file is part of thin render.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Author:         Ignacio Contreras Pinilla <ignacio.contreras@i2cat.net>
 *                  Marc Fernandez Vanaclocha <marc.fernandez@i2cat.net>
 */

#ifndef SLIDER_BLOCK_GUI_H
#define SLIDER_BLOCK_GUI_H

#include "SliderGUI.h"

class SliderBlockGUI: public SliderGUI {
public:
	SliderBlockGUI(float left, float top, float width, float height, int type = HORIZONTAL_SLIDER) : SliderGUI(left, top, width, height, type){
		guiObjectType = SLIDER_BLOCK_OBJECT;
		index = 0;
	}
	~SliderBlockGUI();

	void update(float xDiff, float yDiff, bool input = true);
	int getIndex();
	void includeRect(RectGUI* rect, float offset = 10.0f);
	RectGUI* getCurrentRect();
	RectGUI* getRect(int i);
private:
	int index;
};

#endif
