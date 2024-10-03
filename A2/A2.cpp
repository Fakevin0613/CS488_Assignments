// Termm--Fall 2024

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

const static float FACTOR = 0.01f; 
const static float SCALE = 1.03f;
float static const FOV_MIN = 5.0f;
float static const FOV_MAX = 160.0f;
float static const FOV_FACTOR = 0.1f;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

	initCube();
	reset();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Fall 2022
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

void A2::initCube()
{
	cubeVertices[0] = glm::vec4(-0.5f, -0.5f, -0.5f, 0.5f);
    cubeVertices[1] = glm::vec4(0.5f, -0.5f, -0.5f, 0.5f);
    cubeVertices[2] = glm::vec4(-0.5f, 0.5f, -0.5f, 0.5f);
    cubeVertices[3] = glm::vec4(0.5f, 0.5f, -0.5f, 0.5f);
    cubeVertices[4] = glm::vec4(-0.5f, -0.5f, 0.5f, 0.5f);
    cubeVertices[5] = glm::vec4(0.5f, -0.5f, 0.5f, 0.5f);
    cubeVertices[6] = glm::vec4(-0.5f, 0.5f, 0.5f, 0.5f);
    cubeVertices[7] = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);

    // Define the cube's 12 edges (pairs of vertex indices)
    cubeEdges[0][0] = 0; cubeEdges[0][1] = 1;
    cubeEdges[1][0] = 0; cubeEdges[1][1] = 2;
    cubeEdges[2][0] = 1; cubeEdges[2][1] = 3;
    cubeEdges[3][0] = 2; cubeEdges[3][1] = 3;

    cubeEdges[4][0] = 4; cubeEdges[4][1] = 5;
    cubeEdges[5][0] = 4; cubeEdges[5][1] = 6;
    cubeEdges[6][0] = 5; cubeEdges[6][1] = 7;
    cubeEdges[7][0] = 6; cubeEdges[7][1] = 7;

    cubeEdges[8][0] = 0; cubeEdges[8][1] = 4;
    cubeEdges[9][0] = 1; cubeEdges[9][1] = 5;
    cubeEdges[10][0] = 2; cubeEdges[10][1] = 6;
    cubeEdges[11][0] = 3; cubeEdges[11][1] = 7;

	gnomonsVertices[0] = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
	gnomonsVertices[1] = glm::vec4(0.5f, 0.0f, 0.0f, 0.5f);
	gnomonsVertices[2] = glm::vec4(0.0f, 0.5f, 0.0f, 0.5f);
	gnomonsVertices[3] = glm::vec4(0.0f, 0.0f, 0.5f, 0.5f);

	gonmonsEdges[0][0] = 0; gonmonsEdges[0][1] = 1;
	gonmonsEdges[1][0] = 0; gonmonsEdges[1][1] = 2;
	gonmonsEdges[2][0] = 0; gonmonsEdges[2][1] = 3;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

glm::mat4 A2::createView() {
	glm::mat4 T(
		1.0f, 0.0f, 0.0f ,0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 15.0f, 1.0f
	);
	// cout << T << endl;
	return T;
}

glm::mat4 A2::makeProjection() {
	float aspect = (float) (std::abs(viewportX1 - viewportX2)) / (float)(std::abs(viewportY1 - viewportY2));
	float theta = glm::radians(fov);
	float cot = std::cos(theta/2) / std::sin(theta/2);
	glm::mat4 Projection(
		cot / aspect, 0.0f, 0.0f ,0.0f,
		0.0f, cot, 0.0f, 0.0f,
		0.0f, 0.0f, (far + near) / (far - near), 1.0f,
		0.0f, 0.0f, -2 * far * near / (far - near), 0.0f
	);

	return Projection;
}

