//----------------------------------------------------------------------------------------
/**
* \file       main.cpp
* \author     Jaroslav Hrach
* \date       2015
* \brief      Main file of the project.
*/
//----------------------------------------------------------------------------------------

#include <time.h>
#include <iostream>
#include <list>
#include "pgr.h"
#include "parameters.h"
#include "objects.h"
#include "spline.h"

//shader programs
extern SCommonShaderProgram shaderProgram;
extern SSkyboxShaderProgram skyboxShaderProgram;

//list for objects in the scene
typedef std::vector<void *> ObjectsList;

struct GameState {

	int windowWidth;
	int windowHeight;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	int activeCamera;
	bool cameraNeedsSetup;
	bool freeCamera;
	float cameraElevationAngle;

	bool keyMap[KEYS_COUNT];

	int flashlightEnable;
	float flashlightIntensity;
	int fogEnable;
	int fogAutomatic;
	int lampEnable;
	bool scannerAnimated;
	
	float elapsedTime;

} gameState;

struct Objects {
	//camera
	CameraObject *camera;

	//objects
	FloorObject *floor;
	AlienObject  *alien;
	ScannerObject  *scanner;
	UfoObject *ufo;
	LampObject *lamp;
	CargoObject *cargo;
	StopObject *stop;
	SwarmObject *swarm;
	SwarmObject *swarm2;
	CatObject* cat;

	//objects list
	ObjectsList boxes;
	ObjectsList explosions;

} objects;

/**
* Checks whether a given point is inside a sphere or not.
* \param[in]  point      Point to be tested.
* \param[in]  center     Center of the sphere.
* \param[in]  radius     Radius of the sphere.
* \return                True if the point lies inside the sphere, otherwise false.
*/
bool pointInSphere(const glm::vec3 &point, const glm::vec3 &center, float radius) {
	
	float x = (point.x - center.x);
	float y = (point.y - center.y);
	float z = (point.z - center.z);

	float distance = x*x + y*y + z*z;

	if (distance <= radius*radius) return true;

	return false;
}


void insertExplosion(const glm::vec3 & position) {

	ExplosionObject* newExplosion = new ExplosionObject;

	newExplosion->speed = 0.0f;
	newExplosion->notDestroyed = true;
	newExplosion->startTime = gameState.elapsedTime;
	newExplosion->currentTime = newExplosion->startTime;
	newExplosion->size = 0.1f;
	newExplosion->direction = glm::vec3(0.0f, 0.0f, 1.0f);
	newExplosion->frameDuration = 0.1f;
	newExplosion->textureFrames = 16;
	newExplosion->position = position;

	objects.explosions.push_back(newExplosion);
}

/**
* Turns camera to left.
* \param[in] deltaAngle   Angle to turn camera.
*/
void turnCameraLeft(float deltaAngle) {
	objects.camera->viewAngle += deltaAngle;
	if (objects.camera->viewAngle>360.0f)	objects.camera->viewAngle -= 360.0f;
	objects.camera->direction = glm::vec3(cos(glm::radians(objects.camera->viewAngle)), sin(glm::radians(objects.camera->viewAngle)), 0.0f);
}

/**
* Turns camera to right.
* \param[in] deltaAngle   Angle to turn camera.
*/
void turnCameraRight(float deltaAngle) {
	objects.camera->viewAngle -= deltaAngle;
	if (objects.camera->viewAngle<0.0f)
		objects.camera->viewAngle += 360.0f;
	objects.camera->direction = glm::vec3(cos(glm::radians(objects.camera->viewAngle)), sin(glm::radians(objects.camera->viewAngle)), 0.0f);
}

/**
* Clean all objects in ObjectList.
*/
void cleanUpObjects(void) {

	// delete asteroids
	while (!objects.boxes.empty()) {
		delete objects.boxes.back();
		objects.boxes.pop_back();
	}
}

/**
* generates random position that does not collide with camera
*/
glm::vec3 generateRandomPosition(void) {
	glm::vec3 newPosition;
	bool invalidPosition = false;

	do {
		// position is generated randomly
		newPosition = glm::vec3((float)(3.0 * (rand() / (double)RAND_MAX) - 1.0), (float)(3.0 * (rand() / (double)RAND_MAX) - 1.0), 0.045f);
		invalidPosition = pointInSphere(newPosition, objects.camera->position, 3.0f*CAMERA_SIZE);
		if (invalidPosition == false) invalidPosition = pointInSphere(newPosition, objects.cargo->position, 3.0f*CARGO_SIZE);
	} while (invalidPosition == true);

	return newPosition;
}

/**
*	Creates a floor.
*   \return New FloorObject
*/
FloorObject* createFloor(void){
	FloorObject* newFloor = new FloorObject;

	newFloor->size = FLOOR_SIZE;
	newFloor->position = glm::vec3(0.0f, 0.0f, -0.015f);
	
	return newFloor;
}

