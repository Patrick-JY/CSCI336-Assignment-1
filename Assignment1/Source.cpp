#define _USE_MATH_DEFINES
#include <cstdio>
#include <iostream>
#include <string>
#include <charconv>
#include <cmath>
using namespace std;

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"

//global vars

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define TIRE_RADIUS 0.20
#define RIM_RADIUS 0.13
#define CIRCLE_SLICES 32


bool fillMode = true;

//moved this to global
float scale_factor = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;
struct Vertex {
	GLfloat position[3];
	GLfloat color[3];
};



GLfloat ground[] = {
	-1.0f,-0.60,0.0f,//vertex 1
	0.0f,1.0f,0.0f, // colour 1
	-1.0f,-1.0f,0.0f, //vertex 2
	0.0f,1.0f,0.0f, //colour 2
	1.0f,-0.60f,0.0f,//vertex 3
	0.0f,1.0f,0.0f, //colour 3
	1.0f,-1.0f,0.0f, //vertex 4
	0.0f,1.0f,0.0f, //colour 4
};

//Truck Components
GLfloat tire1Vertices[(CIRCLE_SLICES+2)*4] = {
	-0.2f, ground[1]+TIRE_RADIUS, 0.0f,// vertex 1
	TIRE_RADIUS * scale_factor + -0.2f, ground[1]+TIRE_RADIUS, 0.0f, //vertex 2
};
GLfloat tireColour[(CIRCLE_SLICES + 2) * 4] = {
	0.14,0.14,0.14, //color 1
	0.14,0.14,0.14, //color 2
};

GLfloat tire2Vertices[(CIRCLE_SLICES + 2) * 4] = {
	0.4f, ground[1] + TIRE_RADIUS, 0.0f,// vertex 1
	TIRE_RADIUS * scale_factor + 0.4f, ground[1] + TIRE_RADIUS, 0.0f, //vertex 2
};

GLfloat rimColour[(CIRCLE_SLICES + 2) * 4] = {
	0.7098f,0.7098f,0.74f,
	0.7098f,0.7098f,0.74f,
};

GLfloat rim1Vertices[(CIRCLE_SLICES + 2) * 4] = {
	-0.2f, ground[1] + RIM_RADIUS + 0.07f, 0.0f,// vertex 1
	RIM_RADIUS * scale_factor + -0.2f, ground[1] + RIM_RADIUS + 0.07f, 0.0f, //vertex 2
};

GLfloat rim2Vertices[(CIRCLE_SLICES + 2) * 4] = {
	0.4f, ground[1] + RIM_RADIUS + 0.07f, 0.0f,// vertex 1
	RIM_RADIUS * scale_factor + 0.4f, ground[1] + RIM_RADIUS + 0.07f, 0.0f, //vertex 2
};

GLfloat base[] = {
	-0.4f,ground[1] + 0.25f,0.0f, //vertex
	0.7098f,0.7098f,0.74f,
	-0.4f,ground[1] + 0.15f,0.0f, //vertex
	0.7098f,0.7098f,0.74f,
	0.6f,ground[1] + 0.25f,0.0f, //vertex
	0.7098f,0.7098f,0.74f,
	0.6f,ground[1] + 0.15f,0.0f, //vertex
	0.7098f,0.7098f,0.74f,

};

GLfloat driver_compartment[] = {
	-0.4f,ground[1] + 0.26f,0.0f, //vertex
	1.0f,0.0f,0.0f, //color
	-0.0f,ground[1] + 0.26f,0.0f, //vertex
	1.0f,0.0f,0.0f, //color
	-0.4f,ground[1] + 0.50f,0.0f, //vertex
	1.0f,0.0f,0.0f, //color

	-0.0f,ground[1] + 0.50f,0.0f, //vertex
	1.0f,0.0f,0.0f, //color

	-0.3f,ground[1] + 0.80f,0.0f,//vertex
	1.0f,0.0f,0.0f, //color

	-0.0f,ground[1] + 0.80f, 0.0f, //vertex
	1.0f,0.0f,0.0f, //color

};

GLfloat driver_window[] = {
	-0.35f,ground[1] + 0.50f,0.0f, //vertex 1
	0.7098f,0.7098f,0.74f,
	-0.20f,ground[1] + 0.50f,0.0f, //vertex 2
	0.7098f,0.7098f,0.74f,
	-0.20f,ground[1] + 0.70f,0.0f, //vertex 3
	0.7098f,0.7098f,0.74f,
};



//this needs to be changed at somepoint as it is currently pointless (might be needed for antweak)
GLuint g_slices = (CIRCLE_SLICES);

