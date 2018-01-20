#pragma warning(disable:4996)

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <time.h>
#include "stb_image.h"
#include "Shader.h"
#include "PathOperation.h"
#include "Texture.h"

using namespace std;

HWND hWnd;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

float cursorX = 0;
float cursorY = 0;

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	//printf("Mouse position move to [%d:%d]\n", int(x), int(y));
	cursorX = (float)x;
	cursorY = (float)y;
	return;
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShaderToyGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, cursor_position_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	string initPath = getAppPath();
	vector<string> fileExtensions;
	fileExtensions.push_back("ps");
	auto filePath = getOpenFilePath(hWnd, ".\\", fileExtensions);
	if (filePath == "")
		return -1;
	Shader ourShader(filePath.c_str()); 

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // colors           // texture coords
		1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// load and create a texture 
	ToyTexture texture1;
	string imagePath = initPath + "\\Images\\Cloud.png";
	//string imagePath = initPath + "\\test.jpg";
	texture1.pixelFormat = GL_RGBA;
	texture1.mMagFilter = GL_NEAREST;
	texture1.mMinFiler = GL_NEAREST;
	texture1.init();
	texture1.setData(imagePath, true);
	texture1.setTexIndex(0);
	
	// tell OpenGL for each sampler to which texture unit it belongs to (only has to be done once)
	ourShader.use();
	ourShader.setInt("iChannel0", 0); // iChannel0 samples Texture0
	
	// Set Uniforms
	float resolution[3] = { SCR_WIDTH, SCR_HEIGHT, 0.0f};
	ourShader.setVec3("iResolution", resolution);


	// render loop
	float curTime = (float)clock();
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// render the triangle
		ourShader.use();

		// uniforms
		auto preTime = curTime;
		curTime = (float)clock();
		curTime = curTime / 1000.0f;
		ourShader.setFloat("iTime", curTime);

		auto deltaTime = curTime - preTime;
		ourShader.setFloat("iTimeDelta", deltaTime);

		float mouse[4] = { cursorX, cursorY, 0, 0 };
		ourShader.setVec4("iMouse", mouse);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		cout << "MOUSE DOWN" << endl;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}