void A2::reset() {
	modelTransformation = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 2.0f, 2.0f, 1.0f
		);
	viewTransformation = glm::mat4(1.0f);
	gnomonModelTransformation = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 2.0f, 2.0f, 1.0f
		);
	gnomonViewTransformation = glm::mat4(1.0f);
	scaleTransformation  = glm::mat4(1.0f);
	mode = RotateModel;
	near = 5.0f;
	far = 20.f;
	fov = 30.0f;
	viewportX1 = 0.05 * m_framebufferWidth;
	viewportY1 = 0.05 * m_framebufferHeight;
	viewportX2 = 0.95 * m_framebufferWidth;
	viewportY2 = 0.95 * m_framebufferHeight;
	viewportMode = false;

	view = createView();
	projection = makeProjection();
	
}

void A2::translateModel(double xPos, double yPos) {
	// float xOffset = xPos - previous_x;
    // float yOffset = yPos - previous_y;
    // float distance = sqrt((xOffset * xOffset) + (yOffset * yOffset));
    // distance = (xOffset > 0) ? distance : -distance;
	float distance = xPos - previous_x;
	if (leftMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			distance * FACTOR, 0.0f, 0.0f, 1.0f
		);
		modelTransformation = modelTransformation * temp;
		gnomonModelTransformation = gnomonModelTransformation * temp;

	}
	if (middleMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, distance * FACTOR, 0.0f, 1.0f
		);
		modelTransformation = modelTransformation * temp;
		gnomonModelTransformation = gnomonModelTransformation * temp;
	}
	if (rightMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, distance * FACTOR, 1.0f
		);
		modelTransformation = modelTransformation * temp;
		gnomonModelTransformation = gnomonModelTransformation * temp;
	}
	previous_x = xPos;
    previous_y = yPos;
}

void A2::translateView(double xPos, double yPos) {
    // float xOffset = xPos - previous_x;
    // float yOffset = yPos - previous_y;
    // float distance = sqrt((xOffset * xOffset) + (yOffset * yOffset));
    // distance = (xOffset > 0) ? distance : -distance;
	float distance = xPos - previous_x;
    if (leftMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			distance * FACTOR, 0.0f, 0.0f, 1.0f
		);
		viewTransformation = glm::mat4(1.0f) / temp * viewTransformation;
		gnomonViewTransformation = glm::mat4(1.0f) / temp * gnomonViewTransformation;
	}
	if (middleMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, distance * FACTOR, 0.0f, 1.0f
		);
		viewTransformation = glm::mat4(1.0f) / temp * viewTransformation;
		gnomonViewTransformation = glm::mat4(1.0f) / temp * gnomonViewTransformation;
	}
	if (rightMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, distance * FACTOR, 1.0f
		);
		viewTransformation = glm::mat4(1.0f) / temp * viewTransformation;
		gnomonViewTransformation = glm::mat4(1.0f) / temp * gnomonViewTransformation;
	}
    previous_x = xPos;
    previous_y = yPos;
}

