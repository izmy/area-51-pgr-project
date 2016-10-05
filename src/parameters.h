//----------------------------------------------------------------------------------------
/**
* \file       parameters.h
* \author     Jaroslav Hrach
* \date       2015
* \brief      Constant values for application.
*
*  Constant values, simple shaders and vertices for some objects.
*
*/
//----------------------------------------------------------------------------------------

#include <string>

#ifndef __PARAMETERS_H
#define __PARAMETERS_H

#define WINDOW_WIDTH   1200
#define WINDOW_HEIGHT  800
#define WINDOW_TITLE   "Area 51"

#define SCENE_WIDTH  1.0f
#define SCENE_HEIGHT 1.0f
#define SCENE_DEPTH  1.0f

#define REFRESH_INTERVAL 33
#define AREA_SIZE_X 2.0f
#define AREA_SIZE_Y 2.0f

// keys used in the key map
enum { KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_SPACE, KEYS_COUNT };

// camera
#define NUMBER_OF_CAMERA 3
#define VIEW_ANGLE_DELTA 50.0f 
#define CAMERA_ELEVATION_MAX 50.0f 
#define CAMERA_MOVEMENT_SPEED 0.4f
#define CAMERA_SIZE 0.05f

// objects
#define BOXES_NUMBER 10
#define BOX_SIZE 0.06f
#define LAMP_SIZE 0.15f
#define ALIEN_SIZE 0.08f
#define SCANNER_SIZE 0.05f
#define CARGO_SIZE 0.35f
#define CAT_SIZE 0.05f
#define STOP_SIZE 0.15f
#define SWARM_SIZE 0.08f

// floor
#define FLOOR_TRIANGLES 2
#define FLOOR_SIZE 4.0f


const float Floor[] = {
	// x   y     z     r     g     b 
	1.0f, 0.0f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 35.0f, 35.0f,
	-1.0f, 0.0f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 35.0f,
	1.0f, 0.0f, -1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 35.0f, 0.0f,

	1.0f, 0.0f, -1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 35.0f, 0.0f,
	-1.0f, 0.0f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 35.0f,
	-1.0f, 0.0f, -1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
};

const int explosionNumQuadVertices = 4;
const float explosionVertexData[explosionNumQuadVertices * 5] = {

	// x      y     z     u     v
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
};

const int ufoNumQuadVertices = 4;
const float ufoVertexData[ufoNumQuadVertices * 5] = {

	// x      y     z     u     v
	-3.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	-3.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	3.0f, 1.0f, 0.0f, 6.0f, 1.0f,
	3.0f, 0.0f, 0.0f, 6.0f, 0.0f
};

#endif 