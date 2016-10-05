//----------------------------------------------------------------------------------------
/**
* \file       objects.cpp
* \author     Jaroslav Hrach
* \date       2015
* \brief      Draws objects.
*
*/
//----------------------------------------------------------------------------------------


#include <iostream>
#include <stdlib.h>
#include "pgr.h"
#include "parameters.h"
#include "spline.h"
#include "objects.h"

///used shader programs
SCommonShaderProgram shaderProgram;
SSkyboxShaderProgram skyboxShaderProgram;
SExplosionShaderProgram explosionShaderProgram;
SUfoProgram ufoShaderProgram;

//objects
MeshGeometry* floorGeometry     = NULL; // 1
MeshGeometry* alienGeometry     = NULL; // 2
MeshGeometry* scannerGeometry   = NULL; // 3
MeshGeometry* cargoGeometry     = NULL; // 4
MeshGeometry* stopGeometry      = NULL; // 5
MeshGeometry* swarmGeometry     = NULL; // 6
MeshGeometry* catGeometry       = NULL; // 7
MeshGeometry* boxGeometry       = NULL; // 8
MeshGeometry* explosionGeometry = NULL; // 9
MeshGeometry* ufoGeometry       = NULL; // 10
MeshGeometry* lampGeometry      = NULL; // 11

//skybox
MeshGeometry* skyboxGeometry = NULL;
const char* SKYBOX_CUBE_TEXTURE_FILE_PREFIX = "data/skybox/";

// paths to objects
const char* ALIEN_MODEL_NAME = "data/alien/alien.obj";
const char* BOX_MODEL_NAME = "data/oil/oildrum.obj";
const char* STOP_MODEL_NAME = "data/stop/stop.obj";
const char* CAT_MODEL_NAME = "data/cat/cat.obj";
const char* SWARM_MODEL_NAME = "data/swarm/Swarm_Infector.obj";
const char* SCANNER_MODEL_NAME = "data/scanner/scanner.obj";
const char* LAMP_MODEL_NAME = "data/lamp/lamp.obj";
const char* CARGO_MODEL_NAME = "data/container/Cargo_container_01.obj";


// paths to textures
const char* FLOOR_TEXTURE_NAME = "data/floor/floor.jpg";
const char* EXPLOSION_TEXTURE_NAME = "data/explode/explode.png";
const char* UFO_TEXTURE_NAME = "data/ufo/ufo.png";

