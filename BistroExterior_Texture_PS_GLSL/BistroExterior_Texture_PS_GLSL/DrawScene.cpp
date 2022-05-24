//
//  DrawScene.cpp
//
//  Written for CSE4170
//  Department of Computer Science and Engineering
//  Copyright © 2022 Sogang University. All rights reserved.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "LoadScene.h"

// Begin of shader setup
#include "Shaders/LoadShaders.h"
#include "ShadingInfo.h"

extern SCENE scene;

// for simple shaders
GLuint h_ShaderProgram_simple, h_ShaderProgram_TXPS; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// for Phong Shading (Textured) shaders
#define NUMBER_OF_LIGHT_SUPPORTED 4 
GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_TXPS, loc_ModelViewMatrix_TXPS, loc_ModelViewMatrixInvTrans_TXPS;
GLint loc_texture;
GLint loc_flag_texture_mapping;
GLint loc_flag_fog;

// include glm/*.hpp only if necessary
// #include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> 
// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;
// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix
glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when ready.
glm::mat4 ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

/*********************************  START: camera *********************************/
typedef enum {
	CAMERA_0,
	CAMERA_1,
	CAMERA_2,
	CAMERA_3,
	CAMERA_m,
	CAMERA_t,
	CAMERA_g,
	NUM_CAMERAS
} CAMERA_INDEX;

typedef struct _Camera {
	float pos[3];
	float uaxis[3], vaxis[3], naxis[3];
	float fovy, aspect_ratio, near_c, far_c;
	int move, rotation_axis;
} Camera;

Camera camera_info[NUM_CAMERAS];
Camera current_camera;
int camera_mod;

using glm::mat4;

//ViewMatrix 설정
void set_ViewMatrix_from_camera_frame(void) {
	ViewMatrix = glm::mat4(current_camera.uaxis[0], current_camera.vaxis[0], current_camera.naxis[0], 0.0f,
		current_camera.uaxis[1], current_camera.vaxis[1], current_camera.naxis[1], 0.0f,
		current_camera.uaxis[2], current_camera.vaxis[2], current_camera.naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-current_camera.pos[0], -current_camera.pos[1], -current_camera.pos[2]));
}

