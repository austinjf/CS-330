#include <iostream>             // cout, cerr
#include <cstdlib>              // EXIT_FAILURE
#include <GL/glew.h>            // GLEW library
#include <GLFW/glfw3.h>         // GLFW library

// GL math
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// camera movement
#include "camera.h"

// objects and light shader
#include "shader.h"

// shapes
#include "plane.h"
#include "cube.h"
#include "cone.h"
#include "cylinder.h"
#include "torus.h"
#include "sphere.h"

// image processing (for textures)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
	const char* const WINDOW_TITLE = "CS 330 Final Project - Austin Fuchs"; // window title

	// Variables for window width and height
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	// VAOs and VBOs for objects
	struct GLMesh
	{
		GLuint vaoTable;
		GLuint vboTable;
		GLuint nVertices; // for table (2D plane)
		GLuint vaoCupcakeFrosting;
		GLuint vboCupcakeFrosting;
		GLuint vaoCupcakeCake;
		GLuint vboCupcakeCake;
		GLuint vaoDonut;
		GLuint vboDonut;
		GLuint vaoIceCreamBar;
		GLuint vboIceCreamBar;
		GLuint vaoIceCreamStick;
		GLuint vboIceCreamStick;
		GLuint vaoCottonCandyCart;
		GLuint vboCottonCandyCart;
		GLuint vaoCottonCandyBall;
		GLuint vboCottonCandyBall;
		GLuint vaoCottonCandyTire;
		GLuint vboCottonCandyTire;
		GLuint vaoCottonCandyTop;
		GLuint vboCottonCandyTop;
	};

	// Main GLFW window
	GLFWwindow* window = nullptr;
	// object mesh data
	GLMesh mesh;
	// Shader program
	GLuint programId;
	// textures
	GLuint tableTexture;
	GLuint cupcakeFrostingTexture;
	GLuint cupcakeCakeTexture;
	GLuint donutTexture;
	GLuint iceCreamBarTexture;
	GLuint iceCreamStickTexture;
	GLuint cottonCandyCartTexture;
	GLuint cottonCandyBallTexture;
	GLuint cottonCandyTireTexture;
	GLuint cottonCandyTopTexture;
	//glm::vec2 UVScale(5.0f, 5.0f);
	// 
	// light properties
	glm::vec3 objectColor(1.0f, 1.0f, 1.0f); // may not be needed?
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // white light
	glm::vec3 lightPosition(0.0f, 7.0f, 0.0f);
	glm::vec3 lightScale(0.5f);

	// camera
	Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
	float lastX = WINDOW_WIDTH / 2.0f;
	float lastY = WINDOW_HEIGHT / 2.0f;
	bool firstMouse = true;

	// perspective or orthographic projection
	bool isPerspective = true;

	// time between frames
	float deltaTime = 0.0f;
	float lastFrameTime = 0.0f;
}

// user defined methods
bool initOpenGL(GLFWwindow** window);
void resizeWindow(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mousePositionCallback(GLFWwindow* window, double xPos, double yPos);
void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void createMesh(GLMesh& mesh);
void createAndBindBuffers(GLuint vao, GLuint vbo);
void destroyMesh(GLMesh& mesh);
bool createTexture(const char* filepath, GLuint& textureId);
void destroyTexture(GLuint textureId);
void render(Shader objectShader, Shader lampShader);
bool createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource, GLuint& programId);
void destroyShaderProgram(GLuint programId);