/**
*	Creates a new alien.
*   \return New AlienObject
*/
AlienObject* createAlien(void){
	AlienObject* newAlien = new AlienObject;

	//newAlien->position = glm::vec3(-0.4f, -0.4f, 0.065f);
	newAlien->position = glm::vec3(0.0f, 0.0f, 0.065f);
	newAlien->size = ALIEN_SIZE;
	newAlien->collision = glm::length(glm::vec2((objects.camera->position.x - newAlien->position.x), (objects.camera->position.y - newAlien->position.y)));
	newAlien->direction = glm::vec3((float)(2.0 * (rand() / (double)RAND_MAX) - 1.0), (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0), 0.0f);
	
	return newAlien;
}

/**
*	Creates a new scanner.
*   \return New ScannerObject
*/
ScannerObject* createScanner(void){
	ScannerObject* newScanner = new ScannerObject;

	newScanner->initPosition = glm::vec3(0.0f, 0.0f, 0.5f);
	newScanner->position = newScanner->initPosition;
	newScanner->size = SCANNER_SIZE;
	newScanner->radius = 0.2f;

	newScanner->direction = glm::vec3((float)(2.0 * (rand() / (double)RAND_MAX) - 1.0), (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0), 0.0f);
	newScanner->direction = glm::normalize(newScanner->direction);
	newScanner->startTime = gameState.elapsedTime;

	return newScanner;
}

/**
*	Creates a new cargo.
*   \return New CargoObject
*/
CargoObject* createCargo(void){
	CargoObject* newCargo = new CargoObject;

	newCargo->position = glm::vec3(0.0f, 1.0f, 0.10f);
	newCargo->size = CARGO_SIZE;
	newCargo->direction = glm::vec3(-0.8f, -0.7f, 0.0f);

	return newCargo;
}

/**
*	Creates a new stop.
*   \return New StopObject
*/
StopObject* createStop(void){
	StopObject* newStop = new StopObject;

	newStop->position = glm::vec3(0.8f, 0.0f, 0.10f);
	newStop->size = STOP_SIZE;
	newStop->direction = glm::vec3(-0.2f, 1.0f, 0.0f);

	return newStop;
}

/**
*	Creates a new swarm.
*   \return New SwarmObject
*/
SwarmObject* createSwarm(void){
	SwarmObject* newSwarm = new SwarmObject;

	newSwarm->position = glm::vec3(0.8f, 0.2f, 0.03f);
	newSwarm->size = SWARM_SIZE;
	newSwarm->direction = glm::vec3(-1.0f, 0.1f, 0.0f);
	newSwarm->collision = glm::length(glm::vec2((objects.camera->position.x - newSwarm->position.x), (objects.camera->position.y - newSwarm->position.y)));

	return newSwarm;
}

/**
*	Creates a new cat.
*   \return New CatObject
*/
CatObject* createCat(void){
	CatObject* newCat = new CatObject;

	newCat->size = CAT_SIZE;
	newCat->position = generateRandomPosition();
	newCat->position.z = 0.02f;
	newCat->direction = glm::vec3((float)(2.0 * (rand() / (double)RAND_MAX) - 1.0), (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0), 0.0f);
	newCat->direction = glm::normalize(newCat->direction);

	return newCat;
}

/**
*	Creates a new lamp.
*   \return New LampObject
*/
LampObject* createLamp(void){
	LampObject* newLamp = new LampObject;

	newLamp->position = glm::vec3(0.0f, 0.4f, 0.1f);
	newLamp->size = LAMP_SIZE;
	newLamp->radius = 0.2f;
	
	return newLamp;
}

/**
*	Creates a new box.
*   \return New BoxObject
*/
BoxObject* createBox(void) {
	BoxObject* newBox = new BoxObject;

	newBox->notDestroyed = true;
	newBox->startTime = gameState.elapsedTime;
	newBox->currentTime = newBox->startTime;
	newBox->size = BOX_SIZE;

	newBox->direction = glm::vec3((float)(2.0 * (rand() / (double)RAND_MAX) - 1.0), (float)(2.0 * (rand() / (double)RAND_MAX) - 1.0), 0.0f);
	newBox->direction = glm::normalize(newBox->direction);

	newBox->position = generateRandomPosition();
	newBox->rotationSpeed = 1.0f * (float)(rand() / (double)RAND_MAX);

	return newBox;
}

/**
*	Creates a new ufo.
*   \return New UfoObject
*/
UfoObject* createUfo() {
	UfoObject *newUfo = new UfoObject;

	newUfo->position = glm::vec3(-0.5f, -0.4f, 0.2f);
	newUfo->size = 0.1f;
	newUfo->time = 0.0f;
	newUfo->rotationAngle = glm::radians(5.0f);
	newUfo->direction = 0;

	return newUfo;
}

