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
#ifndef BASIC_SCENE_MANAGER_H
#define BASIC_SCENE_MANAGER_H

class RectGUI;
class SliderGUI;
#include "Node.h"
#include "../sceneObjects/SceneObject.h"
#include "../shader/Shader.h"
#include "../shader/shadow/DepthMapShader.h"
#include "../shader/gui/AxisShader.h"
#include "../sceneObjects/mesh/Mesh.h"
#include "../sceneObjects/mesh/MeshManager.h"
#include "../sceneObjects/camera/Camera.h"
#include "../sceneObjects/light/Light.h"
#include "../utils/Maths.h"
#include "RTT/BasicRTT.h"
#include "RTT/DepthRTT.h"
#include "RTT/PickRTT.h"
#include "../GUI/Rect.h"
#include "../GUI/GUIUtils/SliderGUI.h"

#include <string>
#include <vector>
#include <queue>

/**
 * Class that manages the rendering parts, scenograph and creation of scene object
 */
class BasicSceneManager {
public:
	
    BasicSceneManager();
	
    /**
     * Destroy nodes and internal data, does not destroy meshes, textures and Text but destroys rects, incoherence
     */
    ~BasicSceneManager();

    /**
     * void method
     */
	virtual void loadScene();
    
    /**
     * loadScene loads a scene from xml file
     * @param std::string fileName to a file with xml and a specific format
     */
	void loadScene(std::string fileName);
    
    /**
     * saveScene writes the current scene into a xml file
     */
    void saveScene();
    
    /**
     * closeScene is called by de destructor but it can be called outside to reset the scene
     */
    virtual void closeScene();

    /**
     * createCamera creates a perspective camera
     * @return Camera*
     */
	Camera* createCamera();
    
    /**
     * createMesh creates a mesh from file
     * @param std::string name is the filepath from assets folder to mesh file, is accepted .obj and md5mesh formats
     * @return Mesh*
     */
	Mesh* createMesh(string name);
    
    /**
     * createPlaneMesh creates a plane mesh of side 1, centered at (0,0,0) and aligned in x,y plane
     * @return Mesh*
     */
    Mesh* createPlaneMesh();
    
    /**
     * createRectangleGUI returns a complete Rect for 2D graphics, with all screen size
     * @return RectGUI*
     */
	RectGUI* createRectangleGUI();
    
    /**
     * createRectangleBackground RectGUI that will be rendered at the end with depth buffer enabled
     * @return RectGUI*
     */
	RectGUI* createRectangleBackground();
    
    /**
     * createRectangleGUI returns a complete Rect for 2D graphics with custom position and size
     * @param float left
     * @param float top
     * @param float w
     * @param float t
     * @return RectGUI*
     */
	RectGUI* createRectangleGUI(float left, float top, float w, float h);
	SliderGUI* createSliderGUI(float left, float top, float w, float h, int type);
    
    /**
     * getRootNode returns the root node of the scene
     * @return Node*
     */
	Node* getRootNode();
    
    /**
     * createLight creates a light object
     * @return Light*
     */
	Light* createLight();
    
    /**
     * setAmbientLight of the scene
     * @param glm::vec3 ambient light in rgb format
     */
	void setAmbientLight(glm::vec3 ambient);
    
    /**
     * updateVariables is called before start rendering, it updates the nodes and gets the correct models, views and projections matrix
     */
	void updateVariables();

    /**
     * createShadowMap renders a depth texture in case of having the RTTDepth setted
     */
	void createShadowMap();
    
    /**
     * cleanBuffers cleans pixel, depth and stencil buffer
     */
	void cleanBuffers();
    
    /**
     * render makes all the needed calls to render the scene
     */
	void render();
    
    /**
     * renderGUI draws all the RectGUI objects
     */
	void renderGUI();
    
    /**
     * renderBackground renders the RectGUI from createRectangleBackground
     */
	void renderBackground();
    
    /**
     * setNodeToShowAxis shows the 3 axis on the node space
     * @param Node* nodeWithAxis to attach the mesh
     */
	void setNodeToShowAxis(Node* nodeWithAxis);
    
    /**
     * getRectTouch returns the RectGUI pointer or 0 in case of any gui touch
     * @param float x
     * @param float y
     * @return RectGUI* touched or 0
     */
	RectGUI* getRectTouch(float x, float y);
    
    /**
     * getIdObjectSelected returns the id of the object selected, it works with color picking, it have large cost because needs to stop to rasterize the scene, usese PickRTT class
     * @param float xClick
     * @param float yClick
     * @return int id
     */
	int getIdObjectSelected(int xClick, int yClick);
    
	void setColorClean(glm::vec3 color);
 //   void cleanGUI();
 //   void cleanBackground();
private:
	glm::vec3 colorBufferClean;
	void sortMeshes();
	//shadow vars
	glm::mat4 projectionLight, viewLight;
	glm::mat4 viewTransLight, projViewLight;
	glm::vec3 lightWorldPosition, lightWorldDirection;
	//scene vars
	Camera* currentCamera;
	glm::mat4 projection, view, viewTrans, projView;
	glm::vec4 light_diffuse, light_specular, light_position, light_direction;
	Light* currentLight;

	bool shadows;
	DepthRTT* depthRTT;
	PickRTT* pickRTT;

	Camera* getFirstCameraEnabled();
	Camera* getFirstCameraNotEnabled();
	Light* getFirstLightEnabled();
	Node* root;

	vector<Mesh*> meshVector;
	vector<Camera*> cameraVector;
	vector<Light*> lightVector;

	priority_queue<Mesh*> withoutAlpha;
	priority_queue<Mesh*> withAlpha;
	priority_queue<Mesh*> castShadow;
	vector<RectGUI*> guiRect;
	vector<SliderGUI*> guiSlider;
	RectGUI* guiBackground;

	glm::vec3 ambientLight;

	void initDebug();
	Mesh* debugPlane;

	void initAxisVectorMesh();
	Mesh* rightVectorMesh;
	Mesh* upVectorMesh;
	Mesh* forwardVectorMesh;
	Node* nodeToPrintAxis;
	AxisShader* shaderAxis;
};

#endif
