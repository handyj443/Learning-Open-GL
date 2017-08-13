#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

using u32 = unsigned int;
using u8 = unsigned char;
using VAO = u32;
using VBO = u32;
using EBO = u32;
using VSO = u32;
using FSO = u32;
using TXO = u32;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
float Clamp(float input, float low, float high);

// Global camera
Camera g_camera(glm::vec3(0.0f, 0.0f, 3.0f));
float g_lastX;
float g_lastY;
bool g_firstMouse = true;

// Global timing
float g_deltaTime = 0.0f;
float g_lastFrame = 0.0f;

int main()
{
	const int VIEWPORT_WIDTH = 1280;
	const int VIEWPORT_HEIGHT = 720;

    float g_lastX = VIEWPORT_WIDTH / 2;
    float g_lastY = VIEWPORT_HEIGHT / 2;

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	//__________________________________________________________________________
	// INITIALISATION
	int nAttritubes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttritubes);
	std::cout << "Max vertex attributes supported: " << nAttritubes
	          << std::endl;

    // RENDER STATE
    glEnable(GL_DEPTH_TEST);


    // GEOMETRY
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
    const u32 NUM_TEXTURES = 2;
    TXO textures[NUM_TEXTURES];
    int widths[NUM_TEXTURES];
    int heights[NUM_TEXTURES];
    int nChannels[NUM_TEXTURES];
    stbi_set_flip_vertically_on_load(true);
    u8 *nicoTextureData = stbi_load("assets/nico.png", &widths[0], &heights[0], &nChannels[0], 0);
    u8 *nozomiTextureData = stbi_load("assets/nozomi_trans.png", &widths[1], &heights[1], &nChannels[1], 0);
    if (nicoTextureData && nozomiTextureData)
    {
        glGenTextures(NUM_TEXTURES, textures);

        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widths[0], heights[0], 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nicoTextureData);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widths[1], heights[1], 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nozomiTextureData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
    	std::cout << "Failed to load texture." << std::endl;
    }
    stbi_image_free(nicoTextureData);
    stbi_image_free(nozomiTextureData);

	// MAIN LOOP
    while (!glfwWindowShouldClose(window))
    {
    	// TIMING
    	float currentFrame = (float)glfwGetTime();
    	g_deltaTime = currentFrame - g_lastFrame;
    	g_lastFrame = currentFrame;
    	
    	// INPUT
        processInput(window);

		// RENDER
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // transform
        glm::mat4 model;
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
        
        glm::mat4 view;
        view = g_camera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(g_camera.Zoom),
                                      (float)VIEWPORT_WIDTH / VIEWPORT_HEIGHT, 0.1f, 100.0f);

		// rectangle
		shaderProgram.Use();
		shaderProgram.SetInt("texture0", 0);
		shaderProgram.SetInt("texture1", 1);
		shaderProgram.SetMatrix("model", model);
		shaderProgram.SetMatrix("view", view);
		shaderProgram.SetMatrix("projection", projection);
		
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glActiveTexture(GL_TEXTURE0); // activate the texture unit first
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		
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
	
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        g_camera.ProcessKeyboard(Camera_Movement::FORWARD, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        g_camera.ProcessKeyboard(Camera_Movement::BACKWARD, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        g_camera.ProcessKeyboard(Camera_Movement::LEFT, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        g_camera.ProcessKeyboard(Camera_Movement::RIGHT, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        g_camera.ProcessKeyboard(Camera_Movement::DOWN, g_deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        g_camera.ProcessKeyboard(Camera_Movement::UP, g_deltaTime);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (g_firstMouse) // this bool variable is initially set to true
    {
        g_lastX = (float)xpos;
        g_lastY = (float)ypos;
        g_firstMouse = false;
    }

	float xoffset = (float)xpos - g_lastX;
	float yoffset = (float)ypos - g_lastY;
	g_lastX = (float)xpos;
	g_lastY = (float)ypos;
	
	g_camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_camera.ProcessMouseScroll((float)yoffset);
}

float Clamp(float input, float low, float high)
{
    return glm::min(glm::max(low, input), high);
}
