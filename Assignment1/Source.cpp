#define _USE_MATH_DEFINES
#include <cstdio>
#include <iostream>
#include <string>
#include <charconv>
#include <cmath>
#include <cstddef>
using namespace std;

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include "shader.h"


//global vars

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define TIRE_RADIUS 0.20
#define RIM_RADIUS 0.13
#define CIRCLE_SLICES 32
#define MOVEMENT_SENSITIVITY 3.0f
#define ROTATION_SENSITIVITY 3.0f


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
	1.0f,1.0f,1.0f,
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
	-0.4f,ground[1] + 0.25f,0.0f, //vertex
	1.0f,0.0f,0.0f, //color
	-0.0f,ground[1] + 0.25f,0.0f, //vertex
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
	-0.28f,ground[1] + 0.70f,0.0f, //vertex 4
	0.7098f,0.7098f,0.74f,
	-0.20f,ground[1] + 0.70f,0.0f, //vertex 3
	0.7098f,0.7098f,0.74f,
	
};

GLfloat dump_box[] = {
	0.1f,ground[1] + 0.25f,0.0f, //vertex 1
	0.0f,1.0f,0.0f,
	0.6f,ground[1] + 0.25f,0.0f, //vertex 2
	0.0f,1.0f,0.0f,
	-0.0f,ground[1] + 0.50f,0.0f, //vertex 4
	0.0f,1.0f,0.0f,
	0.70f,ground[1] + 0.50f,0.0f, //vertex 3
	0.0f,1.0f,0.0f,
	0.1f,ground[1] + 0.70f,0.0f, //vertex 6
	0.0f,1.0f,0.0f,
	0.6f,ground[1] + 0.70f,0.0f, //vertex 5
	0.0f,1.0f,0.0f,
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
GLuint g_VBO[13];
GLuint g_VAO[10];
GLuint g_modelMatrixIndex = 0;
GLuint g_shaderProgramID = 0;
glm::mat4 g_modelMatrix[5];


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
	g_modelMatrixIndex = glGetUniformLocation(g_shaderProgramID, "uModelMatrix");

	//init model matrix to the identity matrix
	g_modelMatrix[0] = mat4(1.0f); //handles the ground (probably not needed)
	g_modelMatrix[1] = mat4(1.0f); //handles the truck chasis
	g_modelMatrix[2] = mat4(1.0f); //handles the dumpbox
	g_modelMatrix[3] = mat4(1.0f); //handles wheel 1
	g_modelMatrix[4] = mat4(1.0f); //handles wheel 2


	
	
	glGenBuffers(13, g_VBO);
	//ground
	glBindBuffer(GL_ARRAY_BUFFER,g_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);

	glGenVertexArrays(10, g_VAO);

	
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

	//DumpBox
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[12]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(dump_box), dump_box, GL_STATIC_DRAW);

	glBindVertexArray(g_VAO[8]);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO[12]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

	glEnableVertexAttribArray(positionIndex);
	glEnableVertexAttribArray(colorIndex);



	
}


static void update_scene(GLFWwindow* window, float frameTime) 
{
	// declare variables to transform the object
	glm::vec3 moveVec(0.0f, 0.0f, 0.0f);
	float wheelRotateAngle = 0.0f;
	glm::mat4 tempMat = mat4(1.0f);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		moveVec.x -= MOVEMENT_SENSITIVITY * frameTime;
		wheelRotateAngle += ROTATION_SENSITIVITY * frameTime;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		moveVec.x += MOVEMENT_SENSITIVITY * frameTime;
		wheelRotateAngle -= ROTATION_SENSITIVITY * frameTime;
	}


	//update model matrix for chasis
	g_modelMatrix[1] *= glm::translate(moveVec);
	//update Dump box model matrix
	g_modelMatrix[2] *= glm::translate(moveVec);
	//update Wheel 1 model matrix
	tempMat = mat4(1.0f);
	tempMat[3][0] = g_modelMatrix[1][3][0]; //x pos
	tempMat[3][1] = g_modelMatrix[1][3][1]; //y pos
	tempMat[3][2] = g_modelMatrix[1][3][2]; //z pos
	g_modelMatrix[3] = tempMat * glm::rotate(wheelRotateAngle
		,glm::vec3(0.0f,0.0f,1.0f));
	//update Wheel 2 model matrix
	tempMat = mat4(1.0f);
	tempMat[3][0] = g_modelMatrix[1][3][0]; //x pos
	tempMat[3][1] = g_modelMatrix[1][3][1]; //y pos
	tempMat[3][2] = g_modelMatrix[1][3][2]; //z pos
	g_modelMatrix[4]  *= glm::rotate(wheelRotateAngle
		, glm::vec3(0.0f, 0.0f, 1.0f));



}










static void render_scene() {
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(g_shaderProgramID);
	
	glUniformMatrix4fv(g_modelMatrixIndex, 1, GL_FALSE, &g_modelMatrix[0][0][0]);
	//ground
	glBindVertexArray(g_VAO[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUniformMatrix4fv(g_modelMatrixIndex, 1, GL_FALSE, &g_modelMatrix[1][0][0]);
	//base
	glBindVertexArray(g_VAO[5]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


	
	//truck compartment
	glBindVertexArray(g_VAO[6]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

	
	//truck window
	glBindVertexArray(g_VAO[7]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//solution for a model with moving parts is to have the tire/dump matrices
	//behave similarly to truck one, might be wrong but can't figure out the proper way
	//this is what is done in the orbit demo code
	
	glUniformMatrix4fv(g_modelMatrixIndex, 1, GL_FALSE, &g_modelMatrix[2][0][0]);
	//dump box
	glBindVertexArray(g_VAO[8]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);


	glUniformMatrix4fv(g_modelMatrixIndex, 1, GL_FALSE, &g_modelMatrix[3][0][0]);
	//Tire 1
	glBindVertexArray(g_VAO[1]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, g_slices + 2);

	//rim 1
	glBindVertexArray(g_VAO[3]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, g_slices + 2);

	glUniformMatrix4fv(g_modelMatrixIndex, 1, GL_FALSE, &g_modelMatrix[4][0][0]);
	//Tire 2
	glBindVertexArray(g_VAO[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, g_slices + 2);

	//rim 2
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
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	init();

	//taken from tuts
	double lastUpdateTime = glfwGetTime();	// last update time
	double elapsedTime = lastUpdateTime;	// time elapsed since last update
	float frameTime = 0.0f;				// frame time
	int frameCount = 0;						// number of frames since last update


	while (!glfwWindowShouldClose(window)) {
		update_scene(window, frameTime);		// update the scene
		render_scene();				// render the scene

		glfwSwapBuffers(window);	// swap buffers
		glfwPollEvents();			// poll for events

		frameCount++;
		elapsedTime = glfwGetTime() - lastUpdateTime;	// current time - last update time
		if (elapsedTime >= 1.0f)	// if time since last update >= to 1 second
		{
			frameTime = static_cast<float>(1.0f / frameCount);	// calculate frame time

			

			frameCount = 0;					// reset frame count
			lastUpdateTime += elapsedTime;	// update last update time
		}
	
	}

	glDeleteProgram(g_shaderProgramID);
	glDeleteBuffers(10, g_VBO);
	glDeleteVertexArrays(12, g_VAO);

	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);


}