/**
*	Sets up camera according to number of active camera
*
*/
void setupCamera(void){
	//static 1
	if (gameState.activeCamera == 0) {
		objects.camera->position = glm::vec3(1.5f, 0.2f, 0.09f);
		objects.camera->viewAngle = 175.0f;
		objects.camera->direction = glm::vec3(cos(glm::radians(objects.camera->viewAngle)), sin(glm::radians(objects.camera->viewAngle)), 0.0f);
		objects.camera->startTime = gameState.elapsedTime;
		objects.camera->currentTime = objects.camera->startTime;
		gameState.cameraElevationAngle = 10.0f;
	}
	//static 2
	else if (gameState.activeCamera == 1) {
		objects.camera->position = glm::vec3(0.88f, 0.87f, 0.5f);
		objects.camera->viewAngle = 230.0f;
		objects.camera->direction = glm::vec3(cos(glm::radians(objects.camera->viewAngle)), sin(glm::radians(objects.camera->viewAngle)), -0.2f);
		objects.camera->startTime = gameState.elapsedTime;
		objects.camera->currentTime = objects.camera->startTime;
		gameState.cameraElevationAngle = 10.0f;
	}
	gameState.cameraNeedsSetup = false;
}

/**
*	Changes settings to default, cleans objects and make new objects.
*
*/
void reloadScene(void) {

	cleanUpObjects();

	//set camera, create newOne and setup it
	gameState.activeCamera = 0;
	if (objects.camera == NULL)
		objects.camera = new CameraObject;
	
	objects.camera->size = CAMERA_SIZE;
	objects.camera->sprint = 0;

	setupCamera();

	objects.floor = createFloor();
	objects.alien = createAlien();
	objects.cargo = createCargo();
	objects.cat = createCat();
	objects.stop = createStop();
	objects.swarm = createSwarm();
	objects.swarm2 = createSwarm();
	objects.scanner = createScanner();
	objects.lamp = createLamp();
	objects.ufo = createUfo();

	// initialize asteroids
	int maxBoxes = BOXES_NUMBER;
	for (int i = 0; i < maxBoxes; i++) {
		BoxObject* newBox = createBox();

		objects.boxes.push_back(newBox);
	}

}