/**
*	Sets all shaders used in this program.
*/
void initializeShaderPrograms(void) {

	std::vector<GLuint> shaderList;

	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shaders/mainVertex.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shaders/mainFragment.frag"));

	// create the program with two shaders (fragment and vertex)
	shaderProgram.program = pgr::createProgram(shaderList);

	// get position and color attributes locations
	shaderProgram.posLocation = glGetAttribLocation(shaderProgram.program, "position");
	shaderProgram.colorLocation = glGetAttribLocation(shaderProgram.program, "color");
	shaderProgram.normalLocation = glGetAttribLocation(shaderProgram.program, "normal");
	shaderProgram.texCoordLocation = glGetAttribLocation(shaderProgram.program, "texCoord");
	shaderProgram.timeLocation = glGetUniformLocation(shaderProgram.program, "time");
	// material
	shaderProgram.ambientLocation = glGetUniformLocation(shaderProgram.program, "material.ambient");
	shaderProgram.diffuseLocation = glGetUniformLocation(shaderProgram.program, "material.diffuse");
	shaderProgram.specularLocation = glGetUniformLocation(shaderProgram.program, "material.specular");
	shaderProgram.shininessLocation = glGetUniformLocation(shaderProgram.program, "material.shininess");
	// texture
	shaderProgram.texSamplerLocation = glGetUniformLocation(shaderProgram.program, "texSampler");
	shaderProgram.useTextureLocation = glGetUniformLocation(shaderProgram.program, "material.useTexture");
	// matrix    
	shaderProgram.VmatrixLocation = glGetUniformLocation(shaderProgram.program, "Vmatrix");
	shaderProgram.PVMmatrixLocation = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
	shaderProgram.MmatrixLocation = glGetUniformLocation(shaderProgram.program, "Mmatrix");
	shaderProgram.normalMatrixLocation = glGetUniformLocation(shaderProgram.program, "normalMatrix");

	//fog
	shaderProgram.fogActiveLocation = glGetUniformLocation(shaderProgram.program, "fogActive");

	//lights
	shaderProgram.lampActiveLoc = glGetUniformLocation(shaderProgram.program, "lampActive");
	shaderProgram.lampPositionLoc = glGetUniformLocation(shaderProgram.program, "lampPosition");
	shaderProgram.lampIntensityLoc = glGetUniformLocation(shaderProgram.program, "lampIntensity");

	shaderProgram.flashlightPositionLoc = glGetUniformLocation(shaderProgram.program, "flashlightPosition");
	shaderProgram.flashlightDirectionLoc = glGetUniformLocation(shaderProgram.program, "flashlightDirection");
	shaderProgram.flashlightIntensityLoc = glGetUniformLocation(shaderProgram.program, "flashlightIntensity");


	// load and compile shader for explosions (dynamic texture)
	shaderList.clear();

	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shaders/explosionVertex.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shaders/explosionFragment.frag"));

	// create the program with two shaders
	explosionShaderProgram.program = pgr::createProgram(shaderList);

	// get position and texture coordinates attributes locations
	explosionShaderProgram.posLocation = glGetAttribLocation(explosionShaderProgram.program, "position");
	explosionShaderProgram.texCoordLocation = glGetAttribLocation(explosionShaderProgram.program, "texCoord");
	// get uniforms locations
	explosionShaderProgram.PVMmatrixLocation = glGetUniformLocation(explosionShaderProgram.program, "PVMmatrix");
	explosionShaderProgram.VmatrixLocation = glGetUniformLocation(explosionShaderProgram.program, "Vmatrix");
	explosionShaderProgram.timeLocation = glGetUniformLocation(explosionShaderProgram.program, "time");
	explosionShaderProgram.texSamplerLocation = glGetUniformLocation(explosionShaderProgram.program, "texSampler");
	explosionShaderProgram.frameDurationLocation = glGetUniformLocation(explosionShaderProgram.program, "frameDuration");

	//skybox -------------------------------------------------------------
	shaderList.clear();
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shaders/skyboxVertex.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shaders/skyboxFragment.frag"));
	skyboxShaderProgram.program = pgr::createProgram(shaderList);

	skyboxShaderProgram.posLocation = glGetAttribLocation(skyboxShaderProgram.program, "position");
	skyboxShaderProgram.skyboxSamplerLocation = glGetUniformLocation(skyboxShaderProgram.program, "skyboxSampler");
	skyboxShaderProgram.PVMmatrixLocation = glGetUniformLocation(skyboxShaderProgram.program, "PVM");
	skyboxShaderProgram.fogDensityLocation = glGetUniformLocation(skyboxShaderProgram.program, "fogDensity");
	skyboxShaderProgram.fogActiveLocation = glGetUniformLocation(skyboxShaderProgram.program, "fogActive");

	//ufo --------------------------------------------------------------
	shaderList.clear();

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "shaders/animatedVertex.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "shaders/animatedFragment.frag"));
	ufoShaderProgram.program = pgr::createProgram(shaderList);

	ufoShaderProgram.posLocation = glGetAttribLocation(ufoShaderProgram.program, "position");
	ufoShaderProgram.texCoordLocation = glGetAttribLocation(ufoShaderProgram.program, "texCoord");
	ufoShaderProgram.PVMmatrixLocation = glGetUniformLocation(ufoShaderProgram.program, "PVMmatrix");
	ufoShaderProgram.timeLocation = glGetUniformLocation(ufoShaderProgram.program, "time");
	ufoShaderProgram.texSamplerLocation = glGetUniformLocation(ufoShaderProgram.program, "texSampler");
}

