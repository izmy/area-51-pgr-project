//----------------------------------------------------------------------------------------
/**
* \file       objects.h
* \author     Jaroslav Hrach
* \date       2015
* \brief      Data for objects.cpp
*
*	Defines objects used in this application.
* 
*/
//----------------------------------------------------------------------------------------

#ifndef __OBJECTS_H
#define __OBJECTS_H

#include "pgr.h"
#include <string>

/**
*	struct for a mesh geometry
*
*/
typedef struct MeshGeometry {
	GLuint        vertexBufferObject;   // identifier for the vertex buffer object
	GLuint        elementBufferObject;  // identifier for the element buffer object
	GLuint        vertexArrayObject;    // identifier for the vertex array object
	unsigned int  numTriangles;         // number of triangles in the mesh

	// material
	glm::vec3     ambient;
	glm::vec3     diffuse;
	glm::vec3     specular;
	float         shininess;
	GLuint        texture;
} MeshGeometry;

/**
*	struct for a camera
*
*/
typedef struct CameraObject {
	glm::vec3 position;
	glm::vec3 direction;
	float startTime;
	float currentTime;
	float viewAngle;
	float size;
	bool sprint;
} CameraObject;

/**
*	struct for a floor
*
*/
typedef struct FloorObject {
	glm::vec3 position;
	float     size;
	float	   radius;
} FloorObject;

/**
*	struct for a alien
*
*/
typedef struct AlienObject {
	glm::vec3 position;
	glm::vec3 direction;
	float     size;
	float     collision;
} AlienObject;

/**
*	struct for a scanner
*
*/
typedef struct ScannerObject {
	glm::vec3	position;
	glm::vec3	initPosition;
	glm::vec3	direction;
	float		size;
	float		radius;
	float		startTime;

} ScannerObject;

/**
*	struct for a cargo
*
*/
typedef struct CargoObject {
	glm::vec3 position;
	glm::vec3 direction;
	float     size;
} CargoObject;

/**
*	struct for a stop
*
*/
typedef struct StopObject {
	glm::vec3 position;
	glm::vec3 direction;
	float     size;
} StopObject;

/**
*	struct for a swarm
*
*/
typedef struct SwarmObject {
	glm::vec3 position;
	glm::vec3 direction;
	float     size;
	float     collision;
} SwarmObject;

/**
*	struct for a cat
*
*/
typedef struct CatObject {
	glm::vec3 position;
	glm::vec3 direction;
	float     size;
} CatObject;

/**
*	struct for a box
*
*/
typedef struct boxObject {
	glm::vec3 position;
	glm::vec3 direction;
	float     speed;
	float     size;

	bool notDestroyed;

	float startTime;
	float currentTime;

	float rotationSpeed;

} BoxObject;

/**
*	struct for a lamp
*
*/
typedef struct LampObject {
	glm::vec3 position;
	float     size;
	float	   radius;
} LampObject;

/**
*	struct for a explosion
*
*/
typedef struct ExplosionObject {
	glm::vec3 position;
	glm::vec3 direction;
	float     speed;
	float     size;

	bool notDestroyed;

	float startTime;
	float currentTime;

	int    textureFrames;
	float  frameDuration;

} ExplosionObject;

/**
*	struct for a ufo
*
*/
typedef struct UfoObject
{
	glm::vec3 position;
	float size;
	float rotationAngle;
	float time; // time used for moving ufo texture
	int direction;

} UfoObject;

typedef struct _commonShaderProgram {

	GLuint program;
	GLint posLocation;
	GLint colorLocation;
	GLint normalLocation;
	GLint texCoordLocation;

	GLint PVMmatrixLocation;
	GLint VmatrixLocation;      //   view/camera matrix
	GLint MmatrixLocation;      //  modeling matrix
	GLint normalMatrixLocation; //  inverse transposed VMmatrix

	GLint timeLocation;         //  elapsed time in seconds

	// material 
	GLint diffuseLocation;
	GLint ambientLocation;
	GLint specularLocation;
	GLint shininessLocation;
	
	// texture
	GLint useTextureLocation;
	GLint texSamplerLocation;

	//fog
	GLint fogActiveLocation;

	//lights
	GLint lampActiveLoc;
	GLint lampPositionLoc;
	GLint lampIntensityLoc;

	GLint flashlightIntensityLoc;
	GLint flashlightPositionLoc;
	GLint flashlightDirectionLoc;
} SCommonShaderProgram;

/**
*	struct for a skybox shader program
*
*/
typedef struct SSkyboxShaderProgram {
	GLuint program;
	GLint PVMmatrixLocation;
	GLint skyboxSamplerLocation;
	GLint fogDensityLocation;
	GLint posLocation;
	GLint fogActiveLocation;
} SSkyboxShaderProgram;

typedef struct SExplosionShaderProgram {
	// identifier for the program
	GLuint program;              // = 0;
	// vertex attributes locations
	GLint posLocation;           // = -1;
	GLint texCoordLocation;      // = -1;
	// uniforms locations
	GLint PVMmatrixLocation;     // = -1;
	GLint VmatrixLocation;       // = -1;
	GLint timeLocation;          // = -1;
	GLint texSamplerLocation;    // = -1;
	GLint frameDurationLocation; // = -1;

} SExplosionShaderProgram;

/**
*	struct for a ufo shader program
*
*/
typedef struct SUfoProgram {
	GLuint program;
	GLint posLocation;
	GLint texCoordLocation;
	GLint PVMmatrixLocation;
	GLint timeLocation;
	GLint texSamplerLocation;
} SUfoProgram;

//drawing objects
void drawFloor(FloorObject* floor, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawAlien(AlienObject* alien, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawScanner(ScannerObject* scanner, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawCargo(CargoObject* cargo, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawStop(StopObject* stop, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawSwarm(SwarmObject* swarm, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawCat(CatObject* cat, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawBox(BoxObject* box, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawLamp(LampObject* lamp, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawExplosion(ExplosionObject* explosion, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawUfo(UfoObject* ufo, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawSkybox(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);


//shaders
void initializeShaderPrograms();
void deleteShaderPrograms();

//models
void initializeModels();
void deleteModels();

#endif 