/**
*	Draws the scene.
*
*/
void drawSceneContent(void) {

	if (gameState.cameraNeedsSetup == true) setupCamera();
	
	// setup parallel projection
	glm::mat4 orthoProjectionMatrix = glm::ortho(
		-SCENE_WIDTH, SCENE_WIDTH,
		-SCENE_HEIGHT, SCENE_HEIGHT,
		-10.0f*SCENE_DEPTH, 10.0f*SCENE_DEPTH
		);
	gameState.projectionMatrix = orthoProjectionMatrix;
	glm::vec3 cameraPosition = objects.camera->position;
	glm::vec3 cameraCenter = objects.camera->position + objects.camera->direction;
	glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 cameraViewDirection = objects.camera->direction;
	glm::vec3 rotationAxis = glm::cross(cameraViewDirection, glm::vec3(0.0f, 0.0f, 1.0f)); //vektorovy soucin, osa podle ktere se rotuje
	glm::mat4 cameraTransform = glm::rotate(glm::mat4(1.0f), -1.0f*gameState.cameraElevationAngle, rotationAxis); //co, o kolik, podle ktere osy

	cameraUpVector = glm::vec3(cameraTransform * glm::vec4(cameraUpVector, 0.0f)); //transform. up vektoru
	cameraViewDirection = glm::vec3(cameraTransform * glm::vec4(cameraViewDirection, 0.0f));	//to same se smerem pohledu
	cameraCenter = cameraPosition + cameraViewDirection;

	gameState.viewMatrix = glm::lookAt(
		cameraPosition,
		cameraCenter,
		cameraUpVector
		);
	gameState.projectionMatrix = glm::perspective(60.0f, gameState.windowWidth / (float)gameState.windowHeight, 0.01f, 10.0f);
	
	// floor
	drawFloor(objects.floor, gameState.viewMatrix, gameState.projectionMatrix);
	
	// scanner
	drawScanner(objects.scanner, gameState.viewMatrix, gameState.projectionMatrix);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	int id = 0;
	// draw objects boom
	for (ObjectsList::iterator it = objects.boxes.begin(); it != objects.boxes.end(); ++it) {
		glStencilFunc(GL_ALWAYS, id + 1, 0xFF);
		BoxObject* box = (BoxObject*)(*it);
		drawBox(box, gameState.viewMatrix, gameState.projectionMatrix);
		id++;
	}
	
	glDisable(GL_STENCIL_TEST);

	// skybox
	drawSkybox(gameState.viewMatrix, gameState.projectionMatrix);

	// ufo
	drawUfo(objects.ufo, gameState.viewMatrix, gameState.projectionMatrix);

	// draw explosions with depth test disabled
	glDisable(GL_DEPTH_TEST);

	for (ObjectsList::iterator it = objects.explosions.begin(); it != objects.explosions.end(); ++it) {
		ExplosionObject* explosion = (ExplosionObject *)(*it);
		drawExplosion(explosion, gameState.viewMatrix, gameState.projectionMatrix);
	}
	glEnable(GL_DEPTH_TEST);
	
	// alien
	drawAlien(objects.alien, gameState.viewMatrix, gameState.projectionMatrix);

	// cargo
	drawCargo(objects.cargo, gameState.viewMatrix, gameState.projectionMatrix);

	// stop
	drawStop(objects.stop, gameState.viewMatrix, gameState.projectionMatrix);
	
	// swarm
	drawSwarm(objects.swarm, gameState.viewMatrix, gameState.projectionMatrix);
	drawSwarm(objects.swarm2, gameState.viewMatrix, gameState.projectionMatrix);
	
	objects.swarm2->position = glm::vec3(-0.15f, -0.55f, 0.07f);
	objects.swarm2->size = SWARM_SIZE * 1.5f;
	objects.swarm2->direction = glm::vec3(-0.42f, -0.9f, 0.0f);
	objects.swarm2->collision = glm::length(glm::vec2((objects.camera->position.x - objects.swarm2->position.x), (objects.camera->position.y - objects.swarm2->position.y)));

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// cat
	glStencilFunc(GL_ALWAYS, 88, 0xFF);
	drawCat(objects.cat, gameState.viewMatrix, gameState.projectionMatrix);
	
	// lamp
	glStencilFunc(GL_ALWAYS, 99, 0xFF);
	drawLamp(objects.lamp, gameState.viewMatrix, gameState.projectionMatrix);
	
	glDisable(GL_STENCIL_TEST);

	glUseProgram(shaderProgram.program);
	glUniform3fv(shaderProgram.lampPositionLoc, 1, glm::value_ptr(objects.lamp->position));
	glUniform1f(shaderProgram.lampIntensityLoc, 1.5f);

	glUniform3fv(shaderProgram.flashlightPositionLoc, 1, glm::value_ptr(objects.camera->position));
	glUniform3fv(shaderProgram.flashlightDirectionLoc, 1, glm::value_ptr(cameraViewDirection));
	glUseProgram(0);
}


/**
*	Callback for update the display.
*
*/
void displayCallback() {
	GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;

	glUseProgram(shaderProgram.program);
	// fog
	if (gameState.fogEnable == 1) {
		glUniform1i(shaderProgram.fogActiveLocation, 1);
	} else {
		glUniform1i(shaderProgram.fogActiveLocation, 0);
	}

	
	// flashlight
	if (gameState.flashlightEnable == 0) {
		glUniform1f(shaderProgram.flashlightIntensityLoc, gameState.flashlightIntensity);
	}
	else {
		glUniform1f(shaderProgram.flashlightIntensityLoc, gameState.flashlightIntensity);
	}

	// lamp
	if (gameState.lampEnable == 0) {
		glUniform1i(shaderProgram.lampActiveLoc, 0);
	}
	else {
		glUniform1i(shaderProgram.lampActiveLoc, 1);
	}

	glUseProgram(0);

	glUseProgram(skyboxShaderProgram.program);
	
	// fog
	if (gameState.fogEnable == 1) {
		glUniform1i(skyboxShaderProgram.fogActiveLocation, 1);
	}
	else {
		glUniform1i(skyboxShaderProgram.fogActiveLocation, 0);
	}
	glUseProgram(0);

	glClear(mask);
	drawSceneContent();
	glutSwapBuffers();
}