/**
*	Sets (matrices, lights) uniforms for shaderProgram.
*	\param[in] modelMatrix
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void setTransformUniforms(const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix) {

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(shaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));	//value_ptr vraci pointer
	glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(viewMatrix * modelMatrix));
	glUniformMatrix4fv(shaderProgram.normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));  // correct matrix for non-rigid transform

}

/**
*	Sets (material and texture) uniforms for shaderProgram.
*	\param[in] ambient
*	\param[in] specular
*	\param[in] shininess
*	\param[in] texture
*/
void setMaterialUniforms(const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, float shininess, GLuint texture) {
	glUniform3fv(shaderProgram.diffuseLocation, 1, glm::value_ptr(diffuse));  // 2nd parameter must be 1 - it declares number of vectors in the vector array
	glUniform3fv(shaderProgram.ambientLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(shaderProgram.specularLocation, 1, glm::value_ptr(specular));
	glUniform1f(shaderProgram.shininessLocation, shininess);
	CHECK_GL_ERROR();
	if (texture != 0){
		glUniform1i(shaderProgram.useTextureLocation, 1);  // do texture sampling
		CHECK_GL_ERROR();
		glUniform1i(shaderProgram.texSamplerLocation, 0);  // texturing unit 0 -> samplerID   [for the GPU linker]
		CHECK_GL_ERROR();
		glActiveTexture(GL_TEXTURE0 + 0);                  // texturing unit 0 -> to be bound [for OpenGL BindTexture]
		CHECK_GL_ERROR();
		glBindTexture(GL_TEXTURE_2D, texture);
		CHECK_GL_ERROR();
	}
	else{
		glUniform1i(shaderProgram.useTextureLocation, 0);  // do not sample the texture
	}
}

/** Load mesh using assimp library
* \param filename [in] file to open/load
* \param shader [in] vao will connect loaded data to shader
* \param vbo [out] vertex and normal data |VVVVV...|NNNNN...| (no interleaving)
* \param eao [out] triangle indices
* \param vao [out] vao connects data to shader input
* \param numTriangles [out] how many triangles have been loaded and stored into index array eao
*/
bool loadSingleMesh(const std::string &fileName, SCommonShaderProgram& shader, MeshGeometry** geometry) {
	Assimp::Importer importer;

	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1); // Unitize object in size (scale the model to fit into (-1..1)^3)
	// Load asset from the file - you can play with various processing steps
	const aiScene * scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		*geometry = NULL;
		return false;
	}
	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example

	if (scn->mNumMeshes != 1) {
		std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
		*geometry = NULL;
		return false;
	}
	// in this phase we know we have one mesh in our loaded scene, we can directly copy its data to opengl ...
	const aiMesh * mesh = scn->mMeshes[0];

	*geometry = new MeshGeometry;

	// vertex buffer object, store all vertex positions and normals
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float)*mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
	// first store all vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float)*mesh->mNumVertices, mesh->mVertices);
	// then store all normals
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float)*mesh->mNumVertices, 3 * sizeof(float)*mesh->mNumVertices, mesh->mNormals);

	// just texture 0 for now
	float *textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
	float *currentTextureCoord = textureCoords;

	// copy texture coordinates
	aiVector3D vect;

	if (mesh->HasTextureCoords(0)) {
		// we use 2D textures with 2 coordinates and ignore the third coordinate
		for (unsigned int idx = 0; idx<mesh->mNumVertices; idx++) {
			vect = (mesh->mTextureCoords[0])[idx];
			*currentTextureCoord++ = vect.x;
			*currentTextureCoord++ = vect.y;
		}
	}

	// finally store all texture coordinates
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float)*mesh->mNumVertices, 2 * sizeof(float)*mesh->mNumVertices, textureCoords);

	// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
	unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}
	// copy our temporary index array to OpenGL and free the array
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

	delete[] indices;

	// copy the material info to MeshGeometry structure
	const aiMaterial *mat = scn->mMaterials[mesh->mMaterialIndex];
	aiColor3D color;
	aiString name;

	// Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
	mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!
	mat->Get<aiColor3D>(AI_MATKEY_COLOR_DIFFUSE, color);
	(*geometry)->diffuse = glm::vec3(color.r, color.g, color.b);
	mat->Get<aiColor3D>(AI_MATKEY_COLOR_AMBIENT, color);
	(*geometry)->ambient = glm::vec3(color.r, color.g, color.b);
	mat->Get<aiColor3D>(AI_MATKEY_COLOR_SPECULAR, color);
	(*geometry)->specular = glm::vec3(color.r, color.g, color.b);
	float shininess;

	mat->Get<float>(AI_MATKEY_SHININESS, shininess);
	(*geometry)->shininess = shininess / 4.0f;  // shininess divisor-not descibed anywhere

	(*geometry)->texture = 0;

	// load texture image
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		// get texture name 
		mat->Get<aiString>(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), name);
		std::string textureName = name.data;

		size_t found = fileName.find_last_of("/\\");
		// insert correct texture file path 
		if (found != std::string::npos) { // not found
			//subMesh_p->textureName.insert(0, "/");
			textureName.insert(0, fileName.substr(0, found + 1));
		}

		std::cout << "Loading texture file: " << textureName << std::endl;
		CHECK_GL_ERROR();
		(*geometry)->texture = pgr::createTexture(textureName);
	}
	CHECK_GL_ERROR();

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

	glEnableVertexAttribArray(shader.posLocation);
	glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shader.texCoordLocation);
	CHECK_GL_ERROR();
	glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
	CHECK_GL_ERROR();
	glEnableVertexAttribArray(shader.normalLocation);
	glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));
	CHECK_GL_ERROR();

	CHECK_GL_ERROR();
	glBindVertexArray(0);
	CHECK_GL_ERROR();
	(*geometry)->numTriangles = mesh->mNumFaces;
	CHECK_GL_ERROR();
	return true;
}