// the only and only main method
int main() {

	if (!initOpenGL(&window))
		return EXIT_FAILURE;

	// create the mesh of objects
	createMesh(mesh);

	// initialize shader programs
	Shader objectShader("shaderfiles/object.vs", "shaderfiles/object.fs");
	Shader lampShader("shaderfiles/lamp.vs", "shaderfiles/lamp.fs");

	// load textures
	const char* tablePath = "images/table.jpg";
	if (!createTexture(tablePath, tableTexture))
		return EXIT_FAILURE;
	const char* cupcakeFrostingPath = "images/cupcake_frosting.jpg";
	if (!createTexture(cupcakeFrostingPath, cupcakeFrostingTexture))
		return EXIT_FAILURE;
	const char* cupcakeCakePath = "images/cupcake_cake.jpg";
	if (!createTexture(cupcakeCakePath, cupcakeCakeTexture))
		return EXIT_FAILURE;
	const char* donutPath = "images/donut.jpg";
	if (!createTexture(donutPath, donutTexture))
		return EXIT_FAILURE;
	const char* iceCreamBarPath = "images/ice_cream_bar.jpg";
	if (!createTexture(iceCreamBarPath, iceCreamBarTexture))
		return EXIT_FAILURE;
	const char* iceCreamStickPath = "images/ice_cream_stick.jpg";
	if (!createTexture(iceCreamStickPath, iceCreamStickTexture))
		return EXIT_FAILURE;
	const char* cottonCandyCartPath = "images/cotton_candy_cart.jpg";
	if (!createTexture(cottonCandyCartPath, cottonCandyCartTexture))
		return EXIT_FAILURE;
	const char* cottonCandyBallPath = "images/cotton_candy_ball.jpg";
	if (!createTexture(cottonCandyBallPath, cottonCandyBallTexture))
		return EXIT_FAILURE;
	const char* cottonCandyTirePath = "images/cotton_candy_tire.jpg";
	if (!createTexture(cottonCandyTirePath, cottonCandyTireTexture))
		return EXIT_FAILURE;
	const char* cottonCandyTopPath = "images/cotton_candy_top.jpg";
	if (!createTexture(cottonCandyTopPath, cottonCandyTopTexture))
		return EXIT_FAILURE;

	// set textures
	objectShader.use();
	objectShader.setInt("tableTexture", 0);
	objectShader.setInt("cupcakeFrostingTexture", 1);
	objectShader.setInt("cupcakeCakeTexture", 2);
	objectShader.setInt("donutTexture", 3);
	objectShader.setInt("iceCreamBarTexture", 4);
	objectShader.setInt("iceCreamStickTexture", 5);
	objectShader.setInt("cottonCandyCartTexture", 6);
	objectShader.setInt("cottonCandyBallTexture", 7);
	objectShader.setInt("cottonCandyTireTexture", 8);
	objectShader.setInt("cottonCandyTopTexture", 9);

	// Sets the background color of the window to black
	glClearColor(0.529f, 0.808f, 0.922f, 1.0f);

	// render loop - one frame per iteration
	while (!glfwWindowShouldClose(window)) {

		// time between frames
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		// keyboard/mouse inputs
		processInput(window);

		// render this frame
		render(objectShader, lampShader);

		glfwPollEvents();
	}

	// de-allocate textures
	destroyTexture(tableTexture);
	destroyTexture(cupcakeFrostingTexture);
	destroyTexture(cupcakeCakeTexture);
	destroyTexture(donutTexture);
	destroyTexture(iceCreamBarTexture);
	destroyTexture(iceCreamStickTexture);
	destroyTexture(cottonCandyCartTexture);
	destroyTexture(cottonCandyBallTexture);
	destroyTexture(cottonCandyTireTexture);
	destroyTexture(cottonCandyTopTexture);

	// de-allocate mesh data
	destroyMesh(mesh); //

	exit(EXIT_SUCCESS);
}


// Initialize GLFW, GLEW, and create a window
bool initOpenGL(GLFWwindow** window)
{
	// initialize glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (*window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, resizeWindow);
	glfwSetCursorPosCallback(*window, mousePositionCallback);
	glfwSetScrollCallback(*window, mouseScrollCallback);
	glfwSetMouseButtonCallback(*window, mouseButtonCallback);

	// capture mouse
	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//// initialize glew
	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	// Displays GPU OpenGL version
	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

	return true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void resizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// process all input - check glfw for keypresses this frame for camera movement
void processInput(GLFWwindow* window)
{
	// closes the window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// camera movement key bindings
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);

	// changes the projection from perspective to orthographic and vise visa in the render() method
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {

		if (isPerspective) {
			isPerspective = false;
		}
		else {
			isPerspective = true;
		}
	}

}