/**
*	Callback called when the window is resized.
*
*/
void reshapeCallback(int newWidth, int newHeight) {

	gameState.windowWidth = newWidth;
	gameState.windowHeight = newHeight;

	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

void updateObjects(float elapsedTime) {

	ObjectsList::iterator it = objects.boxes.begin();
	while (it != objects.boxes.end()) {
		BoxObject* asteroid = (BoxObject*)(*it);

		if (asteroid->notDestroyed == false) {
			it = objects.boxes.erase(it);
			std::cout << "boom" << std::endl;
		}
		else {
			++it;
		}
	}

	// update explosion billboards
	it = objects.explosions.begin();
	while (it != objects.explosions.end()) {
		ExplosionObject* explosion = (ExplosionObject*)(*it);

		// update explosion
		explosion->currentTime = elapsedTime;

		if (explosion->currentTime > explosion->startTime + explosion->textureFrames*explosion->frameDuration)
			explosion->notDestroyed = false;

		if (explosion->notDestroyed == false) {
			it = objects.explosions.erase(it);
		}
		else {
			++it;
		}
	}

	float curveParamT = 0.5f * (elapsedTime - objects.scanner->startTime);
	float curveAlienParamT = (elapsedTime - objects.scanner->startTime);

	objects.scanner->position = objects.scanner->initPosition + evaluateClosedCurve(curveData, curveSize, curveParamT);
	objects.scanner->direction = glm::normalize(evaluateClosedCurve_1stDerivative(curveData, curveSize, curveParamT));

	objects.alien->direction = glm::normalize(evaluateClosedCurve_1stDerivative(curveAlienData, curveAlienSize, curveAlienParamT));

	//std::cout << objects.swarm2->direction.x << '\t' << objects.swarm2->direction.y << std::endl;

	if (gameState.fogAutomatic == 1) {
		int sceneTime = (int)elapsedTime % 24;
		//std::cout << sceneTime << std::endl;
		if (sceneTime > 18 && sceneTime < 24) {
			gameState.fogEnable = 1;
		}
		else {
			gameState.fogEnable = 0;
		}
	}
	
}

/**
*	Moving with free camera.
*
*/
void move(int type) {
	glm::vec3 newPosition;
	float speed;
	float direction;

	if (objects.camera->sprint == 1) {
		speed = 0.05f;
	}
	else {
		speed = 0.01f;
	}

	if (type == 0) {
		newPosition = objects.camera->position + glm::vec3(speed, speed, speed) * objects.camera->direction;
	} else if (type == 1) {
		newPosition = objects.camera->position - glm::vec3(speed, speed, speed) * objects.camera->direction;
	} else if (type == 2) {
		direction = glm::radians(objects.camera->viewAngle - 90);
		newPosition = objects.camera->position + (glm::vec3(cos(direction), sin(direction), 0.0f) * speed);
	}
	else if (type == 3) {
		direction = glm::radians(objects.camera->viewAngle + 90);
		newPosition = objects.camera->position + (glm::vec3(cos(direction), sin(direction), 0.0f) * speed);
	}

	// collision with alien
	glm::vec2 distanceVecAlien(newPosition.x - objects.alien->position.x, newPosition.y - objects.alien->position.y);
	float distanceAlien = glm::length(distanceVecAlien);
	if (distanceAlien < objects.alien->collision) { //move closer
		if (objects.alien->collision >= 0.1f) objects.alien->collision = distanceAlien;
		else return;
	}

	// collision with swarm
	glm::vec2 distanceVecSwarm(newPosition.x - objects.swarm->position.x, newPosition.y - objects.swarm->position.y);
	float distanceSwarm = glm::length(distanceVecSwarm);
	if (distanceSwarm < objects.swarm->collision) { //move closer
		if (objects.swarm->collision >= 0.1f) objects.swarm->collision = distanceSwarm;
		else return;
	}

	// collision with swarm2
	glm::vec2 distanceVecSwarm2(newPosition.x - objects.swarm2->position.x, newPosition.y - objects.swarm2->position.y);
	float distanceSwarm2 = glm::length(distanceVecSwarm2);
	if (distanceSwarm2 < objects.swarm2->collision) { //move closer
		if (objects.swarm2->collision >= 0.2f) objects.swarm2->collision = distanceSwarm2;
		else return;
	}
	
	objects.camera->position = newPosition;

	//std::cout << objects.camera->position.x << '\t' << objects.camera->position.y << "\t" << objects.camera->viewAngle << std::endl;
	if (objects.camera->position.x < -AREA_SIZE_X) objects.camera->position.x = -AREA_SIZE_X;
	if (objects.camera->position.x > AREA_SIZE_X) objects.camera->position.x = AREA_SIZE_X;
	if (objects.camera->position.y < -AREA_SIZE_Y) objects.camera->position.y = -AREA_SIZE_Y;
	if (objects.camera->position.y > AREA_SIZE_Y) objects.camera->position.y = AREA_SIZE_Y;
}

/**
*	Callback for timer.
*
*/
void timerCallback(int) {
	// update scene time
	gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

	// free camera
	glm::vec3 newPosition;
	float timeDelta = gameState.elapsedTime - objects.camera->currentTime;
	objects.camera->currentTime = gameState.elapsedTime;
	if (gameState.freeCamera == true) {
		if (gameState.keyMap[KEY_UP_ARROW] == true) move(0);
		if (gameState.keyMap[KEY_DOWN_ARROW] == true) move(1);
		if (gameState.keyMap[KEY_RIGHT_ARROW] == true) move(2);
		if (gameState.keyMap[KEY_LEFT_ARROW] == true) move(3);
	}

	if (gameState.activeCamera == 2) {
		objects.camera->position = objects.alien->position;
		objects.camera->viewAngle = 150.0f;
		objects.camera->direction = objects.alien->direction;
		objects.camera->startTime = gameState.elapsedTime;
		objects.camera->currentTime = objects.camera->startTime;
		gameState.cameraElevationAngle = 10.0f;
	}

	// update objects in the scene
	updateObjects(gameState.elapsedTime);

	if (objects.ufo->direction == 0) {
		if (objects.ufo->time < 4.5f) {
			objects.ufo->time += 0.01f;
		}
		else {
			objects.ufo->direction = 1;
		}
	}
	else {
		if (objects.ufo->time > 0.5f) {
			objects.ufo->time -= 0.01f;
		}
		else {
			objects.ufo->direction = 0;
		}
	}
	//std::cout << objects.ufo->time << std::endl;


	glutTimerFunc(REFRESH_INTERVAL, timerCallback, 0);
	glutPostRedisplay();
}


/**
*	Listens to mouse motion.
*	changes state.cameraElevationAngle or turns camera to left/right.
*	\param[in] mouseX   New mouse X position.
*	\param[in] mouseY   New mouse Y position.
*/
void passiveMouseMotionCallback(int mouseX, int mouseY) {

	if (mouseY != gameState.windowHeight / 2) {
		float cameraElevationAngleDelta = 0.5f * (mouseY - gameState.windowHeight / 2);
		if (fabs(gameState.cameraElevationAngle + cameraElevationAngleDelta) < CAMERA_ELEVATION_MAX) {
			gameState.cameraElevationAngle += cameraElevationAngleDelta;

		}
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		glutPostRedisplay();
	}

	if (mouseX != gameState.windowWidth / 2) {
		float delta = 0.5f * (mouseX - gameState.windowWidth / 2);
		if ((mouseX - gameState.windowWidth / 2)<0)	turnCameraLeft(-delta);
			else turnCameraRight(delta);
		glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		glutPostRedisplay();
	}
}

/**
*	Callback called whenever a key on the keyboard is pressed.
*
*/
void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY) {

	int mod = glutGetModifiers();

	if (mod != 0) {
		switch (mod) {
			case GLUT_ACTIVE_SHIFT: {
				objects.camera->sprint = 1;
				switch (keyPressed) {
					case 'W':
						gameState.keyMap[KEY_UP_ARROW] = true;
						break; 
					case 'A':
						gameState.keyMap[KEY_LEFT_ARROW] = true;
						break;
					case 'S':
						gameState.keyMap[KEY_DOWN_ARROW] = true;
						break;
					case 'D':
						gameState.keyMap[KEY_RIGHT_ARROW] = true;
						break;
					default:
						break;
				}
				break;
			}
			default:
				break;
		}
	}
	else {
		objects.camera->sprint = 0;
		switch (keyPressed) {
		case 27:
			glutLeaveMainLoop();
			break;
		case 'f':
			if (gameState.activeCamera <= 2) {
				gameState.freeCamera = !gameState.freeCamera;
				if (gameState.freeCamera == true) {
					glutPassiveMotionFunc(passiveMouseMotionCallback);
					glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
				}
				else {
					gameState.cameraNeedsSetup = true;
					glutPassiveMotionFunc(NULL);
				}
			}
			break;
		case 'o':
			gameState.fogEnable = !gameState.fogEnable;
			gameState.fogAutomatic = 0;
			break;
		case 'l':
			gameState.flashlightEnable = !gameState.flashlightEnable;
			if (gameState.flashlightEnable) {
				gameState.flashlightIntensity = 0.8f;
			}
			else {
				gameState.flashlightIntensity = 0.0f;
			}
			std::cout << "flashlight " << gameState.flashlightIntensity << std::endl;
			break;
		case 'k':
			gameState.lampEnable = !gameState.lampEnable;
			std::cout << "lamp " << gameState.lampEnable << std::endl;
			break;
		case 'c':
			gameState.activeCamera++;
			gameState.activeCamera %= NUMBER_OF_CAMERA;
			gameState.cameraNeedsSetup = true;
			gameState.freeCamera = false;
			glutPassiveMotionFunc(NULL);
			break;
		case 'w':
			gameState.keyMap[KEY_UP_ARROW] = true;
			break;
		case 'a':
			gameState.keyMap[KEY_LEFT_ARROW] = true;
			break;
		case 's':
			gameState.keyMap[KEY_DOWN_ARROW] = true;
			break;
		case 'd':
			gameState.keyMap[KEY_RIGHT_ARROW] = true;
			break;
		case 'r':
			reloadScene();
			break;
		default:
			;
		}
	}

}