/**
*	Draws floor
*	\param[in] floor Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawFloor(FloorObject* floor, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix){

	glUseProgram(shaderProgram.program);
	
	glm::mat4 modelMatrix = alignObject(floor->position, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(floor->size));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
	setMaterialUniforms(
		floorGeometry->ambient,
		floorGeometry->diffuse,
		floorGeometry->specular,
		floorGeometry->shininess,
		floorGeometry->texture
	);
	
	glBindVertexArray(floorGeometry->vertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, 3 * floorGeometry->numTriangles);

	glBindVertexArray(0);
	glUseProgram(0);
}

/**
*	Draws alien
*	\param[in] alien Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawAlien(AlienObject* alien, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = alignObject(alien->position, alien->direction, glm::vec3(0.0f, 0.0f, 0.2f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(alien->size));
	modelMatrix = glm::rotate(modelMatrix, 0.0f, glm::vec3(1, 0, 0));
	CHECK_GL_ERROR();

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	setMaterialUniforms(
		alienGeometry->ambient,
		alienGeometry->diffuse,
		alienGeometry->specular,
		alienGeometry->shininess,
		alienGeometry->texture
		);


	CHECK_GL_ERROR();
	glBindVertexArray(alienGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, alienGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

/**
*	Draws Scanner
*	\param[in] Scanner Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawScanner(ScannerObject* scanner, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = alignObject(scanner->position, scanner->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scanner->size));
	CHECK_GL_ERROR();

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	setMaterialUniforms(
		scannerGeometry->ambient,
		scannerGeometry->diffuse,
		scannerGeometry->specular,
		scannerGeometry->shininess,
		scannerGeometry->texture
		);


	CHECK_GL_ERROR();
	glBindVertexArray(scannerGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, scannerGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

/**
*	Draws Cargo
*	\param[in] Cargo Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawCargo(CargoObject* cargo, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = alignObject(cargo->position, cargo->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(cargo->size));
	CHECK_GL_ERROR();

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	setMaterialUniforms(
		cargoGeometry->ambient,
		cargoGeometry->diffuse,
		cargoGeometry->specular,
		cargoGeometry->shininess,
		cargoGeometry->texture
		);


	CHECK_GL_ERROR();
	glBindVertexArray(cargoGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, cargoGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

/**
*	Draws Stop
*	\param[in] Stop Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawStop(StopObject* stop, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = alignObject(stop->position, stop->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(stop->size));
	CHECK_GL_ERROR();

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	setMaterialUniforms(
		stopGeometry->ambient,
		stopGeometry->diffuse,
		stopGeometry->specular,
		stopGeometry->shininess,
		stopGeometry->texture
		);


	CHECK_GL_ERROR();
	glBindVertexArray(stopGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, stopGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

/**
*	Draws Swarm
*	\param[in] Swarm Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawSwarm(SwarmObject* swarm, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = alignObject(swarm->position, swarm->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(swarm->size));
	CHECK_GL_ERROR();

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	setMaterialUniforms(
		swarmGeometry->ambient,
		swarmGeometry->diffuse,
		swarmGeometry->specular,
		swarmGeometry->shininess,
		swarmGeometry->texture
		);


	CHECK_GL_ERROR();
	glBindVertexArray(swarmGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, swarmGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

/**
*	Draws Cat
*	\param[in] Cat Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawCat(CatObject* cat, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = alignObject(cat->position, cat->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(cat->size));
	CHECK_GL_ERROR();

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	setMaterialUniforms(
		catGeometry->ambient,
		catGeometry->diffuse,
		catGeometry->specular,
		catGeometry->shininess,
		catGeometry->texture
		);


	CHECK_GL_ERROR();
	glBindVertexArray(catGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, catGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

/**
*	Draws Box
*	\param[in] Box Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawBox(BoxObject* box, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), box->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(box->size));
	modelMatrix = glm::rotate(modelMatrix, 90.0f, glm::vec3(1, 0, 0));

	// setting matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
	setMaterialUniforms(
		boxGeometry->ambient,
		boxGeometry->diffuse,
		boxGeometry->specular,
		boxGeometry->shininess,
		boxGeometry->texture
		);

	glBindVertexArray(boxGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, boxGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

/**
*	Draws lamp
*	\param[in] lamp Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawLamp(LampObject* lamp, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {
	
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), lamp->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(lamp->size));
	modelMatrix = glm::rotate(modelMatrix, 90.0f, glm::vec3(1, 0, 0));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
	setMaterialUniforms(
		lampGeometry->ambient,
		lampGeometry->diffuse,
		lampGeometry->specular,
		lampGeometry->shininess,
		lampGeometry->texture
		);

	glUniform1i(shaderProgram.texSamplerLocation, 0);
	
	glBindVertexArray(lampGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, lampGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

/**
*	Draws Explosion
*	\param[in] Explosion Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawExplosion(ExplosionObject* explosion, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(explosionShaderProgram.program);

	// just take rotation part of the view transform
	glm::mat4 billboardRotationMatrix = glm::mat4(
		viewMatrix[0],
		viewMatrix[1],
		viewMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	// inverse view rotation
	billboardRotationMatrix = glm::transpose(billboardRotationMatrix);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), explosion->position);
	matrix = glm::scale(matrix, glm::vec3(explosion->size));
	matrix = matrix*billboardRotationMatrix; // make billboard to face the camera

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
	glUniformMatrix4fv(explosionShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));  // model-view-projection
	glUniformMatrix4fv(explosionShaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));   // view
	glUniform1f(explosionShaderProgram.timeLocation, explosion->currentTime - explosion->startTime);
	glUniform1i(explosionShaderProgram.texSamplerLocation, 0);
	glUniform1f(explosionShaderProgram.frameDurationLocation, explosion->frameDuration);

	glBindVertexArray(explosionGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_2D, explosionGeometry->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, explosionGeometry->numTriangles);

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_BLEND);
}

/**
*	Draws Ufo
*	\param[in] Explosion Object to draw
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawUfo(UfoObject* ufo, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(ufoShaderProgram.program);

	// just take rotation part of the view transform
	glm::mat4 billboardRotationMatrix = glm::mat4(
		viewMatrix[0],
		viewMatrix[1],
		viewMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	// inverse view rotation
	billboardRotationMatrix = glm::transpose(billboardRotationMatrix);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), ufo->position);
	matrix = glm::scale(matrix, glm::vec3(ufo->size));
	matrix = matrix*billboardRotationMatrix;

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
	glUniformMatrix4fv(ufoShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));  // model-view-projection
	glUniform1f(ufoShaderProgram.timeLocation, (*ufo).time);
	glUniform1i(ufoShaderProgram.texSamplerLocation, 0);

	glBindVertexArray(ufoGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_2D, ufoGeometry->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, ufoGeometry->numTriangles);

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_BLEND);
}

/**
*	Draws skybox
*	\param[in] viewMatrix
*	\param[in] projectionMatrix
*/
void drawSkybox(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) {

	glUseProgram(skyboxShaderProgram.program);

	glm::mat4 modelMatrix = alignObject(glm::vec3(0.0f, 0.0f, 0.05f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::rotate(modelMatrix, -90.f, glm::vec3(1, 0, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(2.2f));

	glm::mat4 viewWithoutTranslation = viewMatrix;
	viewWithoutTranslation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 PVM = projectionMatrix * viewWithoutTranslation *modelMatrix;
	//glUniformMatrix4fv(skyboxShaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	//glUniformMatrix4fv(skyboxShaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(skyboxShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniform1i(skyboxShaderProgram.skyboxSamplerLocation, 0);
	CHECK_GL_ERROR();

	
	CHECK_GL_ERROR();

	glBindVertexArray(skyboxGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxGeometry->texture);
	glDrawElements(GL_TRIANGLES, skyboxGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);
	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);
}

/**
*	Initializes floor geometry.
*	\param[in] shader	Used shader program.
*	\param[in] geometry Geometry object for a floor.
*/
void initFloorGeometry(SCommonShaderProgram &shader, MeshGeometry **geometry) {

	*geometry = new MeshGeometry;
	(*geometry)->texture = pgr::createTexture(FLOOR_TEXTURE_NAME);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));		//VAO
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));		//VBO
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Floor), Floor, GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader.posLocation);
	glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);

	glEnableVertexAttribArray(shader.normalLocation);
	glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	CHECK_GL_ERROR();
	
	glBindTexture(GL_TEXTURE_2D, (*geometry)->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glEnableVertexAttribArray(shader.texCoordLocation);
	glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));


	(*geometry)->ambient = glm::vec3(0.6f, 0.6f, 0.6f);
	(*geometry)->diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
	(*geometry)->specular = glm::vec3(0.02f, 0.02f, 0.02f);
	(*geometry)->shininess = 0.9f;

	glBindVertexArray(0);
	(*geometry)->numTriangles = FLOOR_TRIANGLES;
}