void set_current_camera(int camera_num) {
	Camera* pCamera = &camera_info[camera_num];

	camera_mod = camera_num;
	memcpy(&current_camera, pCamera, sizeof(Camera));
	set_ViewMatrix_from_camera_frame();
	ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

//카메라 0, 1, 2, 3
void initialize_camera(void) {
	Camera* pCamera;

	//CAMERA_0 : tank view
	pCamera = &camera_info[CAMERA_0];
	pCamera->pos[0] = 1698.044189; pCamera->pos[1] = 4226.301758; pCamera->pos[2] = 1073.223999;
	pCamera->uaxis[0] = -0.947783; pCamera->uaxis[1] = 0.318916; pCamera->uaxis[2] = 0.000000;
	pCamera->vaxis[0] = -0.082445; pCamera->vaxis[1] = -0.245019; pCamera->vaxis[2] = 0.966009;
	pCamera->naxis[0] = 0.308075; pCamera->naxis[1] = 0.915569; pCamera->naxis[2] = 0.258517;
	pCamera->move = 0;
	pCamera->fovy = 0.698132, pCamera->aspect_ratio = 1.777778, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	//CAMERA_1 : ironman view
	pCamera = &camera_info[CAMERA_1];
	pCamera->pos[0] = -975.329895; pCamera->pos[1] = -304.546387; pCamera->pos[2] = 309.933868;
	pCamera->uaxis[0] = -0.446822; pCamera->uaxis[1] = -0.893904; pCamera->uaxis[2] = 0.035884;
	pCamera->vaxis[0] = 0.024694; pCamera->vaxis[1] = 0.027772; pCamera->vaxis[2] = 0.999309;
	pCamera->naxis[0] = -0.894283; pCamera->naxis[1] = 0.447400; pCamera->naxis[2] = 0.009665;
	pCamera->move = 0;
	pCamera->fovy = 0.698132, pCamera->aspect_ratio = 1.777778, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	//CAMERA_2 : bike view
	pCamera = &camera_info[CAMERA_2];
	pCamera->pos[0] = -1623.476318; pCamera->pos[1] = 692.174133; pCamera->pos[2] = 1053.776855;
	pCamera->uaxis[0] = 0.750404; pCamera->uaxis[1] = 0.660965; pCamera->uaxis[2] = -0.005153;
	pCamera->vaxis[0] = -0.311077; pCamera->vaxis[1] = 0.360028; pCamera->vaxis[2] = 0.879542;
	pCamera->naxis[0] = 0.583203; pCamera->naxis[1] = -0.658410; pCamera->naxis[2] = 0.475780;
	pCamera->move = 0;
	pCamera->fovy = 0.698132, pCamera->aspect_ratio = 1.777778, pCamera->near_c = 0.100000; pCamera->far_c = 30000.0f;

	//CAMERA_3 : tree view
	pCamera = &camera_info[CAMERA_3];
	pCamera->pos[0] = 4010.615479; pCamera->pos[1] = -666.143188; pCamera->pos[2] = 978.349915;
	pCamera->uaxis[0] = -0.957236; pCamera->uaxis[1] = -0.287192; pCamera->uaxis[2] = 0.034577;
	pCamera->vaxis[0] = 0.140261; pCamera->vaxis[1] = -0.356284; pCamera->vaxis[2] = 0.923766;
	pCamera->naxis[0] = -0.252989; pCamera->naxis[1] = 0.889130; pCamera->naxis[2] = 0.381342;
	pCamera->move = 0;
	pCamera->fovy = 0.698132, pCamera->aspect_ratio = 1.777778, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;

	//CAMERA_m : move_camera
	pCamera = &camera_info[CAMERA_m];
	pCamera->pos[0] = -1310.289185; pCamera->pos[1] = -236.713699; pCamera->pos[2] = 316.325989;
	pCamera->uaxis[0] = 0.607906; pCamera->uaxis[1] = -0.793984; pCamera->uaxis[2] = 0.007044;
	pCamera->vaxis[0] = 0.024694; pCamera->vaxis[1] = 0.027772; pCamera->vaxis[2] = 0.999309;
	pCamera->naxis[0] = -0.793634; pCamera->naxis[1] = -0.607310; pCamera->naxis[2] = 0.036489;
	pCamera->move = 0;
	pCamera->fovy = 0.698132, pCamera->aspect_ratio = 1.777778, pCamera->near_c = 0.1f; pCamera->far_c = 30000.0f;
	set_current_camera(CAMERA_m);
}
/*********************************  END: camera *********************************/

/******************************  START: shader setup ****************************/
// Begin of Callback function definitions
void prepare_shader_program(void) {
	char string[256];

	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	ShaderInfo shader_info_TXPS[3] = {
	{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
	{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram_simple);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");

	for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");
	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");
}
/*******************************  END: shder setup ******************************/

/****************************  START: geometry setup ****************************/
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define INDEX_VERTEX_POSITION	0
#define INDEX_NORMAL			1
#define INDEX_TEX_COORD			2

bool b_draw_grid = false;

//axes
GLuint axes_VBO, axes_VAO;
GLfloat axes_vertices[6][3] = {
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };

void prepare_axes(void) {
	// Initialize vertex buffer object.
	glGenBuffers(1, &axes_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), &axes_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	fprintf(stdout, " * Loaded axes into graphics memory.\n");
}

void draw_axes(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(8000.0f, 8000.0f, 8000.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	glBindVertexArray(axes_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

//grid
#define GRID_LENGTH			(100)
#define NUM_GRID_VETICES	((2 * GRID_LENGTH + 1) * 4)
GLuint grid_VBO, grid_VAO;
GLfloat grid_vertices[NUM_GRID_VETICES][3];
GLfloat grid_color[3] = { 0.5f, 0.5f, 0.5f };

void prepare_grid(void) {

	//set grid vertices
	int vertex_idx = 0;
	for (int x_idx = -GRID_LENGTH; x_idx <= GRID_LENGTH; x_idx++)
	{
		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = -GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = x_idx;
		grid_vertices[vertex_idx][1] = GRID_LENGTH;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	for (int y_idx = -GRID_LENGTH; y_idx <= GRID_LENGTH; y_idx++)
	{
		grid_vertices[vertex_idx][0] = -GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;

		grid_vertices[vertex_idx][0] = GRID_LENGTH;
		grid_vertices[vertex_idx][1] = y_idx;
		grid_vertices[vertex_idx][2] = 0.0f;
		vertex_idx++;
	}

	// Initialize vertex buffer object.
	glGenBuffers(1, &grid_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices), &grid_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &grid_VAO);
	glBindVertexArray(grid_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, grid_VAO);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	fprintf(stdout, " * Loaded grid into graphics memory.\n");
}

void draw_grid(void) {
	if (!b_draw_grid)
		return;

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(1.0f);
	glBindVertexArray(grid_VAO);
	glUniform3fv(loc_primitive_color, 1, grid_color);
	glDrawArrays(GL_LINES, 0, NUM_GRID_VETICES);
	glBindVertexArray(0);
	glLineWidth(1.0f);
	glUseProgram(0);
}

//bistro_exterior
GLuint* bistro_exterior_VBO;
GLuint* bistro_exterior_VAO;
int* bistro_exterior_n_triangles;
int* bistro_exterior_vertex_offset;
GLfloat** bistro_exterior_vertices;
GLuint* bistro_exterior_texture_names;

int flag_fog;
bool* flag_texture_mapping;

void initialize_lights(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 1.0f, 1.0f, 1.0f, 1.0f);

	for (i = 0; i < scene.n_lights; i++) {
		glUniform1i(loc_light[i].light_on, 1);
		glUniform4f(loc_light[i].position,
			scene.light_list[i].pos[0],
			scene.light_list[i].pos[1],
			scene.light_list[i].pos[2],
			0.0f);

		glUniform4f(loc_light[i].ambient_color, 0.13f, 0.13f, 0.13f, 1.0f);
		glUniform4f(loc_light[i].diffuse_color, 0.5f, 0.5f, 0.5f, 0.5f);
		glUniform4f(loc_light[i].specular_color, 0.8f, 0.8f, 0.8f, 1.0f);
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUseProgram(0);
}

void initialize_flags(void) {
	flag_fog = 0;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_flag_fog, flag_fog);
	glUseProgram(0);
}

bool readTexImage2D_from_file(char* filename) {
	FREE_IMAGE_FORMAT tx_file_format;
	int tx_bits_per_pixel;
	FIBITMAP* tx_pixmap, * tx_pixmap_32;

	int width, height;
	GLvoid* data;

	tx_file_format = FreeImage_GetFileType(filename, 0);
	// assume everything is fine with reading texture from file: no error checking
	tx_pixmap = FreeImage_Load(tx_file_format, filename);
	if (tx_pixmap == NULL)
		return false;
	tx_bits_per_pixel = FreeImage_GetBPP(tx_pixmap);

	//fprintf(stdout, " * A %d-bit texture was read from %s.\n", tx_bits_per_pixel, filename);
	if (tx_bits_per_pixel == 32)
		tx_pixmap_32 = tx_pixmap;
	else {
		fprintf(stdout, " * Converting texture from %d bits to 32 bits...\n", tx_bits_per_pixel);
		tx_pixmap_32 = FreeImage_ConvertTo32Bits(tx_pixmap);
	}

	width = FreeImage_GetWidth(tx_pixmap_32);
	height = FreeImage_GetHeight(tx_pixmap_32);
	data = FreeImage_GetBits(tx_pixmap_32);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	//fprintf(stdout, " * Loaded %dx%d RGBA texture into graphics memory.\n\n", width, height);

	FreeImage_Unload(tx_pixmap_32);
	if (tx_bits_per_pixel != 32)
		FreeImage_Unload(tx_pixmap);

	return true;
}

void prepare_bistro_exterior(void) {
	int n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	// VBO, VAO malloc
	bistro_exterior_VBO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);
	bistro_exterior_VAO = (GLuint*)malloc(sizeof(GLuint) * scene.n_materials);

	bistro_exterior_n_triangles = (int*)malloc(sizeof(int) * scene.n_materials);
	bistro_exterior_vertex_offset = (int*)malloc(sizeof(int) * scene.n_materials);

	flag_texture_mapping = (bool*)malloc(sizeof(bool) * scene.n_textures);

	// vertices
	bistro_exterior_vertices = (GLfloat**)malloc(sizeof(GLfloat*) * scene.n_materials);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		MATERIAL* pMaterial = &(scene.material_list[materialIdx]);
		GEOMETRY_TRIANGULAR_MESH* tm = &(pMaterial->geometry.tm);

		// vertex
		bistro_exterior_vertices[materialIdx] = (GLfloat*)malloc(sizeof(GLfloat) * 8 * tm->n_triangle * 3);

		int vertexIdx = 0;
		for (int triIdx = 0; triIdx < tm->n_triangle; triIdx++) {
			TRIANGLE tri = tm->triangle_list[triIdx];
			for (int triVertex = 0; triVertex < 3; triVertex++) {
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].x;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].y;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.position[triVertex].z;

				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].x;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].y;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.normal_vetcor[triVertex].z;

				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].u;
				bistro_exterior_vertices[materialIdx][vertexIdx++] = tri.texture_list[triVertex][0].v;
			}
		}

		// # of triangles
		bistro_exterior_n_triangles[materialIdx] = tm->n_triangle;

		if (materialIdx == 0)
			bistro_exterior_vertex_offset[materialIdx] = 0;
		else
			bistro_exterior_vertex_offset[materialIdx] = bistro_exterior_vertex_offset[materialIdx - 1] + 3 * bistro_exterior_n_triangles[materialIdx - 1];

		glGenBuffers(1, &bistro_exterior_VBO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, bistro_exterior_VBO[materialIdx]);
		glBufferData(GL_ARRAY_BUFFER, bistro_exterior_n_triangles[materialIdx] * 3 * n_bytes_per_vertex,
			bistro_exterior_vertices[materialIdx], GL_STATIC_DRAW);

		// As the geometry data exists now in graphics memory, ...
		free(bistro_exterior_vertices[materialIdx]);

		// Initialize vertex array object.
		glGenVertexArrays(1, &bistro_exterior_VAO[materialIdx]);
		glBindVertexArray(bistro_exterior_VAO[materialIdx]);

		glBindBuffer(GL_ARRAY_BUFFER, bistro_exterior_VBO[materialIdx]);
		glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(0));
		glEnableVertexAttribArray(INDEX_VERTEX_POSITION);
		glVertexAttribPointer(INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_NORMAL);
		glVertexAttribPointer(INDEX_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(INDEX_TEX_COORD);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if ((materialIdx > 0) && (materialIdx % 100 == 0))
			fprintf(stdout, " * Loaded %d bistro exterior materials into graphics memory.\n", materialIdx / 100 * 100);
	}
	fprintf(stdout, " * Loaded %d bistro exterior materials into graphics memory.\n", scene.n_materials);

	// textures
	bistro_exterior_texture_names = (GLuint*)malloc(sizeof(GLuint) * scene.n_textures);
	glGenTextures(scene.n_textures, bistro_exterior_texture_names);

	for (int texId = 0; texId < scene.n_textures; texId++) {
		glActiveTexture(GL_TEXTURE0 + texId);
		glBindTexture(GL_TEXTURE_2D, bistro_exterior_texture_names[texId]);

		bool bReturn = readTexImage2D_from_file(scene.texture_file_name[texId]);

		if (bReturn) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			flag_texture_mapping[texId] = true;
		}
		else {
			flag_texture_mapping[texId] = false;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	fprintf(stdout, " * Loaded bistro exterior textures into graphics memory.\n\n");

	free(bistro_exterior_vertices);
}

