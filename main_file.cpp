#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include "tree.h"
#include <time.h>

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;

glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
float moveSpeed = 0.1f;

ShaderProgram* sp;

float* vertices = treeVertices;
float* normals = treeNormals;
float* texCoords = treeTexCoords;
float* colors = treeColors;
int vertexCount = treeVertexCount;
GLuint tex0;

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, filename);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_LEFT) cameraPos.x -= moveSpeed;
		if (key == GLFW_KEY_RIGHT) cameraPos.x += moveSpeed;
		if (key == GLFW_KEY_UP) cameraPos.z -= moveSpeed;
		if (key == GLFW_KEY_DOWN) cameraPos.z += moveSpeed;
		if (key == GLFW_KEY_W) cameraPos.y += moveSpeed;
		if (key == GLFW_KEY_S) cameraPos.y -= moveSpeed;
	}
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_1) speed_x = -PI / 2;
		if (key == GLFW_KEY_2) speed_x = PI / 2;
		if (key == GLFW_KEY_3) speed_y = PI / 2;
		if (key == GLFW_KEY_4) speed_y = -PI / 2;

	}
	if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_1) speed_x = 0;
		if (key == GLFW_KEY_2) speed_x = 0;
		if (key == GLFW_KEY_3) speed_y = 0;
		if (key == GLFW_KEY_4) speed_y = 0;

	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	tex0 = readTexture("metal.png");
}

void freeOpenGLProgram(GLFWwindow* window) {
	delete sp;
}

void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

	glm::mat4 V = glm::lookAt(
		cameraPos,
		cameraPos + glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f);
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
	M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));

	sp->use();
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform4f(sp->u("lp"), 0, 0, -6, 1);
	glUniform1i(sp->u("textureMap0"), 0);

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(sp->a("color"));
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors);
	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals);
	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

	glfwSwapBuffers(window);
}

int main(void) {
	GLFWwindow* window;
	srand(time(nullptr));
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		fprintf(stderr, "Nie mozna zainicjowac GLFW.\n");
		exit(EXIT_FAILURE);
	}
	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Nie mozna utworzyc okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Nie mozna zainicjowac GLEW.\n");
		exit(EXIT_FAILURE);
	}
	initOpenGLProgram(window);
	float angle_x = 0;
	float angle_y = 0;
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window)) {
		angle_x += speed_x * glfwGetTime();
		angle_y += speed_y * glfwGetTime();
		glfwSetTime(0);
		drawScene(window, angle_x, angle_y);
		glfwPollEvents();
	}
	freeOpenGLProgram(window);
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}