/**
*	When the key is released.
*
*/
void keyboardUpCallback(unsigned char keyPressed, int mouseX, int mouseY) {

	int mod = glutGetModifiers();

	if (mod != 0) {
		switch (mod) {
		case GLUT_ACTIVE_SHIFT: {
			objects.camera->sprint = 0;
			switch (keyPressed) {
			case 'W':
				gameState.keyMap[KEY_UP_ARROW] = false;
				break;
			case 'A':
				gameState.keyMap[KEY_LEFT_ARROW] = false;
				break;
			case 'S':
				gameState.keyMap[KEY_DOWN_ARROW] = false;
				break;
			case 'D':
				gameState.keyMap[KEY_RIGHT_ARROW] = false;
				break;
			default:
				break;
			}
			break;
		}
		default:
			break;
		}
	}
	else {
		switch (keyPressed) {
		case 'w':
			gameState.keyMap[KEY_UP_ARROW] = false;
			break;
		case 'a':
			gameState.keyMap[KEY_LEFT_ARROW] = false;
			break;
		case 's':
			gameState.keyMap[KEY_DOWN_ARROW] = false;
			break;
		case 'd':
			gameState.keyMap[KEY_RIGHT_ARROW] = false;
			break;
		default:
			;
		}
	}
}

/**
*	When the special key is pressed.
*
*/
void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY) {

	switch (specKeyPressed) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = true;
		break;
	case GLUT_KEY_F1:
		std::cout << "First camera" << std::endl;
		gameState.activeCamera = 0;
		gameState.cameraNeedsSetup = true;
		break;
	case GLUT_KEY_F2:
		std::cout << "Second camera" << std::endl;
		gameState.activeCamera = 1;
		gameState.cameraNeedsSetup = true;
		break;
	case GLUT_KEY_F3:
		std::cout << "Alien view" << std::endl;
		gameState.activeCamera = 2;
		break;
	case GLUT_KEY_F5:
		reloadScene();
		break;
	case GLUT_KEY_F12:
		std::cout << "Fog automatic" << std::endl;
		gameState.fogAutomatic = 1;
		break;
	default:
		;
	}
}