void draw_bistro_exterior(void) {
	glUseProgram(h_ShaderProgram_TXPS);
	ModelViewMatrix = ViewMatrix;
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::transpose(glm::inverse(glm::mat3(ModelViewMatrix)));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

	for (int materialIdx = 0; materialIdx < scene.n_materials; materialIdx++) {
		// set material
		glUniform4fv(loc_material.ambient_color, 1, scene.material_list[materialIdx].shading.ph.ka);
		glUniform4fv(loc_material.diffuse_color, 1, scene.material_list[materialIdx].shading.ph.kd);
		glUniform4fv(loc_material.specular_color, 1, scene.material_list[materialIdx].shading.ph.ks);
		glUniform1f(loc_material.specular_exponent, scene.material_list[materialIdx].shading.ph.spec_exp);
		glUniform4fv(loc_material.emissive_color, 1, scene.material_list[materialIdx].shading.ph.kr);

		int texId = scene.material_list[materialIdx].diffuseTexId;
		glUniform1i(loc_texture, texId);
		glUniform1i(loc_flag_texture_mapping, flag_texture_mapping[texId]);

		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0 + texId);
		glBindTexture(GL_TEXTURE_2D, bistro_exterior_texture_names[texId]);

		glBindVertexArray(bistro_exterior_VAO[materialIdx]);
		glDrawArrays(GL_TRIANGLES, 0, 3 * bistro_exterior_n_triangles[materialIdx]);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

// TO DO

#define LOC_VERTEX 0
#define N_TIGER_FRAMES 12

GLuint tiger_VBO, tiger_VAO;
int tiger_n_triangles[N_TIGER_FRAMES];
int tiger_vertex_offset[N_TIGER_FRAMES];
GLfloat* tiger_vertices[N_TIGER_FRAMES];
int cur_frame_tiger = 0;
int cur_frame_spider = 0;

//geometry load 함수
int read_geometry(GLfloat** object, int bytes_per_primitive, char* filename) {
	int n_triangles;
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float*)malloc(n_triangles * bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_triangles, fp);
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void prepare_tiger(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tiger_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);
		tiger_n_triangles[i] = read_geometry(&tiger_vertices[i], n_bytes_per_triangle, filename);
		// Assume all geometry files are effective.
		tiger_n_total_triangles += tiger_n_triangles[i];

		if (i == 0)
			tiger_vertex_offset[i] = 0;
		else
			tiger_vertex_offset[i] = tiger_vertex_offset[i - 1] + 3 * tiger_n_triangles[i - 1];
	}

	// Initialize vertex buffer object.
	glGenBuffers(1, &tiger_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glBufferData(GL_ARRAY_BUFFER, tiger_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_TIGER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, tiger_vertex_offset[i] * n_bytes_per_vertex,
			tiger_n_triangles[i] * n_bytes_per_triangle, tiger_vertices[i]);

	// As the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_TIGER_FRAMES; i++)
		free(tiger_vertices[i]);

	// Initialize vertex array object.
	glGenVertexArrays(1, &tiger_VAO);
	glBindVertexArray(tiger_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tiger_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define N_SPIDER_FRAMES 16
GLuint spider_VBO, spider_VAO;
int spider_n_triangles[N_SPIDER_FRAMES];
int spider_vertex_offset[N_SPIDER_FRAMES];
GLfloat* spider_vertices[N_SPIDER_FRAMES];

void prepare_spider(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, spider_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_SPIDER_FRAMES; i++) {
		sprintf(filename, "Data/spider_vnt_%d%d.geom", i / 10, i % 10);
		spider_n_triangles[i] = read_geometry(&spider_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		spider_n_total_triangles += spider_n_triangles[i];

		if (i == 0)
			spider_vertex_offset[i] = 0;
		else
			spider_vertex_offset[i] = spider_vertex_offset[i - 1] + 3 * spider_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &spider_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glBufferData(GL_ARRAY_BUFFER, spider_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_SPIDER_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, spider_vertex_offset[i] * n_bytes_per_vertex,
			spider_n_triangles[i] * n_bytes_per_triangle, spider_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_SPIDER_FRAMES; i++)
		free(spider_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &spider_VAO);
	glBindVertexArray(spider_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, spider_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint bike_VBO, bike_VAO;
int bike_n_triangles;
GLfloat* bike_vertices;

void prepare_bike(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, bike_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/bike_vnt.geom");
	bike_n_triangles = read_geometry(&bike_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	bike_n_total_triangles += bike_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &bike_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, bike_VBO);
	glBufferData(GL_ARRAY_BUFFER, bike_n_total_triangles * 3 * n_bytes_per_vertex, bike_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(bike_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &bike_VAO);
	glBindVertexArray(bike_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, bike_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint bus_VBO, bus_VAO;
int bus_n_triangles;
GLfloat* bus_vertices;

void prepare_bus(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, bus_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/bus_vnt.geom");
	bus_n_triangles = read_geometry(&bus_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	bus_n_total_triangles += bus_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &bus_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glBufferData(GL_ARRAY_BUFFER, bus_n_total_triangles * 3 * n_bytes_per_vertex, bus_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(bus_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &bus_VAO);
	glBindVertexArray(bus_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, bus_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint cow_VBO, cow_VAO;
int cow_n_triangles;
GLfloat* cow_vertices;

void prepare_cow(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, cow_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/cow_vn.geom");
	cow_n_triangles = read_geometry(&cow_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	cow_n_total_triangles += cow_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &cow_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, cow_VBO);
	glBufferData(GL_ARRAY_BUFFER, cow_n_total_triangles * 3 * n_bytes_per_vertex, cow_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(cow_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &cow_VAO);
	glBindVertexArray(cow_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, cow_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint ironman_VBO, ironman_VAO;
int ironman_n_triangles;
GLfloat* ironman_vertices;

void prepare_ironman(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, ironman_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/ironman_vnt.geom");
	ironman_n_triangles = read_geometry(&ironman_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	ironman_n_total_triangles += ironman_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &ironman_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glBufferData(GL_ARRAY_BUFFER, ironman_n_total_triangles * 3 * n_bytes_per_vertex, ironman_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(ironman_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &ironman_VAO);
	glBindVertexArray(ironman_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, ironman_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GLuint tank_VBO, tank_VAO;
int tank_n_triangles;
GLfloat* tank_vertices;

void prepare_tank(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, tank_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	sprintf(filename, "Data/tank_vnt.geom");
	tank_n_triangles = read_geometry(&tank_vertices, n_bytes_per_triangle, filename);
	// assume all geometry files are effective
	tank_n_total_triangles += tank_n_triangles;


	// initialize vertex buffer object
	glGenBuffers(1, &tank_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, tank_VBO);
	glBufferData(GL_ARRAY_BUFFER, tank_n_total_triangles * 3 * n_bytes_per_vertex, tank_vertices, GL_STATIC_DRAW);

	// as the geometry data exists now in graphics memory, ...
	free(tank_vertices);

	// initialize vertex array object
	glGenVertexArrays(1, &tank_VAO);
	glBindVertexArray(tank_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, tank_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define N_WOLF_FRAMES 17
GLuint wolf_VBO, wolf_VAO;
int wolf_n_triangles[N_WOLF_FRAMES];
int wolf_vertex_offset[N_WOLF_FRAMES];
GLfloat* wolf_vertices[N_WOLF_FRAMES];

void prepare_wolf(void) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle, wolf_n_total_triangles = 0;
	char filename[512];

	n_bytes_per_vertex = 8 * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	for (i = 0; i < N_WOLF_FRAMES; i++) {
		sprintf(filename, "Data/wolf_%02d_vnt.geom", i);
		wolf_n_triangles[i] = read_geometry(&wolf_vertices[i], n_bytes_per_triangle, filename);
		// assume all geometry files are effective
		wolf_n_total_triangles += wolf_n_triangles[i];

		if (i == 0)
			wolf_vertex_offset[i] = 0;
		else
			wolf_vertex_offset[i] = wolf_vertex_offset[i - 1] + 3 * wolf_n_triangles[i - 1];
	}

	// initialize vertex buffer object
	glGenBuffers(1, &wolf_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glBufferData(GL_ARRAY_BUFFER, wolf_n_total_triangles * n_bytes_per_triangle, NULL, GL_STATIC_DRAW);

	for (i = 0; i < N_WOLF_FRAMES; i++)
		glBufferSubData(GL_ARRAY_BUFFER, wolf_vertex_offset[i] * n_bytes_per_vertex,
			wolf_n_triangles[i] * n_bytes_per_triangle, wolf_vertices[i]);

	// as the geometry data exists now in graphics memory, ...
	for (i = 0; i < N_WOLF_FRAMES; i++)
		free(wolf_vertices[i]);

	// initialize vertex array object
	glGenVertexArrays(1, &wolf_VAO);
	glBindVertexArray(wolf_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, wolf_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void prepare_my_objects_20161611(void) {
	prepare_tiger();
	prepare_spider();
	prepare_bike();
	prepare_bus();
	prepare_cow();
	prepare_ironman();
	prepare_tank();
	prepare_wolf();
}

int timestamp_scene_tiger;
;
int tiger_eye_flag;
int tiger_shake_head_flag;
glm::mat4 ModelMatrix;

#define CAM_ANGLE 1.0f

//'t'
void set_tiger_eye(void)
{
	glm::vec4 tiger_eye = ModelMatrix * glm::vec4(0.0f, -88.0f, 62.0f, 1.0f);
	glm::vec4 zero = ModelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 uaxis = ModelMatrix * glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f) - zero;
	glm::vec4 vaxis = ModelMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) - zero;
	glm::vec4 naxis = ModelMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) - zero;

	current_camera.pos[0] = tiger_eye.x; current_camera.pos[1] = tiger_eye.y; current_camera.pos[2] = tiger_eye.z;
	current_camera.uaxis[0] = uaxis.x; current_camera.uaxis[1] = uaxis.y; current_camera.uaxis[2] = uaxis.z;
	current_camera.vaxis[0] = vaxis.x; current_camera.vaxis[1] = vaxis.y; current_camera.vaxis[2] = vaxis.z;
	current_camera.naxis[0] = naxis.x; current_camera.naxis[1] = naxis.y; current_camera.naxis[2] = naxis.z;

	if (tiger_shake_head_flag == 1)
	{
		int nt = timestamp_scene_tiger % 40;
		glm::mat4 axis_rotate = glm::mat4(1.0f);
		glm::vec3 new_vaxis;
		glm::vec3 new_naxis;
		float angle;
		if (nt < 20)
			angle = -TO_RADIAN * CAM_ANGLE * nt / 2;
		else angle = -TO_RADIAN * 10 + TO_RADIAN * CAM_ANGLE * (nt - 20) / 2;

		axis_rotate = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(uaxis[0], uaxis[1], uaxis[2]));
		new_vaxis = glm::vec3(axis_rotate * vaxis);
		new_naxis = glm::vec3(axis_rotate * naxis);
		current_camera.vaxis[0] = new_vaxis[0];	current_camera.vaxis[1] = new_vaxis[1];	current_camera.vaxis[2] = new_vaxis[2];
		current_camera.naxis[0] = new_naxis[0];	current_camera.naxis[1] = new_naxis[1];	current_camera.naxis[2] = new_naxis[2];
	}

	set_ViewMatrix_from_camera_frame();

	ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

//'g'
void set_tiger_watch(void)
{
	int nt = timestamp_scene_tiger % 2370;

	glm::vec4 camera_pos;
	glm::vec4 zero;
	glm::vec4 uaxis;
	glm::vec4 vaxis;
	glm::vec4 naxis;
	glm::mat4 tmpmat;

	float nlength;
	float t;

	if (nt >= 600 && nt < 960)
	{
		t = float(nt - 600) / 360;
		tmpmat = glm::translate(glm::mat4(1.0f), glm::vec3(-479.434540 + t * (-435.445557 - (-479.434540))
			, 733.972229 + t * (-659.739380 - 733.972229), 310));
		tmpmat = glm::rotate(tmpmat, -20 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		camera_pos = tmpmat * glm::vec4(0.0f, 300.0f, 100.0f, 1.0f);
	}
	else if (nt >= 1380 && nt < 1740)
	{
		t = float(nt - 1380) / 360;
		tmpmat = glm::translate(glm::mat4(1.0f), glm::vec3(3436.646240 + t * (-104.064941 - (3436.646240))
			, -2077.112305 + t * (-801.719910 - (-2077.112305)), 10));
		tmpmat = glm::rotate(tmpmat, 245 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		camera_pos = tmpmat * glm::vec4(0.0f, 800.0f, 150.0f, 1.0f);
	}
	else if (nt >= 1740 && nt < 1830)
	{
		t = float(nt - 1740) / 90;
		tmpmat = glm::translate(glm::mat4(1.0f), glm::vec3(-104.064941 + t * (-529.608704 - (-104.064941))
			, -801.719910 + t * (617.851379 - (-801.719910)), 10));
		tmpmat = glm::rotate(tmpmat, (155) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		camera_pos = tmpmat * glm::vec4(0.0f, 500.0f, 100.0f, 1.0f);
	}
	else if (nt >= 1830 && nt < 2190)
	{
		t = float(nt - 1830) / 360;
		tmpmat = glm::translate(glm::mat4(1.0f), glm::vec3(-529.608704 + t * (1457.488281 - (-529.608704))
			, 617.851379 + t * (3484.590820 - (617.851379)), 10));
		tmpmat = glm::rotate(tmpmat, 155 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		camera_pos = tmpmat * glm::vec4(0.0f, 500.0f, 100.0f, 1.0f);
	}
	else
	{
		tmpmat = ModelMatrix;
		camera_pos = tmpmat * glm::vec4(0.0f, 300.0f, 100.0f, 1.0f);
	}


	zero = tmpmat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	uaxis;
	vaxis = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	naxis = tmpmat * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) - zero;

	uaxis = glm::vec4(vaxis.y * naxis.z - vaxis.z * naxis.y
		, vaxis.z * naxis.x - vaxis.x * naxis.z, vaxis.x * naxis.y - vaxis.y * naxis.x, 1.0f);
	nlength = sqrt(uaxis.x * uaxis.x + uaxis.y * uaxis.y + uaxis.z * uaxis.z);

	uaxis /= nlength;

	current_camera.pos[0] = camera_pos.x; current_camera.pos[1] = camera_pos.y; current_camera.pos[2] = camera_pos.z;
	current_camera.uaxis[0] = uaxis.x; current_camera.uaxis[1] = uaxis.y; current_camera.uaxis[2] = uaxis.z;
	current_camera.vaxis[0] = vaxis.x; current_camera.vaxis[1] = vaxis.y; current_camera.vaxis[2] = vaxis.z;
	current_camera.naxis[0] = naxis.x; current_camera.naxis[1] = naxis.y; current_camera.naxis[2] = naxis.z;

	set_ViewMatrix_from_camera_frame();

	ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void draw_tiger(void) {
	float rotation_angle_tiger_y = 0.0f;
	float rotation_angle_tiger_z = 0.0f;
	float rotation_angle_tiger_x = 0.0f;
	float turn_angle_tiger = 0.0f;
	float t;
	int nt = timestamp_scene_tiger % 2370;
	glUseProgram(h_ShaderProgram_simple);
	ModelMatrix = glm::mat4(1.0f);

	if (nt < 90)
	{
		tiger_shake_head_flag = 1;
		rotation_angle_tiger_z = ((nt / 20 - 60) % 360) * TO_RADIAN;
		turn_angle_tiger = (nt) % 360 * TO_RADIAN;
		//회전 중심점
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1136.181763, 3716.681641, 10));
		ModelMatrix = glm::rotate(ModelMatrix, -turn_angle_tiger, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(112.321899, 380.919921, 0));
		ModelMatrix = glm::rotate(ModelMatrix, -rotation_angle_tiger_z, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (nt >= 90 && nt < 390)
	{
		tiger_shake_head_flag = 1;
		t = float(nt - 90) / 300;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1517.101684 + t * (166.990982 - 1517.101684)
			, 3604.359742 + t * (1494.852051 - 3604.359742), 10));
		ModelMatrix = glm::rotate(ModelMatrix, -35 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (nt >= 390 && nt < 540)
	{
		tiger_shake_head_flag = 1;
		t = float(nt - 390) / 150;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(166.990982 + t * (-479.434540 - 166.990982)
			, 1494.852051 + t * (733.972229 - 1494.852051), 10));
		ModelMatrix = glm::rotate(ModelMatrix, -36 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (nt >= 540 && nt < 600)
	{
		tiger_shake_head_flag = 0;
		t = float(nt - 540) / 60;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-479.434540
			, 733.972229, 10 + t * 300));
		ModelMatrix = glm::rotate(ModelMatrix, (-36 + t * 16) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (nt >= 600 && nt < 960)
	{
		tiger_shake_head_flag = 0;
		t = float(nt - 600) / 360;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-479.434540 + t * (-435.445557 - (-479.434540))
			, 733.972229 + t * (-659.739380 - 733.972229), 310 + 40));
		rotation_angle_tiger_y = ((nt - 600) * 10 % 360) * TO_RADIAN;
		ModelMatrix = glm::rotate(ModelMatrix, -20 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle_tiger_y, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, -40.0f));
	}
	else if (nt >= 960 && nt < 1020)
	{
		tiger_shake_head_flag = 0;
		t = float(nt - 960) / 60;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-435.445557
			, -659.739380, 310 - t * 300));
		ModelMatrix = glm::rotate(ModelMatrix, (-20 + t * 85) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (nt >= 1020 && nt < 1380)
	{
		tiger_shake_head_flag = 1;
		t = float(nt - 1020) / 360;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-435.445557 + t * (3436.646240 - (-435.445557))
			, -659.739380 + t * (-2077.112305 - (-659.739380)), 10));
		ModelMatrix = glm::rotate(ModelMatrix, 65 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (nt >= 1380 && nt < 1740)
	{
		tiger_shake_head_flag = 0;
		t = float(nt - 1380) / 360;
		int z = 0;
		float t2 = 0;
		if (nt < 1410 || (nt >= 1470 && nt < 1500)
			|| (nt >= 1560 && nt < 1590)
			|| (nt >= 1650 && nt < 1680))
		{
			if (nt < 1410)
				t2 = float(nt - 1380);
			else if (nt < 1500)
				t2 = float(nt - 1470);
			else if (nt < 1590)
				t2 = float(nt - 1560);
			else
				t2 = float(nt - 1650);
			t2 /= 30;
			z = 300 * t2;
			rotation_angle_tiger_x = -90 * t2;
		}
		else if (nt < 1470 || (nt >= 1500 && nt < 1560)
			|| (nt >= 1590 && nt < 1650)
			|| (nt >= 1680 && nt < 1740))
		{
			if (nt < 1470)
				t2 = float(nt - 1410);
			else if (nt < 1560)
				t2 = float(nt - 1500);
			else if (nt < 1650)
				t2 = float(nt - 1590);
			else
				t2 = float(nt - 1680);

			t2 /= 60;
			z = 300 - 300 * t2;
			rotation_angle_tiger_x = 270 + -270 * t2;
		}
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(3436.646240 + t * (-104.064941 - (3436.646240))
			, -2077.112305 + t * (-801.719910 - (-2077.112305)), 10 + z));
		ModelMatrix = glm::rotate(ModelMatrix, 65 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle_tiger_x * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 200.0f, 0.0f));
	}
	else if (nt >= 1740 && nt < 1830)
	{
		tiger_shake_head_flag = 0;
		t = float(nt - 1740) / 90;
		int z = 0;
		float t2 = 0;
		if (nt < 1770)
		{
			t2 = float(nt - 1740) / 30;
			z = 300 * t2;
			rotation_angle_tiger_x = -90 * t2;
		}
		else
		{
			t2 = float(nt - 1770) / 60;
			z = 300 - 300 * t2;
			rotation_angle_tiger_x = 270 + -270 * t2;
		}
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-104.064941 + t * (-529.608704 - (-104.064941))
			, -801.719910 + t * (617.851379 - (-801.719910)), 10 + z));
		ModelMatrix = glm::rotate(ModelMatrix, (65 + 20 * t2) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -200.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle_tiger_x * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 200.0f, 0.0f));
	}
	else if (nt >= 1830 && nt < 2190)
	{
		tiger_shake_head_flag = 0;
		t = float(nt - 1830) / 360;
		rotation_angle_tiger_y = (nt - 1830) * 10 % 360;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-529.608704 + t * (1457.488281 - (-529.608704))
			, 617.851379 + t * (3484.590820 - (617.851379)), 10));
		ModelMatrix = glm::rotate(ModelMatrix, 85 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 50.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle_tiger_y * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0, -50.0f));
	}
	else if (nt >= 2190 && nt < 2220)
	{
		tiger_shake_head_flag = 1;
		t = float(nt - 2190) / 30;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1457.488281 + t * (1517.101684 - 1457.488281)
			, 3484.590820 + t * (3604.359742 - 3484.590820), 10));
		ModelMatrix = glm::rotate(ModelMatrix, (85 + t * 60) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else if (nt >= 2220 && nt < 2310)
	{
		tiger_shake_head_flag = 1;
		rotation_angle_tiger_z = (((nt - 2220) / 20 + 145) % 360) * TO_RADIAN;
		turn_angle_tiger = (nt - 2220) % 360 * TO_RADIAN;

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1136.181763, 3716.681641, 10));
		ModelMatrix = glm::rotate(ModelMatrix, turn_angle_tiger, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(380.919921, -112.321899, 0));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle_tiger_z, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else
	{
		tiger_shake_head_flag = 1;
		t = float(nt - 2310) / 60;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1248.503662, 4097.601562, 10));
		ModelMatrix = glm::rotate(ModelMatrix, (239 + t * 181) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	if (camera_mod == CAMERA_t)
		set_tiger_eye();
	if (camera_mod == CAMERA_g)
		set_tiger_watch();

	ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1.0f, 0.0f, 1.0f); // Tiger wireframe color = magenta

	glBindVertexArray(tiger_VAO);
	glDrawArrays(GL_TRIANGLES, tiger_vertex_offset[cur_frame_tiger], 3 * tiger_n_triangles[cur_frame_tiger]);
	glBindVertexArray(0);
	glUseProgram(0);
	
}

int timestamp_scene_moving_object;

void draw_spider()
{
	float t = 0;
	int nt = timestamp_scene_moving_object % 1800;
	glUseProgram(h_ShaderProgram_simple);
	if (nt < 360)
	{
		t = float(nt) / 360;
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-2236.867432 + t * (-2933.899902 - (-2236.867432))
			, 902.053711 + t *(1648.135254 - 902.053711), 750.927368));
			ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -70.0f, 40.0f));

		ModelViewMatrix = glm::rotate(ModelViewMatrix, 50 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (nt >= 360 && nt < 720)
	{
		t = float(nt - 360) / 360;
		
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-2933.899902 + t * (-2631.692627 - (-2933.899902))
			, 1648.135254 + t * (2139.930908 - 1648.135254), 750.927368));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -70.0f, 40.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (nt >= 720 && nt < 1080)
	{
		t = float(nt - 720) / 360;

		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-2631.692627 + t * (-2048.920654 - (-2631.692627))
			, 2139.930908 + t * (2784.766602 - 2139.930908), 750.927368));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -70.0f, 40.0f));

		ModelViewMatrix = glm::rotate(ModelViewMatrix, 130 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (nt >= 1080 && nt < 1260)
	{
		t = float(nt - 1080) / 180;
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-2048.920654 + t * (-1085.127930 - (-2048.920654))
			, 2784.766602 + t * (2408.333496 - 2784.766602), 750.927368));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -70.0f, 40.0f));

		ModelViewMatrix = glm::rotate(ModelViewMatrix, (130 + 180 * t) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (nt >= 1260 && nt < 1620)
	{
		t = float(nt - 1260) / 360;

		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-1085.127930 + t * (-1816.343994 - (-1085.127930))
			, 2408.333496 + t * (1607.495605 - 2408.333496), 750.927368));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -70.0f, 40.0f));

		ModelViewMatrix = glm::rotate(ModelViewMatrix, 310 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
	{
		t = float(nt - 1620) / 180;

		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-1816.343994 + t * (-2236.867432 - (-1816.343994))
			, 1607.495605 + t * (902.053711 - 1607.495605), 750.927368));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -70.0f, 40.0f));

		ModelViewMatrix = glm::rotate(ModelViewMatrix, (310 + 100 * t) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1.0f, 0.0f, 1.0f); // Tiger wireframe color = magenta

	glBindVertexArray(spider_VAO);
	glDrawArrays(GL_TRIANGLES, spider_vertex_offset[cur_frame_spider], 3 * spider_n_triangles[cur_frame_spider]);
	glBindVertexArray(0);
	glUseProgram(0);
}