// glfw: callback for camera view whenever the mouse moves
void mousePositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse) {
		lastX = xPos;
		lastY = xPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// glfw: callback for camera zoom whenever the mouse wheel scrolls
void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.ProcessMouseScroll(yOffset);
}

// glfw: callback for mouse button events
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	// no functional requirements for mouse button events at this time
}

// create the mesh of triangles
void createMesh(GLMesh& mesh)
{
	// Vertex data for table
	GLfloat tableVertices[] = {
		// Vertex Positions    // texture	// normals
		-1.0f,  0.0f, -1.0f,   0.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		 1.0f,  0.0f, -1.0f,   1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		 1.0f,  0.0f,  1.0f,   1.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		 1.0f,  0.0f,  1.0f,   1.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		-1.0f,  0.0f,  1.0f,   0.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		-1.0f,  0.0f, -1.0f,   0.0f, 0.0f,	0.0f, 1.0f, 0.0f
	};

	const GLuint floatsPerVertex = 3;
	const GLuint floatsPerNormal = 3;
	const GLuint floatsPerTex = 2;

	// create table mesh
	mesh.nVertices = sizeof(tableVertices) / (sizeof(tableVertices[0]) * (floatsPerVertex + floatsPerNormal + floatsPerTex));
	glGenVertexArrays(1, &mesh.vaoTable);
	glGenBuffers(1, &mesh.vboTable);
	glBindVertexArray(mesh.vaoTable);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vboTable);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tableVertices), tableVertices, GL_STATIC_DRAW);

	// Strides between vertex coordinates
	GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerTex);

	// Create Vertex Attribute Pointers for vertices and textures
	glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, floatsPerTex, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerTex)));
	glEnableVertexAttribArray(2);

	createAndBindBuffers(mesh.vaoCupcakeFrosting, mesh.vboCupcakeFrosting);
	createAndBindBuffers(mesh.vaoCupcakeCake, mesh.vboCupcakeCake);
	createAndBindBuffers(mesh.vaoDonut, mesh.vboDonut);
	createAndBindBuffers(mesh.vaoCottonCandyBall, mesh.vboCottonCandyBall);
	createAndBindBuffers(mesh.vaoCottonCandyCart, mesh.vboCottonCandyCart);
	createAndBindBuffers(mesh.vaoCottonCandyTire, mesh.vboCottonCandyTire);
	createAndBindBuffers(mesh.vaoCottonCandyTop, mesh.vboCottonCandyTop);
	createAndBindBuffers(mesh.vaoIceCreamBar, mesh.vboIceCreamBar);
	createAndBindBuffers(mesh.vaoIceCreamStick, mesh.vboIceCreamStick);
}

void createAndBindBuffers(GLuint vao, GLuint vbo)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

// de-allocate the mesh data
void destroyMesh(GLMesh& mesh)
{
	glDeleteVertexArrays(1, &mesh.vaoTable);
	glDeleteBuffers(1, &mesh.vboTable);
	glDeleteVertexArrays(1, &mesh.vaoCupcakeFrosting);
	glDeleteBuffers(1, &mesh.vboCupcakeFrosting);
	glDeleteVertexArrays(1, &mesh.vaoCupcakeCake);
	glDeleteBuffers(1, &mesh.vboCupcakeCake);
	glDeleteVertexArrays(1, &mesh.vaoDonut);
	glDeleteBuffers(1, &mesh.vboDonut);
	glDeleteVertexArrays(1, &mesh.vaoCottonCandyBall);
	glDeleteBuffers(1, &mesh.vboCottonCandyBall);
	glDeleteVertexArrays(1, &mesh.vaoCottonCandyCart);
	glDeleteBuffers(1, &mesh.vboCottonCandyCart);
	glDeleteVertexArrays(1, &mesh.vaoCottonCandyTire);
	glDeleteBuffers(1, &mesh.vboCottonCandyTire);
	glDeleteVertexArrays(1, &mesh.vaoCottonCandyTop);
	glDeleteBuffers(1, &mesh.vboCottonCandyTop);
	glDeleteVertexArrays(1, &mesh.vaoIceCreamBar);
	glDeleteBuffers(1, &mesh.vboIceCreamBar);
	glDeleteVertexArrays(1, &mesh.vaoIceCreamStick);
	glDeleteBuffers(1, &mesh.vboIceCreamStick);
}

