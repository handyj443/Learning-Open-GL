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
Camera g_camera;
float g_lastX;
float g_lastY;
bool g_firstMouse = true;

// Global timing
float g_deltaTime = 0.0f;
float g_lastFrame = 0.0f;

// Light
glm::vec3 g_wLightPos(1.2f, 1.0f, 2.0f);
glm::vec3 g_lightCol(1.0f, 1.0f, 1.0f);

// Cube
float g_cubeVertices[] = 
{
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

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


    // CAMERA
    g_camera.wPosition = glm::vec3(0.0f, 0.0f, 6.0f);

    // GEOMETRY	

	// Cube vertex buffer
    VBO cubeVbo;
    glGenBuffers(1, &cubeVbo);

	VAO cubeVao;
	{
		// Bind vertex array object
		glGenVertexArrays(1, &cubeVao);
		glBindVertexArray(cubeVao);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_cubeVertices),
		             g_cubeVertices, GL_STATIC_DRAW);
		// Set our vertex attributes pointers
		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		                      (void *)0);
		glEnableVertexAttribArray(0);
		// Normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		                      (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// Texcoords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		                      (void *)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

	// Lamp
	VAO lampVao;
	{
		// Bind vertex array object
		glGenVertexArrays(1, &lampVao);
		glBindVertexArray(lampVao);

		// we only need to bind to the VBO, the container's VBO's data already contains the correct data.
		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		                      (void *)0);
		glEnableVertexAttribArray(0);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// SHADERS
	Shader lightingShader("shaders/lightingTex.vs", "shaders/lightingTex.fs");
    Shader lampShader("shaders/lamp.vs", "shaders/lamp.fs");

    // TEXTURES
    const u32 NUM_TEXTURES = 2;
    TXO textures[NUM_TEXTURES];
    u8 *textureData[NUM_TEXTURES];
    int widths[NUM_TEXTURES];
    int heights[NUM_TEXTURES];
    int nChannels[NUM_TEXTURES];
    stbi_set_flip_vertically_on_load(true);
    textureData[0] = stbi_load("assets/crate_dif.png", &widths[0], &heights[0], &nChannels[0], 0);
    textureData[1] = stbi_load("assets/crate_spec.png", &widths[1], &heights[1], &nChannels[1], 0);
    if (textureData[0] && textureData[1])
    {
        glGenTextures(NUM_TEXTURES, textures);

        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widths[0], heights[0], 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, textureData[0]);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widths[1], heights[1], 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, textureData[1]);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
    	std::cout << "Failed to load textures." << std::endl;
    }
    stbi_image_free(textureData[0]);
    stbi_image_free(textureData[1]);

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

        // camera
        glm::mat4 view;
        view = g_camera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(g_camera.Zoom),
                                      (float)VIEWPORT_WIDTH / VIEWPORT_HEIGHT, 0.1f, 100.0f);

		// cube
		lightingShader.Use();
		lightingShader.SetMat4("view", view);
		lightingShader.SetMat4("projection", projection);

        lightingShader.SetInt("material.diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        lightingShader.SetInt("material.specular", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        lightingShader.SetFloat("material.shininess", 32.0f);

        lightingShader.SetVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.SetVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
        lightingShader.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.SetVec3("light.position", view * glm::vec4(g_wLightPos, 1.0));

        lightingShader.SetVec3("wViewPos", g_camera.wPosition);
        glBindVertexArray(cubeVao);

        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.SetMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(cubeVao);

        // lamp
        glm::mat4 model;
        model = glm::translate(model, g_wLightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lampShader.Use();
        lampShader.SetMat4("model", model);
        lampShader.SetMat4("view", view);
        lampShader.SetMat4("projection", projection);
        lampShader.SetVec3("lightColor", g_lightCol);
        glBindVertexArray(lampVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVao);
	glDeleteVertexArrays(1, &lampVao);
	glDeleteBuffers(1, &cubeVbo);

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