/**
*	Initializes explosion geometry.
*	\param[in] shader	Used shader program.
*	\param[in] geometry Geometry object for a floor.
*/
void initExplosionGeometry(GLuint shader, MeshGeometry **geometry) {

	*geometry = new MeshGeometry;
	(*geometry)->texture = pgr::createTexture(EXPLOSION_TEXTURE_NAME);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(explosionVertexData), explosionVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ufoShaderProgram.posLocation);
	// vertices of triangles - start at the beginning of the array (interlaced array)
	glVertexAttribPointer(explosionShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(explosionShaderProgram.texCoordLocation);
	// texture coordinates are placed just after the position of each vertex (interlaced array)
	glVertexAttribPointer(explosionShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = explosionNumQuadVertices;
}

/**
*	Initializes ufo geometry.
*	\param[in] shader	Used shader program.
*	\param[in] geometry Geometry object for a floor.
*/
void initUfoGeometry(GLuint shader, MeshGeometry **geometry) {

	*geometry = new MeshGeometry;
	(*geometry)->texture = pgr::createTexture(UFO_TEXTURE_NAME);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ufoVertexData), ufoVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ufoShaderProgram.posLocation);
	glVertexAttribPointer(ufoShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glBindTexture(GL_TEXTURE_2D, (*geometry)->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

	glEnableVertexAttribArray(ufoShaderProgram.texCoordLocation);
	// texture coordinates are placed just after the position of each vertex (interlaced array)
	glVertexAttribPointer(ufoShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = ufoNumQuadVertices;
}

/**
*	Initializes skybox geometry.
*	\param[in] shader	Used shader program.
*	\param[in] geometry Geometry object for a skybox.
*/
void initSkyboxGeometry(GLuint shader, MeshGeometry **geometry) {
	*geometry = new MeshGeometry;

	const float skycubeVertices[] = {
		-1.0, 1.0, 1.0,	//0
		-1.0, -1.0, 1.0,//1
		1.0, -1.0, 1.0,//2
		1.0, 1.0, 1.0,//3
		-1.0, 1.0, -1.0,//4
		-1.0, -1.0, -1.0,//5
		1.0, -1.0, -1.0,//6
		1.0, 1.0, -1.0,//7
	};

	const unsigned skycubeIndices[] = {
		0, 1, 2,
		2, 3, 0,
		3, 2, 6,
		6, 7, 3,
		7, 6, 5,
		5, 4, 7,

		4, 5, 1,
		1, 0, 4,
		0, 3, 7,
		7, 4, 0,
		1, 2, 6,
		6, 5, 1
	};

	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &((*geometry)->texture));
	glBindTexture(GL_TEXTURE_CUBE_MAP, (*geometry)->texture);

	const char * suffixes[] = { "bk", "ft", "lf", "rt", "up", "dn" };
	GLuint targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for (int i = 0; i < 6; i++) {
		std::string texName = std::string(SKYBOX_CUBE_TEXTURE_FILE_PREFIX) + "desertsky_" + suffixes[i] + ".jpg";
		std::cout << "Loading cube map texture: " << texName << std::endl;
		if (!pgr::loadTexImage2D(texName, targets[i])) {
			pgr::dieWithError("Skybox cube map loading failed!");
		}
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	CHECK_GL_ERROR();

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));		//VAO
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));		//VBO
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skycubeVertices), skycubeVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &((*geometry)->elementBufferObject));		//EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skycubeIndices), skycubeIndices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(skyboxShaderProgram.posLocation);
	glVertexAttribPointer(skyboxShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	CHECK_GL_ERROR();
	skyboxGeometry->numTriangles = 12;

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

/**
*	Initialize vertex buffers and vertex arrays for all objects.
*/
void initializeModels() {

	// load alien model from external file
	if (loadSingleMesh(ALIEN_MODEL_NAME, shaderProgram, &alienGeometry) != true) {
		std::cerr << "initModels(): Alien model loading failed." << std::endl;
	}

	// load scanner model from external file
	if (loadSingleMesh(SCANNER_MODEL_NAME, shaderProgram, &scannerGeometry) != true) {
		std::cerr << "initModels(): Scanner model loading failed." << std::endl;
	}

	// load cargo model from external file
	if (loadSingleMesh(CARGO_MODEL_NAME, shaderProgram, &cargoGeometry) != true) {
		std::cerr << "initModels(): Cargo model loading failed." << std::endl;
	}

	// load stop model from external file
	if (loadSingleMesh(STOP_MODEL_NAME, shaderProgram, &stopGeometry) != true) {
		std::cerr << "initModels(): Stop model loading failed." << std::endl;
	}

	// load swarm model from external file
	if (loadSingleMesh(SWARM_MODEL_NAME, shaderProgram, &swarmGeometry) != true) {
		std::cerr << "initModels(): Swarm model loading failed." << std::endl;
	}

	// load cat model from external file
	if (loadSingleMesh(CAT_MODEL_NAME, shaderProgram, &catGeometry) != true) {
		std::cerr << "initModels(): Cat model loading failed." << std::endl;
	}

	// load box model from external file
	if (loadSingleMesh(BOX_MODEL_NAME, shaderProgram, &boxGeometry) != true) {
		std::cerr << "initializeModels(): Box model loading failed." << std::endl;
	}

	// load box model from external file
	if (loadSingleMesh(LAMP_MODEL_NAME, shaderProgram, &lampGeometry) != true) {
		std::cerr << "initializeModels(): Lamp model loading failed." << std::endl;
	}

	// load shaders
	initFloorGeometry(shaderProgram, &floorGeometry);
	initSkyboxGeometry(skyboxShaderProgram.program, &skyboxGeometry);
	initExplosionGeometry(explosionShaderProgram.program, &explosionGeometry);
	initUfoGeometry(ufoShaderProgram.program, &ufoGeometry);
	

	CHECK_GL_ERROR();
}

/**
*	Deletes shader programs.
*/
void deleteShaderPrograms(void) {
	pgr::deleteProgramAndShaders(shaderProgram.program);
	pgr::deleteProgramAndShaders(skyboxShaderProgram.program);
	pgr::deleteProgramAndShaders(explosionShaderProgram.program);
	pgr::deleteProgramAndShaders(ufoShaderProgram.program);
}

/**
*	Deletes buffers.
*	\param[in] geometry Geometry to be delete
*/
void deleteGeometry(MeshGeometry *geometry) {
	
	glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
	glDeleteBuffers(1, &(geometry->elementBufferObject));
	glDeleteBuffers(1, &(geometry->vertexBufferObject));

	if (geometry->texture != 0) glDeleteTextures(1, &(geometry->texture));
}

/**
Delete all geometries.
*/
void deleteModels() {
	deleteGeometry(floorGeometry);
	deleteGeometry(alienGeometry);
	deleteGeometry(scannerGeometry);
	deleteGeometry(cargoGeometry);
	deleteGeometry(stopGeometry);
	deleteGeometry(swarmGeometry);
	deleteGeometry(catGeometry);
	deleteGeometry(boxGeometry);
	deleteGeometry(explosionGeometry);
	deleteGeometry(ufoGeometry);
	deleteGeometry(lampGeometry);
	deleteGeometry(skyboxGeometry);
}