/**
*	When the special key is released.
*
*/
void specialKeyboardUpCallback(int specKeyPressed, int mouseX, int mouseY) {

	switch (specKeyPressed) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	default:
		; // printf("Unrecognized special key released\n");
	}
}

void mouseWheel(int button, int dir, int x, int y) {

	if (gameState.flashlightEnable == 1) {
		if (dir > 0) {
			if (gameState.flashlightIntensity < 2.0f ) gameState.flashlightIntensity += 0.1f;
		}
		else
		{
			if (gameState.flashlightIntensity > 0.1f) gameState.flashlightIntensity -= 0.1f;
		}
		std::cout << "flashlight " << gameState.flashlightIntensity << std::endl;
		glUseProgram(shaderProgram.program);
		glUniform1f(shaderProgram.flashlightIntensityLoc, gameState.flashlightIntensity);
		glUseProgram(0);
	}

	return;
}

/**
*	If mouse is clicked, reads from stencil buffer.
*
*/
void mouseCallback(int buttonPressed, int buttonState, int mouseX, int mouseY)
{
	if ((buttonPressed == GLUT_LEFT_BUTTON) && (buttonState == GLUT_DOWN)) {
		unsigned char objectID = 0;

		//std::cout << "klik leve mysi - " << "x: " << mouseX << " y: " << mouseY << std::endl;

		glReadPixels(mouseX, gameState.windowHeight - 1 - mouseY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &objectID);
		
		if (objectID == 0) { 		// background was clicked
			std::cout << "Clicked on background" << std::endl;
		} 
		else if (objectID == 88) {
			std::cout << "MEOW!!!" << std::endl;
			objects.cat->size = 0.0f;
		}
		else if (objectID == 99) {
			std::cout << "Clicked on lamp" << std::endl;
			gameState.lampEnable = !gameState.lampEnable;
		}
		else {				// object was clicked
			std::cout << "Clicked on object with ID: " << (int)objectID << std::endl;

			ObjectsList::iterator it = objects.boxes.begin();
			std::advance(it, objectID - 1); // now it points to the nth asteroids of the list (counting from zero)
			BoxObject* asteroid = (BoxObject*)(*it);

			if (asteroid->notDestroyed == true) {
				asteroid->notDestroyed = false;		   // remove asteroid
				insertExplosion(asteroid->position);   // insert explosion billboard
			}
		}

	}
}

