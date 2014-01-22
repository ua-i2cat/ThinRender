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
 *  Author:         Marc Fernandez Vanaclocha <marc.fernandez@i2cat.net>
 */

#ifndef SLIDER_GUI_H
#define SLIDER_GUI_H

#include "../Rect.h"
#include "../../scene/Node.h"
#include <vector>

class SliderGUI {
public:
	const static int HORIZONTAL_SLIDER = 0;
	const static int VERTICAL_SLIDER = 1;

	SliderGUI(float left, float top, float width, float height, int type = HORIZONTAL_SLIDER);
	~SliderGUI();

	void includeRect(RectGUI* rect, float offset = 10.0f);
	void includeAtrezzoRect(RectGUI* rect, float offset = 10.0f, float atrezzoOffset = 0.0f);
	virtual void update(float xDiff, float yDiff, bool input = true);
	void draw();
	bool isInside(float x, float y);
	RectGUI* click(float x, float y);

	void setEnabled(bool enabled);
protected:
	std::vector<RectGUI*> rects;
	std::vector<RectGUI*> rectsAtrezzo;
	Node* internalNode;
	float left, top, width, height;
	float minTranslation, maxTranslation;
	int type;
	glm::mat4 projMatrix;
	bool enabled;
};
#endif