bool createTexture(const char* filepath, GLuint& textureId)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image = stbi_load(filepath, &width, &height, &channels, 0);

	if (image) {

		// generate texture and bind for use
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		// release image and texture from use
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0);

		// image loaded successfully
		return true;
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	// error loading image
	return false;
}

void destroyTexture(GLuint textureId)
{
	glGenTextures(1, &textureId);
}

// render a single frame
void render(Shader objectShader, Shader lampShader)
{
	// Enable z-depth
	glEnable(GL_DEPTH_TEST);

	// clear the frame and z buffers
	glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// scale the object
	glm::mat4 scale;
	// rotate the object
	glm::mat4 rotation;
	// translate the object
	glm::mat4 translation;
	// model matrix
	glm::mat4 model;

	// shader for all objects
	objectShader.use();

	// camera/view transformation
	glm::mat4 view = camera.GetViewMatrix();

	// Creates a perspective or ortho projection based on user input
	glm::mat4 projection;
	if (isPerspective) {
		projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	}
	else {
		projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	}

	// set shader program variables
	objectShader.setMat4("view", view);
	objectShader.setMat4("projection", projection);
	objectShader.setVec3("objectColor", objectColor.r, objectColor.g, objectColor.b);
	objectShader.setVec3("lightColor", lightColor.r, lightColor.g, lightColor.b);
	objectShader.setVec3("lightPos", lightPosition.x, lightPosition.y, lightPosition.z);
	const glm::vec3 cameraPosition = camera.Position;
	objectShader.setVec3("viewPosition", cameraPosition.x, cameraPosition.y, cameraPosition.z);
	//shader.setVec2("uvScale", UVScale);


	// TABLE (2D plane)
	glBindVertexArray(mesh.vaoTable);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tableTexture);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(7.0f, 1.0f, 7.0f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, -1.0f, 0.0f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create plane and render
	glDrawArrays(GL_TRIANGLES, 0, mesh.nVertices);


	// CUPCAKE - FROSTING (Cone)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cupcakeFrostingTexture);
	glBindVertexArray(mesh.vaoCupcakeFrosting);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, 0.9f, 3.5f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create cone and render
	static_meshes_3D::Cone cupcakeFrosting(1.25, 50, 1.25, true, true, true);
	cupcakeFrosting.render();


	// CUPCAKE - CAKE (Cylinder)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cupcakeCakeTexture);
	glBindVertexArray(mesh.vaoCupcakeCake);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, -0.35f, 3.5f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create cylinder and render
	static_meshes_3D::Cylinder cupcakeCake(1.25, 50, 1.25, true, true, true);
	cupcakeCake.render();


	// DONUT (Torus) 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, donutTexture);
	glBindVertexArray(mesh.vaoDonut);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	rotation = glm::rotate(1.5708f, glm::vec3(1.0f, 0.0f, 0.0f));
	translation = glm::translate(glm::vec3(0.0f, -0.5f, -3.5f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create torus and render
	static_meshes_3D::Torus donut(50, 50, 1, 0.5, true, true, true);
	donut.render();


	// ICE CREAM BAR (Cube) 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, iceCreamBarTexture);
	glBindVertexArray(mesh.vaoIceCreamBar);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(2.0f, 1.0f, 3.0f));
	rotation = glm::rotate(0.7854f, glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(3.5f, -0.495f, 0.0f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create cube and render
	static_meshes_3D::Cube iceCreamBar({ 1.0f, 1.0f, 1.0f, 1.0f }, true, true, true);
	iceCreamBar.render();


	// ICE CREAM STICK (Cube) 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, iceCreamStickTexture);
	glBindVertexArray(mesh.vaoIceCreamStick);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(2.0f / 3.0f, 1.0f / 3.0f, 3.0f / 3.0f)); // a third the size of the ice cream bar
	rotation = glm::rotate(0.7854f, glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(4.85f, -0.495f, 1.35f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create cube and render
	static_meshes_3D::Cube iceCreamStick({ 1.0f, 1.0f, 1.0f, 1.0f }, true, true, true);
	iceCreamStick.render();


	// COTTON CANDY CART (Cube)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cottonCandyCartTexture);
	glBindVertexArray(mesh.vaoCottonCandyCart);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(1.75f, 1.75f, 1.75f));
	rotation = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-3.5f, -0.120f, 0.0f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create cube and render
	static_meshes_3D::Cube cottonCandyCart({ 1.0f, 1.0f, 1.0f, 1.0f }, true, true, true);
	cottonCandyCart.render();


	// COTTON CANDY TIRE - FRONT (Cylinder)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cottonCandyTireTexture);
	glBindVertexArray(mesh.vaoCottonCandyTire);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
	rotation = glm::rotate(1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
	translation = glm::translate(glm::vec3(-2.56f, -0.5f, 0.75f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create cylinder and render
	static_meshes_3D::Cylinder cottonCandyCaryTireFront(2, 50, 0.5, true, true, true);
	cottonCandyCaryTireFront.render();

	// COTTON CANDY TIRE - BACK (Cylinder)
	// (using the same VAO, texture, scale, and rotation as the front tire
	translation = glm::translate(glm::vec3(-4.44f, -0.5f, 0.75f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create cylinder and render
	static_meshes_3D::Cylinder cottonCandyCaryTireBack(2, 50, 0.5, true, true, true);
	cottonCandyCaryTireBack.render();


	// COTTON CANDY BALL (Sphere)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cottonCandyBallTexture);
	glBindVertexArray(mesh.vaoCottonCandyBall);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	rotation = glm::rotate(3.14159f, glm::vec3(0.0f, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(-3.5f, 1.7f, 0.0f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create sphere and render
	static_meshes_3D::Sphere cottonCandyBall(1.25, 25, 25, true, true, true);
	cottonCandyBall.render();


	// COTTON CANDY TOP (Cylinder)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cottonCandyTopTexture);
	glBindVertexArray(mesh.vaoCottonCandyTop);
	//		scale/rotate/translate object
	scale = glm::scale(glm::vec3(0.25f, 0.25f, 0.25f));
	rotation = glm::rotate(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	translation = glm::translate(glm::vec3(-3.5f, 3.0f, 0.0f));
	model = translation * rotation * scale;
	//		set the model matrix
	objectShader.setMat4("model", model);
	//		create cylinder and render
	static_meshes_3D::Cylinder cottonCandyTop(1, 50, 1, true, true, true);
	cottonCandyTop.render();


	// LAMP (light source) 
	lampShader.use();
	glBindVertexArray(mesh.vaoTable);

	// transform and scale light to above all objects
	model = glm::translate(lightPosition) * glm::scale(lightScale);
	lampShader.setMat4("view", view);
	lampShader.setMat4("projection", projection);
	lampShader.setMat4("model", model);
	
	// draw the lamp (using the table object)
	glDrawArrays(GL_TRIANGLES, 0, mesh.nVertices);

	// Deactivate the Vertex Array Object
	glBindVertexArray(0);

	// glfw: swap buffers and poll IO events
	glfwSwapBuffers(window);
}

// create the color shading function between vertices
bool createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource, GLuint& programId)
{
	// Compilation and linkage error reporting
	int success = 0;
	char infoLog[512];

	// Create a Shader program object.
	programId = glCreateProgram();

	// Create the vertex and fragment shader objects
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Retrive the shader source
	glShaderSource(vertexShaderId, 1, &vertexShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fragmentShaderSource, NULL);

	// Compile the vertex shader, and print compilation errors (if any)
	glCompileShader(vertexShaderId); // compile the vertex shader
	// check for shader compile errors
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glCompileShader(fragmentShaderId); // compile the fragment shader
	// check for shader compile errors
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

		return false;
	}

	// Attached compiled shaders to the shader program
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);   // links the shader program
	// check for linking errors
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

		return false;
	}

	glUseProgram(programId);    // Uses the shader program

	return true;
}

// de-allocate the shader program data
void destroyShaderProgram(GLuint programId)
{
	glDeleteProgram(programId);
}