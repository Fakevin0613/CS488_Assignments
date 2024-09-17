// Termm--Fall 2024

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;
static const float PI = 3.14159265f;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col(0), maze(DIM)
{
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed = getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;

	// DELETE FROM HERE...
	maze.digMaze();
	maze.printMaze();
	// ...TO HERE

	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath("VertexShader.vs").c_str());
	m_shader.attachFragmentShader(
		getAssetFilePath("FragmentShader.fs").c_str());
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation("P");
	V_uni = m_shader.getUniformLocation("V");
	M_uni = m_shader.getUniformLocation("M");
	col_uni = m_shader.getUniformLocation("colour");

	// init
	reset();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt(
		glm::vec3(0.0f, 2. * float(DIM) * 2.0 * M_SQRT1_2, float(DIM) * 2.0 * M_SQRT1_2),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	proj = glm::perspective(
		glm::radians(30.0f),
		float(m_framebufferWidth) / float(m_framebufferHeight),
		1.0f, 1000.0f);
}

void A1::reset(){
	blockColor = glm::vec3(1.0f, 0.0f, 0.0f);  // Red by default
	floorColor = glm::vec3(0.0f, 1.0f, 0.0f);  // Green by default
	avatarColor = glm::vec3(0.0f, 0.0f, 1.0f); // Blue by default
	wall_height = 2.0f;
	mazeScale = 1.0f;
	isDragging = false;
	lastMouseX = 0.0;
	rotationAngleX = 0.0f;
	rotationSpeedX = 0.0f;
	avatar_position = initPosition();
	initGrid();
	initCube();
	initFloor();
	initAvatar();
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM + 3);

	float *verts = new float[sz];
	size_t ct = 0;
	for (int idx = 0; idx < DIM + 3; ++idx)
	{
		verts[ct] = -1;
		verts[ct + 1] = 0;
		verts[ct + 2] = idx - 1;
		verts[ct + 3] = DIM + 1;
		verts[ct + 4] = 0;
		verts[ct + 5] = idx - 1;
		ct += 6;

		verts[ct] = idx - 1;
		verts[ct + 1] = 0;
		verts[ct + 2] = -1;
		verts[ct + 3] = idx - 1;
		verts[ct + 4] = 0;
		verts[ct + 5] = DIM + 1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays(1, &m_grid_vao);
	glBindVertexArray(m_grid_vao);

	// Create the cube vertex buffer
	glGenBuffers(1, &m_grid_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_grid_vbo);
	glBufferData(GL_ARRAY_BUFFER, sz * sizeof(float),
				 verts, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete[] verts;

	CHECK_GL_ERRORS;
}

void A1::initCube()
{
	float cube_vertices[] = {
		// Front
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, wall_height, 1.0f,
		0.0f, wall_height, 1.0f,
		// Back
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, wall_height, 0.0f,
		0.0f, wall_height, 0.0f};

	int cube_indices[] = {
		// Front
		0, 1, 2, 2, 3, 0,
		// Back
		4, 5, 6, 6, 7, 4,
		// Left
		4, 0, 3, 3, 7, 4,
		// Right
		1, 5, 6, 6, 2, 1,
		// Top
		3, 2, 6, 6, 7, 3,
		// Bottom
		4, 5, 1, 1, 0, 4};

	glGenVertexArrays(1, &m_cube_vao);
	glBindVertexArray(m_cube_vao);

	glGenBuffers(1, &m_cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_cube_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CHECK_GL_ERRORS;
}

void A1::initFloor()
{
	float floor_vertices[] = {
		0.0f,
		0.0f,
		1.0f,
		1.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
	};

	int floor_indices[] = {
		2, 3, 1, // First triangle
		1, 0, 2	 // Second triangle
	};

	glGenVertexArrays(1, &m_floor_vao);
	glBindVertexArray(m_floor_vao);

	glGenBuffers(1, &m_floor_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_floor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_floor_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_floor_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floor_indices), floor_indices, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CHECK_GL_ERRORS;
}

glm::vec3 A1::initPosition()
{
	for (int i = 0; i < DIM; i++)
	{
		for (int j = 0; j < DIM; j++)
		{
			if ((i == DIM - 1 || i == 0 || j == DIM - 1 || j == 0) && maze.getValue(i, j) == 0)
			{
				return glm::vec3((float)i, 0.0f, (float)j);
			}
		}
	}
	return glm::vec3(0.0f, 0.0f, 0.0f);
}

void A1::initAvatar()
{
	const int horizontal = 18;
	const int vertical = 18;
	const float r = 0.5f;

	int numVertices = (horizontal + 1) * (vertical + 1);
	int numIndices = horizontal * vertical * 6;

	float *vertices = new float[numVertices * 3];
	int *indices = new int[numIndices];

	int index = 0;
	for (int i = 0; i <= horizontal; i++)
	{
		float angle = PI * (float(i) / horizontal) - PI / 2.0f;
		float sinAngle = sin(angle);
		float cosAngle = cos(angle);

		for (int j = 0; j <= vertical; j++)
		{
			float phi = 2.0f * PI * (float(j) / vertical);
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float x = r * cosAngle * cosPhi;
			float y = r * sinAngle;
			float z = r * cosAngle * sinPhi;

			vertices[index++] = x;
			vertices[index++] = y;
			vertices[index++] = z;
		}
	}

	int indexIndex = 0;
	for (int i = 0; i < horizontal; i++)
	{
		for (int j = 0; j < vertical; j++)
		{
			int first = i * (vertical + 1) + j;
			int second = first + vertical + 1;

			// First triangle
			indices[indexIndex++] = first;
			indices[indexIndex++] = second;
			indices[indexIndex++] = first + 1;

			// Second triangle
			indices[indexIndex++] = second;
			indices[indexIndex++] = second + 1;
			indices[indexIndex++] = first + 1;
		}
	}

	// Step 4: Create VAO, VBO, and EBO
	glGenVertexArrays(1, &m_avatar_vao);
	glBindVertexArray(m_avatar_vao);

	// Create and bind VBO for vertex data
	glGenBuffers(1, &m_avatar_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_avatar_vbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

	// Create and bind EBO for index data
	glGenBuffers(1, &m_avatar_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_avatar_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(int), indices, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] vertices;
	delete[] indices;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
	if(!isDragging){
		rotationAngleX += rotationSpeedX;
		// rotationSpeedX *= 0.9f;
		// if (fabs(rotationSpeedX) < 0.03f){
		// 	rotationSpeedX = 0.0f;
		// }
		// rotationAngleY += rotationSpeedY;
		// rotationSpeedY *= 0.9f;
		// if (fabs(rotationSpeedY) < 0.03f){
		// 	rotationSpeedY = 0.0f;
		// }
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100, 100), opacity, windowFlags);
	if (ImGui::Button("Create New Game")){
		maze.digMaze();
		avatar_position = initPosition();
	}
	if (ImGui::Button("Reset"))
	{
		reset();
	}
	if (ImGui::Button("Quit Application"))
	{
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}

	// Eventually you'll create multiple colour widgets with
	// radio buttons.  If you use PushID/PopID to give them all
	// unique IDs, then ImGui will be able to keep them separate.
	// This is unnecessary with a single colour selector and
	// radio button, but I'm leaving it in as an example.

	// Prefixing a widget name with "##" keeps it from being
	// displayed.
	ImGui::PushID("Color Editor");
	static SelectedObject currentSelection = BLOCK;
	ImGui::RadioButton("Maze Block Color", (int*)&currentSelection, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Floor Color", (int*)&currentSelection, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Avatar Color", (int*)&currentSelection, 2);
	ImGui::PopID();
	
	if (currentSelection == BLOCK)
	{
		ImGui::ColorEdit3("Edit Color", glm::value_ptr(blockColor));
	}
	else if (currentSelection == FLOOR)
	{
		ImGui::ColorEdit3("Edit Color", glm::value_ptr(floorColor));
	}
	else if (currentSelection == AVATAR)
	{
		ImGui::ColorEdit3("Edit Color", glm::value_ptr(avatarColor));
	}
	

	// ImGui::PushID( 0 );
	// ImGui::ColorEdit3( "##Colour", colour );
	// ImGui::SameLine();
	// if( ImGui::RadioButton( "##Col", &current_col, 0 ) ) {
	// 	// Select this colour.
	// }
	// ImGui::PopID();

	/*
			// For convenience, you can uncomment this to show ImGui's massive
			// demonstration window right in your application.  Very handy for
			// browsing around to get the widget you want.  Then look in
			// shared/imgui/imgui_demo.cpp to see how it's done.
			if( ImGui::Button( "Test Window" ) ) {
				showTestWindow = !showTestWindow;
			}
	*/

	ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

	ImGui::End();

	if (showTestWindow)
	{
		ImGui::ShowTestWindow(&showTestWindow);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::scale(W, glm::vec3(mazeScale));
	W = glm::rotate(W, glm::radians(rotationAngleX), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotate around x-axis
	W = glm::translate(W, glm::vec3(-float(DIM) / 2.0f, 0, -float(DIM) / 2.0f));
	
	m_shader.enable();
	glEnable(GL_DEPTH_TEST);

	glUniformMatrix4fv(P_uni, 1, GL_FALSE, value_ptr(proj));
	glUniformMatrix4fv(V_uni, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));

	// Just draw the grid for now.
	glBindVertexArray(m_grid_vao);
	glUniform3f(col_uni, 1, 1, 1);
	glDrawArrays(GL_LINES, 0, (3 + DIM) * 4);

	// Draw Avatar
	mat4 originalW = W;
	W = glm::translate(W, avatar_position + 0.5f);
	glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));

	glBindVertexArray(m_avatar_vao);
	glUniform3f(col_uni, avatarColor.r, avatarColor.g, avatarColor.b);
	glDrawElements(GL_TRIANGLES, 18 * 18 * 2 * 3, GL_UNSIGNED_INT, 0);
	W = originalW;

	// Draw the cubes and floors
	for (int i = 0; i <= DIM + 1; i++)
	{
		for (int j = 0; j <= DIM + 1; j++)
		{
			W = glm::translate(W, glm::vec3(i - 1, 0, j - 1));
			glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));

			if (i == 0 || i == DIM + 1 || j == 0 || j == DIM + 1 || maze.getValue(i - 1, j - 1) == 0)
			{
				// draw floors
				glBindVertexArray(m_floor_vao);
				glUniform3f(col_uni, floorColor.r, floorColor.g, floorColor.b);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			else
			{
				// draw cubes
				glBindVertexArray(m_cube_vao);
				glUniform3f(col_uni, blockColor.r, blockColor.g, blockColor.b);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
			W = originalW;
		}
	}

	// Highlight the active square.
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent(
	int entered)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		if (isDragging) {
			double xChange = xPos - lastMouseX;
			rotationAngleX += (float) xChange * 0.1f;
			rotationSpeedX = xChange * 0.85f;
		}
		lastMouseX = xPos;
		eventHandled = true;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT){
			if (actions == GLFW_PRESS){
				isDragging = true;
				double xPos, yPos;
				glfwGetCursorPos(m_window, &xPos, &yPos);
				rotationSpeedX = 0.0f;
			}
			else if (actions == GLFW_RELEASE) {
            	isDragging = false;
        	}
		}
	}
	eventHandled = true;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet)
{
	bool eventHandled(false);

    if (yOffSet > 0) {
        mazeScale -= 0.1f;
    } else if (yOffSet < 0) {
        mazeScale += 0.1f;
    }
    mazeScale = glm::clamp(mazeScale, 0.4f, 2.0f);
	eventHandled = true;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods)
{
	bool eventHandled(false);

	// Fill in with event handling code...
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_Q)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_SPACE)
		{
			if (wall_height < 10.0f){
				wall_height += 0.1f;
				initCube();
				eventHandled = true;
			}
		}
		if (key == GLFW_KEY_BACKSPACE)
		{
			if (wall_height > 0.0f)
			{
				wall_height -= 0.1f;
				initCube();
				eventHandled = true;
			}
		}
		if (key == GLFW_KEY_UP)
		{
			if (avatar_position.z - 1 >= 0 && (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
			{
				maze.setValue(avatar_position.x, avatar_position.z - 1, 0);
				avatar_position.z -= 1;
				eventHandled = true;
			}
			else if (maze.getValue(avatar_position.x, avatar_position.z - 1) == 0 && avatar_position.z - 1 >= 0)
			{
				avatar_position.z -= 1;
				eventHandled = true;
			}
		}
		if (key == GLFW_KEY_DOWN)
		{
			if (avatar_position.z + 1 <= DIM - 1 && (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
			{
				maze.setValue(avatar_position.x, avatar_position.z + 1, 0);
				avatar_position.z += 1;
				eventHandled = true;
			}
			else if (maze.getValue(avatar_position.x, avatar_position.z + 1) == 0 && avatar_position.z + 1 <= DIM - 1)
			{
				avatar_position.z += 1;
				eventHandled = true;
			}
		}
		if (key == GLFW_KEY_LEFT)
		{
			if (avatar_position.x - 1 >= 0 && (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
			{
				maze.setValue(avatar_position.x - 1, avatar_position.z, 0);
				avatar_position.x -= 1;
				eventHandled = true;
			}
			else if (maze.getValue(avatar_position.x - 1, avatar_position.z) == 0 && avatar_position.x - 1 >= 0)
			{
				avatar_position.x -= 1;
				eventHandled = true;
			}
		}
		if (key == GLFW_KEY_RIGHT)
		{
			if (avatar_position.x + 1 <= DIM - 1 && (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
			{
				maze.setValue(avatar_position.x + 1, avatar_position.z, 0);
				avatar_position.x += 1;
				eventHandled = true;
			}
			else if (maze.getValue(avatar_position.x + 1, avatar_position.z) == 0 && avatar_position.x + 1 <= DIM - 1)
			{
				avatar_position.x += 1;
				eventHandled = true;
			}
		}

		if (key == GLFW_KEY_D) {
			maze.digMaze();
			avatar_position = initPosition();
			eventHandled = true;
		}

		if(key == GLFW_KEY_R){
			reset();
			eventHandled = true;
		}
	}

	return eventHandled;
}