void draw_bike(void) {
	glUseProgram(h_ShaderProgram_simple);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-2531.891846, 1773.687622, 20));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1.0f, 0.0f, 1.0f);

	glBindVertexArray(bike_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bike_n_triangles);
	glBindVertexArray(0);
	glUseProgram(0);
}

void draw_bus(void) {
	glUseProgram(h_ShaderProgram_simple);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(5275.063965, - 1588.633179, 10));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(60.0f, 80.0f, 60.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 70 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1.0f, 0.0f, 1.0f);

	glBindVertexArray(bus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bus_n_triangles);
	glBindVertexArray(0);

	glUseProgram(h_ShaderProgram_simple);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(5275.063965, -1588.633179, 410));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(30.0f, 20.0f, 20.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1.0f, 0.0f, 1.0f);

	glBindVertexArray(bus_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * bus_n_triangles);
	glBindVertexArray(0);
}

void draw_cow(void) {
	glUseProgram(h_ShaderProgram_simple);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(3566.815918, -1864.608521, 480.433838));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 80.0f, 130.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 145 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1.0f, 0.0f, 1.0f);

	glBindVertexArray(cow_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * cow_n_triangles);
	glBindVertexArray(0);
}

void draw_ironman(void) {
	glUseProgram(h_ShaderProgram_simple);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(92.381546, -319.062866, 290.257141));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 80.0f, 46.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1.0f, 0.0f, 1.0f);

	glBindVertexArray(ironman_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * ironman_n_triangles);
	glBindVertexArray(0);
}