void A2::rotateModel(double xPos, double yPos){
	// float xOffset = xPos - previous_x;
    // float yOffset = yPos - previous_y;
    // float distance = sqrt((xOffset * xOffset) + (yOffset * yOffset));
    // distance = (xOffset > 0) ? distance : -distance;
	float distance = xPos - previous_x;
	double theta = glm::radians(distance);

	if (leftMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, std::cos(theta), -std::sin(theta), 0.0f,
			0.0f, std::sin(theta), std::cos(theta), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		modelTransformation = modelTransformation * temp;
		gnomonModelTransformation = gnomonModelTransformation * temp;
	}
	if (middleMousePressed){
		glm::mat4 temp(
			std::cos(theta), 0.0f, std::sin(theta), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-std::sin(theta), 0.0f, std::cos(theta), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		modelTransformation = modelTransformation * temp;
		gnomonModelTransformation = gnomonModelTransformation * temp;
	}
	if (rightMousePressed){
		glm::mat4 temp(
			std::cos(theta), -std::sin(theta), 0.0f, 0.0f,
			std::sin(theta), std::cos(theta), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		modelTransformation = modelTransformation * temp;
		gnomonModelTransformation = gnomonModelTransformation * temp;
	}
    previous_x = xPos;
    previous_y = yPos;
}

void A2::rotateView(double xPos, double yPos){
	// float xOffset = xPos - previous_x;
    // float yOffset = yPos - previous_y;
    // float distance = sqrt((xOffset * xOffset) + (yOffset * yOffset));
    // distance = (xOffset > 0) ? distance : -distance;
	float distance = xPos - previous_x;
	double theta = glm::radians(distance);

	if (leftMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, std::cos(theta), -std::sin(theta), 0.0f,
			0.0f, std::sin(theta), std::cos(theta), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		viewTransformation = glm::mat4(1.0f) / temp * viewTransformation;
		gnomonViewTransformation = glm::mat4(1.0f) / temp * gnomonViewTransformation;
	}
	if (middleMousePressed){
		glm::mat4 temp(
			std::cos(theta), 0.0f, std::sin(theta), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-std::sin(theta), 0.0f, std::cos(theta), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		viewTransformation = glm::mat4(1.0f) / temp * viewTransformation;
		gnomonViewTransformation = glm::mat4(1.0f) / temp * gnomonViewTransformation;
	}
	if (rightMousePressed){
		glm::mat4 temp(
			std::cos(theta), -std::sin(theta), 0.0f, 0.0f,
			std::sin(theta), std::cos(theta), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		viewTransformation = glm::mat4(1.0f) / temp * viewTransformation;
		gnomonViewTransformation = glm::mat4(1.0f) / temp * gnomonViewTransformation;
	}
    previous_x = xPos;
    previous_y = yPos;
}

void A2::scaleModel(double xPos, double yPos){	
	// float xOffset = xPos - previous_x;
    // float yOffset = yPos - previous_y;
    // float distance = sqrt((xOffset * xOffset) + (yOffset * yOffset));
    // distance = (xOffset > 0) ? distance : -distance;
	float distance = xPos - previous_x;
    float multipler = (distance > 0) ? SCALE : 1 / SCALE;

	if (leftMousePressed){
		glm::mat4 temp(
			multipler, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		scaleTransformation = scaleTransformation * temp;
	}
	if (middleMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, multipler, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		scaleTransformation = scaleTransformation * temp;
	}
	if (rightMousePressed){
		glm::mat4 temp(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, multipler, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		scaleTransformation = scaleTransformation * temp;
	}

	previous_x = xPos;
	previous_y = yPos;

}

void A2::perspective(double xPos, double yPos) {
	// float xOffset = xPos - previous_x;
    // float yOffset = yPos - previous_y;
    // float distance = sqrt((xOffset * xOffset) + (yOffset * yOffset));
    // distance = (xOffset > 0) ? distance : -distance;
	float distance = xPos - previous_x;

	if (leftMousePressed){
		fov += distance * FOV_FACTOR;
		fov = fov > FOV_MAX ? FOV_MAX : fov;
		fov = fov < FOV_MIN ? FOV_MIN : fov;
		projection = makeProjection();
	}
	if (middleMousePressed){
		near += distance * FACTOR;
		near = near > far ? far : near;
		if (near < 0.0f) near = 0.0f;
		projection = makeProjection();
	}
	if (rightMousePressed){
		far += distance * FACTOR;
		far = far < near ? near : far;
		projection = makeProjection();
	}
	previous_x = xPos;
    previous_y = yPos;
}

void A2::viewport(double xPos,double yPos) {
	if (leftMousePressed) {
		if (viewportMode) {
			viewportX1 = glm::clamp((float)xPos, 0.0f, (float)m_framebufferWidth);
			viewportY1 = glm::clamp((float)yPos, 0.0f, (float)m_framebufferHeight);
			viewportMode = false; // switch to the other corner
		} else {
			viewportX2 = glm::clamp((float)xPos, 0.0f, (float)m_framebufferWidth);
			viewportY2 = glm::clamp((float)yPos, 0.0f, (float)m_framebufferHeight);
		}
	}
	previous_x = xPos;
    previous_y = yPos;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */

glm::vec4 A2::normalizeW(glm::vec4& v) {
	return glm::vec4(v.x / v.w, v.y / v.w, v.z / v.w, 1.0f);
}

float A2::clipLeft(glm::vec4& v) {
	return v.x + v.w;
}

float A2::clipRight(glm::vec4& v) {
	return v.w - v.x;
}

float A2::clipTop(glm::vec4& v) {
	return v.w - v.y;
}

float A2::clipBot(glm::vec4& v) {
	return v.y + v.w;
}

float A2::clipFront(glm::vec4& v) {
	return v.z + v.w;
}

float A2::clipBack(glm::vec4& v) {
	return v.w - v.z;
}

bool A2::clipping(glm::vec4& A, glm::vec4& B) {
    typedef float (A2::*BoundaryFunc)(glm::vec4& v);

    BoundaryFunc clippingBoundaries[] = {
        &A2::clipLeft,
        &A2::clipRight,
        &A2::clipBot,
        &A2::clipTop,
        &A2::clipFront,
        &A2::clipBack
    };

    for (int i = 0; i < 6; ++i) {
        BoundaryFunc clippingBoundary = clippingBoundaries[i];
        float boundaryA = (this->*clippingBoundary)(A);
        float boundaryB = (this->*clippingBoundary)(B);
        if (boundaryA < 0 && boundaryB < 0) {
            return false;
        }

        if (boundaryA >= 0 && boundaryB >= 0) {
            continue;
        }
        float interpolation = boundaryA / (boundaryA - boundaryB);

        if (boundaryA >= 0) {
            B = (1 - interpolation) * A + interpolation * B;
        } else {
            A = (1 - interpolation) * A + interpolation * B;
        }
    }
    return true;
}

void A2::drawInViewPort(glm::vec2& A, glm::vec2& B) {
	drawLine(mapToViewport(A), mapToViewport(B));
}

glm::vec2 A2::mapToViewport(glm::vec2 & v) {
    float vleft = (std::min(viewportX1, viewportX2) - m_framebufferWidth / 2) / (m_framebufferWidth / 2);
	float vright = (std::max(viewportX1, viewportX2) - m_framebufferWidth / 2) / (m_framebufferWidth / 2);
	float vtop = (m_framebufferHeight / 2 - std::min(viewportY1, viewportY2)) / (m_framebufferHeight / 2);
	float vbot = (m_framebufferHeight / 2 - std::max(viewportY1, viewportY2)) / (m_framebufferHeight / 2);

    glm::vec2 mappedV(
        ((vright - vleft) / 2.0f) * (v.x + 1.0f) + vleft,
        ((vtop - vbot) / 2.0f) * (v.y + 1.0f) + vbot
    );

    return mappedV;
}

void A2::appLogic()
{
	// Place per frame, application logic here ...
	// Call at the beginning of frame, before drawing lines:
	initLineData();
    glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);

	// Draw the cube

	float vleft = (std::min(viewportX1, viewportX2) - m_framebufferWidth / 2) / (m_framebufferWidth / 2);
	float vright = (std::max(viewportX1, viewportX2) - m_framebufferWidth / 2) / (m_framebufferWidth / 2);
	float vtop = (m_framebufferHeight / 2 - std::min(viewportY1, viewportY2)) / (m_framebufferHeight / 2);
	float vbot = (m_framebufferHeight / 2 - std::max(viewportY1, viewportY2)) / (m_framebufferHeight / 2);

    for (int i = 0; i < 12; i++) {
        glm::vec4 v1 = cubeVertices[cubeEdges[i][0]];
        glm::vec4 v2 = cubeVertices[cubeEdges[i][1]];
		v1 = projection * viewTransformation * view * mat4(1.0f) * modelTransformation * scaleTransformation * v1;
		v2 = projection * viewTransformation * view * mat4(1.0f) * modelTransformation * scaleTransformation * v2;

		setLineColour(white);

		if (clipping(v1, v2)) {
			v1 = normalizeW(v1);
			v2 = normalizeW(v2);
			glm::vec2 v1_proj = glm::vec2(v1.x, v1.y);
			glm::vec2 v2_proj = glm::vec2(v2.x, v2.y);
			drawInViewPort(v1_proj, v2_proj);
		}
    }

	for (int i = 0; i < 3; i++) {
		glm::vec4 v1 = gnomonsVertices[gonmonsEdges[i][0]];
		glm::vec4 v2 = gnomonsVertices[gonmonsEdges[i][1]];
		v1 = projection * viewTransformation * view * mat4(1.0f) * gnomonModelTransformation * v1;
		v2 = projection * viewTransformation * view * mat4(1.0f) * gnomonModelTransformation * v2;

		if (i == 0) {
			setLineColour(red);
		} else if (i == 1) {
			setLineColour(green);
		} else {
			setLineColour(blue);
		}

		if (clipping(v1, v2)) {
			v1 = normalizeW(v1);
			v2 = normalizeW(v2);
			glm::vec2 v1_proj = glm::vec2(v1.x, v1.y);
			glm::vec2 v2_proj = glm::vec2(v2.x, v2.y);
			drawInViewPort(v1_proj, v2_proj);
		}
	}

	for (int i = 0; i < 3; i++) {
		glm::vec4 v1 = gnomonsVertices[gonmonsEdges[i][0]];
		glm::vec4 v2 = gnomonsVertices[gonmonsEdges[i][1]];
		v1 = projection * gnomonViewTransformation * view * mat4(1.0f) * v1;
		v2 = projection * gnomonViewTransformation * view * mat4(1.0f) * v2;

		if (i == 0) {
			setLineColour(red);
		} else if (i == 1) {
			setLineColour(green);
		} else {
			setLineColour(blue);
		}

		if (clipping(v1, v2)) {
			v1 = normalizeW(v1);
			v2 = normalizeW(v2);
			glm::vec2 v1_proj = glm::vec2(v1.x, v1.y);
			glm::vec2 v2_proj = glm::vec2(v2.x, v2.y);
			drawInViewPort(v1_proj, v2_proj);
		}
	}

	// draw viewport boundaries
	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	// cout << viewportX1 << " " << viewportX2 << " " << viewportY1 << " " << viewportY2 << endl;
	// cout << vleft << " " << vright << " " << vtop << " " << vbot << endl;
	// cout << modelTransformation << endl;
	drawLine(glm::vec2(vleft, vtop), glm::vec2(vleft, vbot));
	drawLine(glm::vec2(vleft, vtop), glm::vec2(vright, vtop));
	drawLine(glm::vec2(vright, vbot), glm::vec2(vleft, vbot));
	drawLine(glm::vec2(vright, vbot), glm::vec2(vright, vtop));

	// // Draw outer square:
	// setLineColour(vec3(1.0f, 0.7f, 0.8f));
	// drawLine(vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f));
	// drawLine(vec2(0.5f, -0.5f), vec2(0.5f, 0.5f));
	// drawLine(vec2(0.5f, 0.5f), vec2(-0.5f, 0.5f));
	// drawLine(vec2(-0.5f, 0.5f), vec2(-0.5f, -0.5f));


	// // Draw inner square:
	// setLineColour(vec3(0.2f, 1.0f, 1.0f));
	// drawLine(vec2(-0.25f, -0.25f), vec2(0.25f, -0.25f));
	// drawLine(vec2(0.25f, -0.25f), vec2(0.25f, 0.25f));
	// drawLine(vec2(0.25f, 0.25f), vec2(-0.25f, 0.25f));
	// drawLine(vec2(-0.25f, 0.25f), vec2(-0.25f, -0.25f));
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...
		ImGui::RadioButton("Translate Model (T)", (int*)&mode, TranslateModel);
		ImGui::RadioButton("Translate View (E)", (int*)&mode, TranslateView);
		ImGui::RadioButton("Rotate Model (R)", (int*)&mode, RotateModel);
		ImGui::RadioButton("Rotate View (O)", (int*)&mode, RotateView);
		ImGui::RadioButton("Scale Model (S)", (int*)&mode, ScaleModel);
		ImGui::RadioButton("Perspective (P)", (int*)&mode, Perspective);
		ImGui::RadioButton("Viewport (V)", (int*)&mode, Viewport);
		
		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application (Q)" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if( ImGui::Button( "Reset (A)" ) ) {
			reset();
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
		ImGui::Text( "Near: %.1f, Far: %.1f", near, far);
		ImGui::Text( "FOV: %.1f", fov);

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
	
	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		switch(mode) {
			case TranslateModel:
				translateModel(xPos, yPos);
				break;
			case TranslateView:
				translateView(xPos, yPos);
				break;
			case RotateModel:
				rotateModel(xPos, yPos);
				break;
			case RotateView:
				rotateView(xPos, yPos);
				break;
			case ScaleModel:
				scaleModel(xPos, yPos);
				break;
			case Perspective:
				perspective(xPos, yPos);
				break;
			case Viewport:
				viewport(xPos, yPos);
				break;
			default:
				previous_x = xPos;
				previous_y = yPos;
				break;
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if(!ImGui::IsMouseHoveringAnyWindow()) {
		if (actions == GLFW_PRESS) {
			switch(button) {
				case GLFW_MOUSE_BUTTON_LEFT:
					leftMousePressed = true;
					if (mode == Viewport) {
						viewportMode = true;
					}
					break;
				case GLFW_MOUSE_BUTTON_MIDDLE:
					middleMousePressed = true;
					break;
				case GLFW_MOUSE_BUTTON_RIGHT:
					rightMousePressed = true;
					break;
				default:
					break;
			}
		}
		if (actions == GLFW_RELEASE) {
			switch(button) {
				case GLFW_MOUSE_BUTTON_LEFT:
					leftMousePressed = false;
					break;
				case GLFW_MOUSE_BUTTON_MIDDLE:
					middleMousePressed = false;
					break;
				case GLFW_MOUSE_BUTTON_RIGHT:
					rightMousePressed = false;
					break;
				default:
					break;
			}
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_Q)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if(key == GLFW_KEY_A) {
			reset();
			eventHandled = true;
		}
		if (key == GLFW_KEY_E) {
			mode = TranslateView;
			cout << "mode: " << "TranslateView" << endl;
			eventHandled = true;
		}
		if (key == GLFW_KEY_T) {
			mode = TranslateModel;
			cout << "mode: " << "TranslateModel" << endl;
			eventHandled = true;
		}
		if (key == GLFW_KEY_R) {
			mode = RotateModel;
			cout << "mode: " << "RotateModel" << endl;
			eventHandled = true;
		}
		if (key == GLFW_KEY_O) {
			mode = RotateView;
			cout << "mode: " << "RotateView" << endl;
			eventHandled = true;
		}
		if (key == GLFW_KEY_S) {
			mode = ScaleModel;
			cout << "mode: " << "ScaleModel" << endl;
			eventHandled = true;
		}
		if (key == GLFW_KEY_P) {
			mode = Perspective;
			cout << "mode: " << "Perspective" << endl;
			eventHandled = true;
		}
		if (key == GLFW_KEY_V) {
			mode = Viewport;
			cout << "mode: " << "Viewport" << endl;
			eventHandled = true;
		}
	}

	return eventHandled;
}
