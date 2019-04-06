#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

using namespace std;

struct LightSource
{
	glm::vec3 position;
};

struct Material
{
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
};

class CustomObject
{
public:
	CustomObject(glm::vec3 position, char * file, char * texture);
	~CustomObject();
	void Render();

private:
	GLuint program_id;
	GLuint texture_id;
	GLuint vao;
	GLuint uniform_mv;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 mv;
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> uvs;
	LightSource light;
	Material material;
	
	const char * fragmentshader_name = "fragmentshader.fsh";
	const char * vertexshader_name = "vertexshader.vsh";
	const int WIDTH = 800;
	const int HEIGHT = 600;
};

