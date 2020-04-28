#include <cstdio>
#include <iostream>
#include <string>
#include <charconv>
using namespace std;

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"

//global vars

GLfloat ground[] = {
	-1.0f,-0.75f,0.0f,//vertex 1
	0.0f,1.0f,0.0f, // colour 1
	-1.0f,-1.0f,0.0f, //vertex 2
	0.0f,1.0f,0.0f, //colour 2
	1.0f,-0.75f,0.0f,//vertex 3
	0.0f,1.0f,0.0f, //colour 3
	1.0f,-1.0f,0.0f, //vertex 4
	0.0f,1.0f,0.0f, //colour 4
};

//Truck Components


//end Truck Components
GLuint g_VBO[1];
GLuint g_VAO = 0;
GLuint g_shaderProgramID = 0;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
}

static void error_callback(int error, const char* description) {
	cerr << description << endl;
}

static void init() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	g_shaderProgramID = loadShaders("spColorVs.vert", "ColorFS.frag");
	glGenBuffers(1, g_VBO);
	glBindBuffer(GL_ARRAY_BUFFER,g_VBO[0]);
	
}

static void render_scene() {
	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();
}


int main(void) {
	GLFWwindow* window = NULL;



	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Assignment 1 5402669", NULL, NULL);

	if (window == NULL) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) {
		cerr << "GLEW initialisation failed" << endl;
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	init();


	while (!glfwWindowShouldClose(window)) {
		render_scene();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);


}