void draw_tank(void) {
	glUseProgram(h_ShaderProgram_simple);

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(733.943237, 2301.803467, 710.973450));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 80.0f, 46.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 56 * TO_RADIAN, glm::vec3(1.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1.0f, 0.0f, 1.0f);


	glBindVertexArray(tank_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tank_n_triangles);
	glBindVertexArray(0);
}

int cur_frame_wolf;

void draw_wolf(void) {
	glUseProgram(h_ShaderProgram_simple);
	int nt = timestamp_scene_moving_object % 180;
	float t = float(nt * 2) / 360;
	float t2 = float(nt % 45) / 45;
	int z;
	if (nt < 45 || (nt >= 90 && nt < 135))
		z = t2 * 200;
	else
		z = 200 - t2 * 200;
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(4484.260742, -2479.470459, 10 + z));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -t * 360 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(-195.305176, +798.099731, 0));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(400.0f, 400.0f, 400.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 80 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	if (nt < 45 || (nt >= 90 && nt < 135))
	{
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 0.0f, 5.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, t2 * 720 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 0.0f, -5.0f));
	}
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3f(loc_primitive_color, 1.0f, 0.0f, 1.0f);

	glBindVertexArray(wolf_VAO);
	glDrawArrays(GL_TRIANGLES, wolf_vertex_offset[cur_frame_wolf], 3 * wolf_n_triangles[cur_frame_wolf]);
	glBindVertexArray(0);
	glUseProgram(0);
}