//generates a circle based on parameters
void generate_circle(float centrey,float centrex,GLfloat circleVertices [],GLfloat circleColour []
	, float radius) {
	float slice_angle = M_PI * 2 / static_cast<float>(g_slices);
	float angle = slice_angle;
		// scale to make it a circle instead of an elipse
	int index = 0;	// vertex index
	
	

	// generate vertex coordinates for triangle fan
	for (int i = 2; i < g_slices + 2; i++)
	{
		// multiply by 3 because a vertex has x, y, z coordinates
		index = i * 3;

		circleVertices[index] = radius * cos(angle)  * scale_factor + centrex;
		circleVertices[index + 1] = radius * sin(angle) + centrey;
		circleVertices[index + 2] = 0.0f;
		circleColour[index] = circleColour[0];
		circleColour[index + 1] = circleColour[1];
		circleColour[index + 2] = circleColour[2];
		// update to next angle
		angle += slice_angle;
	}

	



}


//end Truck Components
GLuint g_VBO[12];
GLuint g_VAO[9];

GLuint g_shaderProgramID = 0;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		if (fillMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fillMode = false;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fillMode = true;
		}
		return;
	}
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
}

static void error_callback(int error, const char* description) {
	cerr << description << endl;
}

static void init() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	g_shaderProgramID = loadShaders("spColorVs.vert", "ColorFS.frag");
	GLuint positionIndex = glGetAttribLocation(g_shaderProgramID, "aPosition");
	GLuint colorIndex = glGetAttribLocation(g_shaderProgramID, "aColor");



	
	
	glGenBuffers(12, g_VBO);
	//ground
	glBindBuffer(GL_ARRAY_BUFFER,g_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);

	glGenVertexArrays(9, g_VAO);

	
	glBindVertexArray(g_VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
		sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

	glEnableVertexAttribArray(positionIndex);
	glEnableVertexAttribArray(colorIndex);


	//Circles


	//tire1
	generate_circle(ground[1] + TIRE_RADIUS, -0.2f, tire1Vertices, tireColour,TIRE_RADIUS);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (g_slices + 2), tire1Vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (g_slices+2), tireColour, GL_DYNAMIC_DRAW);
	glBindVertexArray(g_VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//tire2
	generate_circle(ground[1] + TIRE_RADIUS, 0.4f, tire2Vertices, tireColour,TIRE_RADIUS);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (g_slices + 2), tire2Vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (g_slices + 2), tireColour, GL_DYNAMIC_DRAW);
	glBindVertexArray(g_VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[4]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	//rim1
	generate_circle(ground[1] + RIM_RADIUS + 0.07f, -0.2f, rim1Vertices, rimColour,RIM_RADIUS);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (g_slices + 2), rim1Vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (g_slices + 2), rimColour, GL_DYNAMIC_DRAW);
	glBindVertexArray(g_VAO[3]);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[5]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[6]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	//rim2
	generate_circle(ground[1] + RIM_RADIUS + 0.07f, 0.4f, rim2Vertices, rimColour,RIM_RADIUS);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[7]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (g_slices + 2), rim2Vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[8]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * (g_slices + 2), rimColour, GL_DYNAMIC_DRAW);
	glBindVertexArray(g_VAO[4]);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[7]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[8]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	//base
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[9]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(base), base, GL_STATIC_DRAW);

	glBindVertexArray(g_VAO[5]);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[9]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

	glEnableVertexAttribArray(positionIndex);
	glEnableVertexAttribArray(colorIndex);

	//Driver Compartment
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[10]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(driver_compartment), driver_compartment, GL_STATIC_DRAW);

	glBindVertexArray(g_VAO[6]);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[10]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

	glEnableVertexAttribArray(positionIndex);
	glEnableVertexAttribArray(colorIndex);


	//Driver Window
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[11]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(driver_window), driver_window, GL_STATIC_DRAW);

	glBindVertexArray(g_VAO[7]);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[11]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

	glEnableVertexAttribArray(positionIndex);
	glEnableVertexAttribArray(colorIndex);



	
}

static void render_scene() {
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(g_shaderProgramID);
	glBindVertexArray(g_VAO[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(g_VAO[5]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(g_VAO[6]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

	glBindVertexArray(g_VAO[1]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, g_slices + 2);

	glBindVertexArray(g_VAO[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, g_slices + 2);

	glBindVertexArray(g_VAO[3]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, g_slices + 2);

	glBindVertexArray(g_VAO[4]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, g_slices + 2);

	

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

	glDeleteProgram(g_shaderProgramID);
	glDeleteBuffers(5, g_VBO);
	glDeleteVertexArrays(2, g_VAO);

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);


}