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

#ifndef RECT_H
#define RECT_H

#include "../utils/Maths.h"
#include "../scene/Node.h"
#include "../texture/TextureManager.h"
#include "../texture/Texture.h"
#include "../texture/Image.h"
#include "../sceneObjects/mesh/Mesh.h"
#include "../sceneObjects/mesh/MeshManager.h"
#include "../globalData/GlobalData.h"
#include "../controllerEGL/ContextControllerEGL.h"
#include "../log/Log.h"
#include "Text.h"
#include <string>

/**
 * Basic class to create 2D graphics
 * This class manages:
 *  -Area
 *  -Translation
 *  -Texture
 *  -Text
 */
class  RectGUI {
public:
    /**
     * Constructor all params comes from top left (1.0, 0.0) to bottom right (0.0, 1.0)
     * @param top float
     * @param left float
     * @param width float
     * @param height float
     * @param Node* to attach the RectGUI
     */
	RectGUI(float top, float left, float width, float height, Node* nodeRect);
    /**
     * the previous constructor with different orther (WTF!)
     */
	RectGUI(Node* nodeRect, float left, float top, float w, float h);
    /**
     *  Destructor that only destroy the internal mesh, the Text pointer and Texture pointer are controlled by the respectives managers
     */
	~RectGUI();
    /**
     *  Sets a new texture, the previous one is not deleted (deleting textures is part of TextureManager)
     */
	void setTexture(Texture* texture);

    /**
     * Reload the texture using a char*, is expected the same image format
     * @param unsigned char* imageData pointer to the new raw image
     */
    //better const char*?
    //the texture expected have the same format that the initial one of Texture pointer
	void setTexture(unsigned char* imageData);
    /**
     * Reload the texture using a an opengl pointer to 2D texture, the ratio is used if this texture2D isn't a npot image
     * @param unsigned int glTextId the glTexture2D int
     * @param float xRatio
     * @param float yRatio
     */
    //TODO
	void setTexture(unsigned int glTextId, float xRatio = 1.0f, float yRatio = 1.0f);//X and Y ratio only if the image is a npot image

    /**
     * setPosition method actualizes the internal node with the current position
     * @param float newLeft
     * @param float newTop
     */
    void setPosition(float newLeft, float newTop);
    
    /**
     * setWidth method actualizes the internal width maintaining top and left
     * @param float newWidth
     */
	void setWidth(float newWidth);
    
    /**
     * setHeight method actualizes the internal height maintaining top and left
     * @param float newHeight
     */
    void setHeight(float newHeight);
    
    /**
     * setWidthAndHeight method actualizes the internal width and height maintaining top and left
     * @param float newWidth
     * @param float newHeight
     */
	void setWidthAndHeight(float newWidth, float newHeight);
    
    /**
     * setShader destroy the previous shader and sets the new one
     * @param Shader* shader (expected a shader for gui...)
     */
	void setShader(Shader* shader);

    /**
     * draw method call the different render methods for the rectangle and text
     */
	void draw();
    /**
     * draw method call the different render methods for the rectangle and text
     */
	void draw(glm::mat4 intermediateMatrix);

    /**
     * getTop
     * @return float with top of rectangle from screen space (1 is top of screen and 0 bottom)
     */
	float getTop();
    
    /**
     * getLeft
     * @return float with left of rectangle from screen space (1 is right of screen and 0 left)
     */
	float getLeft();
    
    /**
     * getWidth
     * @return float with left of rectangle from screen space (1 is right of screen and 0 left)
     */
	float getWidth();
    
    /**
     * getHeight
     * @return float with left of rectangle from screen space (1 is right of screen and 0 left)
     */
	float getHeight();
 
    /**
     * setText does not deletes the previous text, is managed via TextManager
     * @param std::string message to set at Text class
     * @param std::string typography file from assets path, the file format accepted are ttf
     * @param int fontSize
     */
	void setText(std::string message, std::string typographyFile, int fontSize);
	void setBlockText(std::string message, std::string typographyFile, int fontSize, int align = 0);
    
    /**
     * isInside calculates the AABB normalized
     * @param float x
     * @param float y
     * @return bool true if x and y are inside the rect and is clickable false otherwise
     */
	bool isInside(float x, float y);
    
    /**
     * setClickable sets if rect is clicable or not, affects directly to isInside method
     * @param bool click
     */
	void setClickable(bool click);
    
    /**
     * getMessage
     * @return std::string with the text content
     */
	std::string getMessage();
    /**
     * setTextColor sets only text color
     * @param glm::vec4 with color in rgba format
     */
	void setTextColor(glm::vec4 color);
    /**
     * setColor sets only rect color
     * @param glm::vec4 with color in rgba format
     */
    void setColor(glm::vec4 color);
    void setEnabled(bool enabled);
    bool getEnabled();
private:
	Mesh* plane;
	Node* node;
	float top, left, width, height;
	bool clickable;
	Text* text;
	std::string textMessage;
	glm::mat4 projMatrix;

	glm::vec4 color;

	void setProjectionMatrix();
	bool enabled;
};

#endif
