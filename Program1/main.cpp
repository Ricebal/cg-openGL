#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "vector"
#include "objloader.hpp"
#include "texture.hpp"

using namespace std;


//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 800, HEIGHT = 600;
const char * fragshader_name = "fragmentshader.fsh";
const char * vertexshader_name = "vertexshader.vsh";
unsigned const int DELTA = 10;

//--------------------------------------------------------------------------------
//typedefs
//--------------------------------------------------------------------------------

struct LightSource
{
	glm::vec3 position;
};

struct Material 
{
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
};

//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

GLuint program_id, texture_id;
GLuint vao;
GLuint uniform_mv;

glm::mat4 model, view, projection;
glm::mat4 mv;

LightSource light;
Material material;



//--------------------------------------------------------------------------------
// Mesh variables
//--------------------------------------------------------------------------------

vector<glm::vec3> vertices;
vector<glm::vec3> normals;
vector<glm::vec2> uvs;


//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void keyboardHandler(unsigned char key, int a, int b)
{
    if (key == 27)
        glutExit();
}


//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

void Render()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Send vao
    glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);

    // Do transformation
    model = glm::rotate(model, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
    mv = view * model;

    // Send mvp
    glUseProgram(program_id);
    glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));

    glutSwapBuffers();
}



//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void Render(int n)
{
    Render();
    glutTimerFunc(DELTA, Render, 0);
}


//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void InitGlutGlew(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Hello OpenGL");
    glutDisplayFunc(Render);
    glutKeyboardFunc(keyboardHandler);
    glutTimerFunc(DELTA, Render, 0);

    glewInit();
}


//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------

void InitShaders()
{
    char * vertexshader = glsl::readFile(vertexshader_name);
    GLuint vsh_id = glsl::makeVertexShader(vertexshader);

    char * fragshader = glsl::readFile(fragshader_name);
    GLuint fsh_id = glsl::makeFragmentShader(fragshader);

    program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}


//------------------------------------------------------------
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices()
{
    model = glm::mat4();
	model = glm::rotate(model, 0.01f, glm::vec3(0.0f, 0.0f, 45.0f));
    view = glm::lookAt(
        glm::vec3(5.0, 5.0, 10.0),
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0));
    projection = glm::perspective(
        glm::radians(45.0f),
        1.0f * WIDTH / HEIGHT, 0.1f,
        20.0f);
    mv = view * model;
}
//------------------------------------------------------------
// void InitObjects
// hier staat wat hij doet maar dat weet ik nog niet
//------------------------------------------------------------
void InitObjects() {
	bool res = loadOBJ("Objects/car.obj", vertices, uvs, normals);
	texture_id = loadBMP("Textures/Yellobrk.bmp");
}

//------------------------------------------------------------
// void InitMaterialsLight()
// Makes a lightmaterial thing
//------------------------------------------------------------
void InitMaterialsLight()
{
	light.position = glm::vec3(4.0, 4.0, 4.0);
	material.ambient_color = glm::vec3(0.2, 0.0, 0.1);
	material.diffuse_color = glm::vec3(0.5, 0.0, 0.25);

}


//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------

void InitBuffers()
{
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


int main(int argc, char ** argv)
{
    InitGlutGlew(argc, argv);
    InitShaders();
	InitMaterialsLight();
    InitMatrices();
    InitObjects();
    InitBuffers();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    glutMainLoop();

    return 0;
}
