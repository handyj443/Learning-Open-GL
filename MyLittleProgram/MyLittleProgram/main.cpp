#include <iostream>

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include "shader.h"

using u32 = unsigned int;
using VAO = u32;
using VBO = u32;
using EBO = u32;
using VSO = u32;
using FSO = u32;
using SPO = u32;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

int main()
{
	const int VIEWPORT_WIDTH = 1280;
	const int VIEWPORT_HEIGHT = 720;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
	                                      "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//__________________________________________________________________________
	// INITIALISATION
	int nAttritubes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttritubes);
	std::cout << "Max vertex attributes supported: " << nAttritubes
	          << std::endl;


	// Triangles
	VAO triangleVao;
	{
		float triangleVertices[]
		    = { -0.5f, -0.5f, 0.0f, -0.1f, -0.5f, 0.0f, -0.1f, 0.5f, 0.0f };

		// Bind vertex array object
		glGenVertexArrays(1, &triangleVao);
		glBindVertexArray(triangleVao);

		// Attach vertex buffer object
		VBO triangleVbo;
		glGenBuffers(1, &triangleVbo);
		glBindBuffer(GL_ARRAY_BUFFER, triangleVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices),
		             triangleVertices, GL_STATIC_DRAW);
		// Set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
		                      (void *)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDeleteBuffers(1, &triangleVbo);
	}

	VAO triangle2Vao;
	{
		float triangle2Vertices[]
		    = { // positions		// colors
			    0.1f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f,
			    0.0f, 1.0f,  0.0f, 0.1f, 0.5f, 0.0f, 0.0f, 0.0f,  1.0f
		      };

		// Bind vertex array object
		glGenVertexArrays(1, &triangle2Vao);
		glBindVertexArray(triangle2Vao);

		// Attach vertex buffer object
		VBO triangle2Vbo;
		glGenBuffers(1, &triangle2Vbo);
		glBindBuffer(GL_ARRAY_BUFFER, triangle2Vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2Vertices),
		             triangle2Vertices, GL_STATIC_DRAW);
		// Set our vertex attributes pointers
		// location, size, type, normalized, stride, pointer offset)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		                      (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		                      (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDeleteBuffers(1, &triangle2Vbo);
	}

	VAO rectangleVao;
	{
		float rectangleVertices[] = {
			0.5f,  0.5f,  0.0f, // top right
			0.5f,  -0.5f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f, // bottom left
			-0.5f, 0.5f,  0.0f  // top left
		};
		u32 rectangleIndices[] = {
			// note that we start from 0!
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		// Bind vertex array object
		glGenVertexArrays(1, &rectangleVao);
		glBindVertexArray(rectangleVao);

		// Attach vertex buffer object
		VBO rectangleVbo;
		glGenBuffers(1, &rectangleVbo);
		glBindBuffer(GL_ARRAY_BUFFER, rectangleVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices),
		             rectangleVertices, GL_STATIC_DRAW);
		// Set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
		                      (void *)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Attach element buffer object
		EBO rectangleEbo;
		glGenBuffers(1, &rectangleEbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangleEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices),
		             rectangleIndices, GL_STATIC_DRAW);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &rectangleVbo);
		glDeleteBuffers(1, &rectangleEbo);
	}

	// SHADERS
	Shader shaderProgram("shaders/1.vs", "shaders/1.fs");
	Shader shaderProgram2("shaders/2.vs", "shaders/2.fs");

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Clear color render target
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// RENDER

		// triangle
		float timeValue = (float)glfwGetTime();
		float greenValue = sin(timeValue) / 2.0f + 0.5f;

		shaderProgram.Use();
		// shaderProgram.SetFloat("xOffset", timeValue * 0.1f);
		shaderProgram.SetFloat("xOffset", 0.0f);
		shaderProgram.SetFloat4("ourColor", 0.0f, greenValue, 0.0f, 1.0f);
		glBindVertexArray(triangleVao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		shaderProgram2.Use();
		glBindVertexArray(triangle2Vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//// rectangle
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// glBindVertexArray(rectangleVao);
		// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &triangleVao);
	glDeleteVertexArrays(1, &rectangleVao);

	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}