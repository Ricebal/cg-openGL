#include "CustomObject.h"
#include "glsl.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "objloader.hpp"
#include "texture.hpp"

CustomObject::CustomObject(glm::vec3 position, char * object, char * texture)
{
	char * vertexshader = glsl::readFile(vertexshader_name);
	GLuint vsh_id = glsl::makeVertexShader(vertexshader);

	char * fragmentshader = glsl::readFile(fragmentshader_name);
	GLuint fsh_id = glsl::makeFragmentShader(fragmentshader);

	program_id = glsl::makeShaderProgram(vsh_id, fsh_id);

	model = glm::mat4();
	model = glm::rotate(model, 0.01f, glm::vec3(0.0f, 0.0f, 45.0f));
	view = glm::lookAt(
		position,
		glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(0.0, 1.0, 0.0)
	);
	projection = glm::perspective(
		glm::radians(45.0f),
		1.0f * WIDTH / HEIGHT,
		1.0f,
		20.0f
	);
	mv = view * model;

	light.position = glm::vec3(4.0, 4.0, 4.0);
	material.ambient_color = glm::vec3(0.2, 0.0, 0.1);
	material.diffuse_color = glm::vec3(0.5, 0.0, 0.25);

	bool res = loadOBJ(object, vertices, uvs, normals);
	texture_id = loadBMP(texture);

	GLuint position_id;
	GLuint normal_id;
	GLuint vbo_vertices;
	GLuint vbo_normals;
	GLuint vbo_uvs;

	// vbo for vertices
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &(vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vbo for vertices
	glGenBuffers(1, &vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &(normals[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vbo for uvs
	glGenBuffers(1, &vbo_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &(uvs[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint uv_id = glGetAttribLocation(program_id, "uv");


	// Get vertex attributes
	position_id = glGetAttribLocation(program_id, "position");
	normal_id = glGetAttribLocation(program_id, "normal");

	// Allocate memory for vao
	glGenVertexArrays(1, &vao);

	// Bind to vao
	glBindVertexArray(vao);

	// Bind vertices to vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position_id);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Bind vertices to vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glVertexAttribPointer(normal_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(normal_id);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// Bind uvs to vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
	glVertexAttribPointer(uv_id, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(uv_id);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Stop bind to vao
	glBindVertexArray(0);

	// Make uniform vars
	uniform_mv = glGetUniformLocation(program_id, "mv");
	GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
	GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
	GLuint uniform_material_ambient = glGetUniformLocation(program_id,
		"mat_ambient");
	GLuint uniform_material_diffuse = glGetUniformLocation(program_id,
		"mat_diffuse");

	glUseProgram(program_id);
	// Fill uniform vars
	glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));
	glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));
	glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(material.ambient_color));
	glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(material.diffuse_color));


	// Make uniform var
	uniform_mv = glGetUniformLocation(program_id, "mv");

	// Fill uniform var
	glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));
}

CustomObject::~CustomObject()
{
}

void CustomObject::Render() {
	// Send mvp
	glUseProgram(program_id);
	glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));

	// Send vao
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);

	// Do transformation
	model = glm::rotate(model, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
	mv = view * model;
}