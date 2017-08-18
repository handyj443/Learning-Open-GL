#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "types.h"

#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"

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
glm::vec3 g_lightCol(1.0f, 0.4f, 0.0f);

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
    glm::vec3 pointLightWPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

	// Lamps
	VBO cubeVbo;
	glGenBuffers(1, &cubeVbo);
	VAO lampVao;
	{
		// Bind vertex array object
		glGenVertexArrays(1, &lampVao);
		glBindVertexArray(lampVao);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_cubeVertices),
					 g_cubeVertices, GL_STATIC_DRAW);
		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		                      (void *)0);
		glEnableVertexAttribArray(0);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Nanosuit
	Model nanosuit("assets/nanosuit/nanosuit.obj");

	// SHADERS
	Shader lightingShader("shaders/lighting3.vs", "shaders/lighting3.fs");
	Shader lampShader("shaders/lamp.vs", "shaders/lamp.fs");
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
									  (float)VIEWPORT_WIDTH / VIEWPORT_HEIGHT,
									  0.1f, 100.0f);

		lightingShader.Use();
		lightingShader.SetMat4("view", view);
		lightingShader.SetMat4("projection", projection);

        lightingShader.SetFloat("material.shininess", 32.0f);

        lightingShader.SetVec3("dirLight.vDirection", view * glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f));
        lightingShader.SetVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.SetVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
        lightingShader.SetVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

		for (u32 i = 0; i < 4; i++)
		{
			glm::vec3 pointLightVPosition
				= view * glm::vec4(pointLightWPositions[i], 1.0f);
			std::string number = std::to_string(i);
			lightingShader.SetVec3(
				("pointLights[" + number + "].vPosition").c_str(),
				pointLightVPosition);
			lightingShader.SetVec3(
				("pointLights[" + number + "].ambient").c_str(), 0.2f, 0.2f,
				0.2f);
			lightingShader.SetVec3(
				("pointLights[" + number + "].diffuse").c_str(), 0.5f, 0.5f,
				0.5f);
			lightingShader.SetVec3(
				("pointLights[" + number + "].specular").c_str(), 1.0f, 1.0f,
				1.0f);
			lightingShader.SetFloat(
				("pointLights[" + number + "].constant").c_str(), 1.0f);
			lightingShader.SetFloat(
				("pointLights[" + number + "].linear").c_str(), 0.09f);
			lightingShader.SetFloat(
				("pointLights[" + number + "].quadratic").c_str(), 0.032f);
		}

		lightingShader.SetVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.SetVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.SetVec3("spotLight.vPosition", 0.0f, 0.0f, 0.0f);
		lightingShader.SetVec3("spotLight.vDirection", 0.0f, 0.0f, -1.0f);
		lightingShader.SetFloat("spotLight.innerCutOff",
								glm::cos(glm::radians(12.5f)));
		lightingShader.SetFloat("spotLight.outerCutOff",
								glm::cos(glm::radians(17.5f)));
		lightingShader.SetFloat("spotLight.constant", 1.0f);
		lightingShader.SetFloat("spotLight.linear", 0.09f);
		lightingShader.SetFloat("spotLight.quadratic", 0.032f);

		// nanosuit

		glm::mat4 model;
		model = glm::scale(model, glm::vec3(0.2f));
		lightingShader.SetMat4("model", model);
		nanosuit.Draw(lightingShader);

		// lamps
		lampShader.Use();
		lampShader.SetMat4("view", view);
		lampShader.SetMat4("projection", projection);
		glBindVertexArray(lampVao);
		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 model;
			model = glm::translate(model, pointLightWPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lampShader.SetMat4("model", model);
			lampShader.SetVec3("lightColor", g_lightCol);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		//nanosuit.Draw(lampShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