void draw_my_objects_20161611()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	draw_tiger();
	draw_spider();
	draw_bike();
	draw_bus();
	draw_cow();
	draw_ironman();
	draw_tank();
	draw_wolf();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
/*****************************  END: geometry setup *****************************/

/********************  START: callback function definitions *********************/
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_my_objects_20161611();
	draw_grid();
	draw_axes();
	draw_bistro_exterior();

	glutSwapBuffers();
}

#define CAM_TSPEED 10.0f

//카메라 이동시 위치 설정
void renew_cam_position(int dir) {

	set_ViewMatrix_from_camera_frame();
	glm::mat4 invViewMatrix = glm::inverse(ViewMatrix);
	glm::vec4 new_cam_pos;

	//ec 에서 camera pos가 이동한 위치를 Mv의 inverse를 통해 wc 에서 위치로 변환
	if (dir == 0)
		new_cam_pos = invViewMatrix * glm::vec4(0, 0, -CAM_TSPEED, 1);
	else if (dir == 1)
		new_cam_pos = invViewMatrix * glm::vec4(0, 0, +CAM_TSPEED, 1);
	else if (dir == 2)
		new_cam_pos = invViewMatrix * glm::vec4(CAM_TSPEED, 0, 0, 1);
	else if (dir == 3)
		new_cam_pos = invViewMatrix * glm::vec4(-CAM_TSPEED, 0, 0, 1);
	else if (dir == 4)
		new_cam_pos = invViewMatrix * glm::vec4(0, CAM_TSPEED, 0, 1);
	else
		new_cam_pos = invViewMatrix * glm::vec4(0, -CAM_TSPEED, 0, 1);

	current_camera.pos[0] = new_cam_pos[0];
	current_camera.pos[1] = new_cam_pos[1];
	current_camera.pos[2] = new_cam_pos[2];
}

