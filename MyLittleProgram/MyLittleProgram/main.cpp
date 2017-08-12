#include <iostream>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

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
	std::cout << "Max vertex attributes supported: " << nAttritubes << std::endl;


	// Triangles
	VAO triangleVao;
	{
		float triangleVertices[] = {
			-0.5f, -0.5f, 0.0f,
			-0.1f, -0.5f, 0.0f,
			-0.1f,  0.5f, 0.0f
		};

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
		float triangle2Vertices[] = {
			// positions		// colors
			0.1f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,
			0.1f,  0.5f, 0.0f,	0.0f, 0.0f, 1.0f
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
			0.5f,  0.5f, 0.0f,  // top right
			0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f,  0.5f, 0.0f   // top left 
		};
		u32 rectangleIndices[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
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
	SPO shaderProgram;
	SPO shaderProgram2;
	{// Vertex shader
		const char *vertexShaderSource = R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;

			void main()
			{
				gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
			})";
		VSO vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		int success;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}

		// Vertex shader 2
		const char *vertexShader2Source = R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec3 aColor; // vertex color

			out vec3 ourColor;

			void main()
			{
				gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
				ourColor = aColor;
			})";
		VSO vertexShader2;
		vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader2, 1, &vertexShader2Source, NULL);
		glCompileShader(vertexShader2);
		glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}

		// Fragment shader
		const char *fragmentShaderSource = R"(
			#version 330 core
			out vec4 FragColor;

			uniform vec4 ourColor;

			void main()
			{
				FragColor = ourColor;
			})";

		FSO fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}

		// Fragment shader 2
		const char *fragmentShaderSource2 = R"(
			#version 330 core
			in vec3 ourColor;
			out vec4 FragColor;

			void main()
			{
				FragColor = vec4(ourColor, 1.0f);
			})";

		FSO fragmentShader2;
		fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
		glCompileShader(fragmentShader2);
		glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}

		// Shader program
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glDeleteShader(vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glDeleteShader(fragmentShader);
		glLinkProgram(shaderProgram);
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
				<< infoLog << std::endl;
		}

		// Shader program 2
		shaderProgram2 = glCreateProgram();
		glAttachShader(shaderProgram2, vertexShader2);
		glDeleteShader(vertexShader2);
		glAttachShader(shaderProgram2, fragmentShader2);
		glDeleteShader(fragmentShader);
		glLinkProgram(shaderProgram2);
		glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(shaderProgram2, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
				<< infoLog << std::endl;
		}
	}

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
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		
		glUseProgram(shaderProgram);
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		glBindVertexArray(triangleVao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(shaderProgram2);
		glBindVertexArray(triangle2Vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//// rectangle
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glBindVertexArray(rectangleVao);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);



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