/**
*	Defines events in the menu.
*
*/
void myMenu(int item)
{
	switch (item)
	{
	case 1:
		std::cout << "First camera" << std::endl;
		gameState.activeCamera = 0;
		gameState.cameraNeedsSetup = true;
		break;
	case 2:
		std::cout << "Second camera" << std::endl;
		gameState.activeCamera = 1;
		gameState.cameraNeedsSetup = true;
		break;
	case 3:
		std::cout << "Free camera" << std::endl;
		if (gameState.activeCamera < 3){
			gameState.freeCamera = !gameState.freeCamera;
			if (gameState.freeCamera == true) {
				glutPassiveMotionFunc(passiveMouseMotionCallback);
				glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			}
			else {
				gameState.cameraNeedsSetup = true;
				glutPassiveMotionFunc(NULL);
			}
		}
		break;
	case 4:
		std::cout << "Alien view" << std::endl;
		gameState.activeCamera = 2;
		gameState.cameraNeedsSetup = true;
		break;
	case 5:
		gameState.fogAutomatic = 0;
		gameState.fogEnable = 1;
		break;
	case 6:
		gameState.fogAutomatic = 0;
		gameState.fogEnable = 0;
		break;
	case 7:
		std::cout << "Fog automatic" << std::endl;
		gameState.fogAutomatic = 1;
		break;
	case 8:
		gameState.flashlightEnable = 1;
		gameState.flashlightIntensity = 0.8f;
		std::cout << "flashlight " << gameState.flashlightIntensity << std::endl;
		break;
	case 9:
		gameState.flashlightEnable = 0;
		gameState.flashlightIntensity = 0.0f;
		std::cout << "flashlight " << gameState.flashlightIntensity << std::endl;
		break;
	case 10:
		gameState.lampEnable = 1;
		std::cout << "lamp on" << std::endl;
		break;
	case 11:
		gameState.lampEnable = 0;
		std::cout << "lamp off" << std::endl;
		break;
	case 99:
		exit(0);
	}
	glutPostRedisplay();
}

/**
*	Initialize menu.
*
*/
void initMenu(void)
{
	int firstOptionMenuID = glutCreateMenu(myMenu);
	glutAddMenuEntry("First camera", 1);
	glutAddMenuEntry("Second camera", 2);
	glutAddMenuEntry("Free camera on/off", 3);
	glutAddMenuEntry("Alien view", 4);

	int secondOptionMenuID = glutCreateMenu(myMenu);
	glutAddMenuEntry("Fog on", 5);
	glutAddMenuEntry("Fog off", 6);
	glutAddMenuEntry("Fog automatic", 7);

	int thirdOptionMenuID = glutCreateMenu(myMenu);
	glutAddMenuEntry("Flashlight on", 8);
	glutAddMenuEntry("Flashlight off", 9);
	
	int fourthOptionMenuID = glutCreateMenu(myMenu);
	glutAddMenuEntry("Lamp on", 10);
	glutAddMenuEntry("Lamp off", 11);

	glutCreateMenu(myMenu);
	glutAddSubMenu("Camera", firstOptionMenuID);
	glutAddSubMenu("Fog", secondOptionMenuID);
	glutAddSubMenu("Flashlight", thirdOptionMenuID);
	glutAddSubMenu("Lamp", fourthOptionMenuID);
	glutAddMenuEntry("Quit", 99);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/**
*	Initialize the scene.
*
*/
void initializeApplication(void) {
	
	// initialize random seed
	srand((unsigned int)time(NULL));

	// initialize OpenGL
	glClearColor(0.5f, 0.4f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	
	glClearStencil(0);

	gameState.fogEnable = 0;
	gameState.flashlightIntensity = 0.0f;
	gameState.fogAutomatic = 0;
	gameState.flashlightEnable = 0;
	gameState.lampEnable = 0;

	// initialize shaders
	initializeShaderPrograms();
	// create geometry for all models used
	initializeModels();

	objects.camera = NULL;

	reloadScene();
}

/**
*	Finalize application.
*
*/
void finalizeApplication(void) {
	cleanUpObjects();
	delete objects.camera;
	objects.camera = NULL;

	deleteModels();
	deleteShaderPrograms();
}

int main(int argc, char** argv) {

	// initialize windowing system
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

	// initial window size
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(displayCallback);
	// register callback for change of window size
	glutReshapeFunc(reshapeCallback);
	
	// register callbacks for keyboard
	glutKeyboardFunc(keyboardCallback);
	glutKeyboardUpFunc(keyboardUpCallback);
	glutSpecialFunc(specialKeyboardCallback);     // key pressed
	glutSpecialUpFunc(specialKeyboardUpCallback); // key released
	
	// mouse
	glutMouseFunc(mouseCallback);
	glutMouseWheelFunc(mouseWheel);
	
	// timer
	glutTimerFunc(REFRESH_INTERVAL, timerCallback, 0);

	// openGL initialize
	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");

	//menu
	initMenu();

	// application
	initializeApplication();
	glutCloseFunc(finalizeApplication);
	glutMainLoop();

	return 0;
}