int world_ob_cam;
//'w', 'a', 's', 'd', 'q', 'e'
void move_camera(int direction_num)
{
	if (camera_mod != CAMERA_m)
		return;

	renew_cam_position(direction_num);
	set_ViewMatrix_from_camera_frame();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

int left_button;
//마우스 좌측 버튼, 'u', 'v', 'n'과 같이 사용시 반대 방향 회전
void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN)
			left_button = 1;
		else if (state == GLUT_UP)
			left_button = 0;
	}
}

//'u', 'v', 'n'
void rotate_camera(int axis)
{
	if (camera_mod != CAMERA_m)
		return;

	glm::mat4 axis_rotate;
	glm::vec3 uaxis = glm::vec3(current_camera.uaxis[0], current_camera.uaxis[1], current_camera.uaxis[2]);
	glm::vec3 vaxis = glm::vec3(current_camera.vaxis[0], current_camera.vaxis[1], current_camera.vaxis[2]);
	glm::vec3 naxis = glm::vec3(current_camera.naxis[0], current_camera.naxis[1], current_camera.naxis[2]);
	glm::vec3 new_uaxis, new_vaxis, new_naxis;
	float angle = TO_RADIAN * CAM_ANGLE;

	if (left_button == 0)
		angle *= -1;

	if (axis == 0)
	{
		axis_rotate = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(uaxis[0], uaxis[1], uaxis[2]));
		new_uaxis = uaxis;
		new_vaxis = glm::vec3(axis_rotate * glm::vec4(vaxis, 1));
		new_naxis = glm::vec3(axis_rotate * glm::vec4(naxis, 1));
	}
	else if (axis == 1)
	{
		axis_rotate = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(vaxis[0], vaxis[1], vaxis[2]));
		new_uaxis = glm::vec3(axis_rotate * glm::vec4(uaxis, 1));
		new_vaxis = vaxis;
		new_naxis = glm::vec3(axis_rotate * glm::vec4(naxis, 1));
	}
	else
	{
		axis_rotate = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(naxis[0], naxis[1], naxis[2]));
		new_uaxis = glm::vec3(axis_rotate * glm::vec4(uaxis, 1));
		new_vaxis = glm::vec3(axis_rotate * glm::vec4(vaxis, 1));
		new_naxis = naxis;
	}

	current_camera.uaxis[0] = new_uaxis[0];
	current_camera.uaxis[1] = new_uaxis[1];
	current_camera.uaxis[2] = new_uaxis[2];
	current_camera.vaxis[0] = new_vaxis[0];
	current_camera.vaxis[1] = new_vaxis[1];
	current_camera.vaxis[2] = new_vaxis[2];
	current_camera.naxis[0] = new_naxis[0];
	current_camera.naxis[1] = new_naxis[1];
	current_camera.naxis[2] = new_naxis[2];

	set_ViewMatrix_from_camera_frame();
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

