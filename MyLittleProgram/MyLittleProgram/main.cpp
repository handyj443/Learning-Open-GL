#include <iostream>

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include "shader.h"
#include "stb_image.h"

using u32 = unsigned int;
using u8 = unsigned char;
using VAO = u32;
using VBO = u32;
using EBO = u32;
using VSO = u32;
using FSO = u32;
using TXO = u32;

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

	VAO rectangleVao;
	{
		float rectangleVertices[] = {
		    // positions          // colors           // texture coords
		     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		                      (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		                      (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		                      (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		
		
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

    // TEXTURES
    TXO texture;
    int width, height, nChannels;
    u8 *textureData = stbi_load("assets/nico.png", &width, &height, &nChannels, 0);
    if (textureData)
    {
        glGenTextures(1, &texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, textureData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
    	std::cout << "Failed to load texture." << std::endl;
    }
    stbi_image_free(textureData);

	// MAIN LOOP
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Clear color render target
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// RENDER
		// rectangle
		shaderProgram.Use();
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindVertexArray(rectangleVao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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