int stop_flag;

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'f':
		b_draw_grid = b_draw_grid ? false : true;
		glutPostRedisplay();
		break;
	case '0':
		set_current_camera(CAMERA_0);
		glutPostRedisplay();
		break;
	case '1':
		set_current_camera(CAMERA_1);
		glutPostRedisplay();
		break;
	case '2':
		set_current_camera(CAMERA_2);
		glutPostRedisplay();
		break;
	case '3':
		set_current_camera(CAMERA_3);
		glutPostRedisplay();
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'm':
		set_current_camera(CAMERA_m);
		glutPostRedisplay();
		break;
	case 'w':
		move_camera(0);
		break;
	case 's':
		move_camera(1);
		break;
	case 'd':
		move_camera(2);
		break;
	case 'a':
		move_camera(3);
		break;
	case 'e':
		move_camera(4);
		break;
	case 'q':
		move_camera(5);
		break;
	case 'u':
		rotate_camera(0);
		break;
	case 'v':
		rotate_camera(1);
		break;
	case 'n':
		rotate_camera(2);
		break;
	case 'r':
		stop_flag = !stop_flag;
		break;
	case 't':
		camera_mod = CAMERA_t;
		break;
	case 'g':
		camera_mod = CAMERA_g;
		break;
	}
}

//줌 인, 줌 아웃
void mousewheel(int button, int dir, int x, int y) {
	int mod = glutGetModifiers();

	if (mod != GLUT_ACTIVE_SHIFT || camera_mod == CAMERA_t || camera_mod == CAMERA_g)
		return;
	if (dir > 0)
		current_camera.fovy -= TO_RADIAN * CAM_ANGLE;
	else
		current_camera.fovy += TO_RADIAN * CAM_ANGLE;

	ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

void reshape(int width, int height) {
	float aspect_ratio;

	glViewport(0, 0, width, height);

	ProjectionMatrix = glm::perspective(current_camera.fovy, current_camera.aspect_ratio, current_camera.near_c, current_camera.far_c);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

//애니메이션을 위한 timer
void timer_scene(int value) {
	cur_frame_tiger = timestamp_scene_tiger % N_TIGER_FRAMES;
	cur_frame_spider = timestamp_scene_moving_object % N_SPIDER_FRAMES;
	cur_frame_wolf = timestamp_scene_moving_object % N_WOLF_FRAMES;
	
	timestamp_scene_moving_object = (timestamp_scene_moving_object + 1) % INT_MAX;
	if (stop_flag == 0)
		timestamp_scene_tiger = (timestamp_scene_tiger + 1) % INT_MAX;
	glutPostRedisplay();
	glutTimerFunc(50, timer_scene, 0);
}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &grid_VAO);
	glDeleteBuffers(1, &grid_VBO);

	glDeleteVertexArrays(scene.n_materials, bistro_exterior_VAO);
	glDeleteBuffers(scene.n_materials, bistro_exterior_VBO);

	glDeleteVertexArrays(1, &tiger_VAO);
	glDeleteBuffers(1, &tiger_VBO);

	glDeleteTextures(scene.n_textures, bistro_exterior_texture_names);

	free(bistro_exterior_n_triangles);
	free(bistro_exterior_vertex_offset);

	free(bistro_exterior_VAO);
	free(bistro_exterior_VBO);

	free(bistro_exterior_texture_names);
	free(flag_texture_mapping);
}
/*********************  END: callback function definitions **********************/

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMouseWheelFunc(mousewheel);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	initialize_lights();
	initialize_flags();
}

void prepare_scene(void) {
	prepare_axes();
	prepare_grid();
	prepare_bistro_exterior();
	prepare_my_objects_20161611();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	initialize_camera();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "********************************************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "********************************************************************************\n\n");
}

void print_message(const char* m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "********************************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n********************************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 9
void drawScene(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 Bistro Exterior Scene";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used:",
		"		'f' : draw x, y, z axes and grid",
		"		'1' : set the camera for original view",
		"		'2' : set the camera for bistro view",
		"		'3' : set the camera for tree view",
		"		'4' : set the camera for top view",
		"		'5' : set the camera for front view",
		"		'6' : set the camera for side view",
		"		'ESC' : program close",
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(900, 600);
	glutInitWindowPosition(20, 20);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}
