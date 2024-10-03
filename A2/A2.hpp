// Termm--Fall 2024

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();
	void initCube();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	glm::vec4 cubeVertices[8];
	int cubeEdges[12][2];
	glm::vec4 gnomonsVertices[4];
	int gonmonsEdges[3][2];
	enum Mode { TranslateModel, TranslateView, RotateModel, RotateView, ScaleModel, Perspective, Viewport, None };
	Mode mode = None;
	bool leftMousePressed;
	bool rightMousePressed;
	bool middleMousePressed;
	double previous_x;
	double previous_y;

	float near;
	float far;
	float fov;

	float viewportX1;
	float viewportY1;
	float viewportX2;
	float viewportY2;
	bool viewportMode;

	glm::mat4 view;
	glm::mat4 projection;

	glm::mat4 modelTransformation;  // Cube axis translation
    glm::mat4 viewTransformation;  // Eye axis translation
	glm::mat4 gnomonModelTransformation;
    glm::mat4 gnomonViewTransformation;

	glm::mat4 createView();
	glm::mat4 makeProjection();
	void translateModel(double xPos, double yPos);
  	void translateView(double xPos, double yPos);
	void rotateModel(double xPos, double yPos);
	void rotateView(double xPos, double yPos);
	void scaleModel(double xPos, double yPos);
	void perspective(double xPos, double yPos);
	void viewport(double xPos, double yPos);

	float clipLeft(glm::vec4& v);
	float clipRight(glm::vec4& v);
	float clipTop(glm::vec4& v);
	float clipBot(glm::vec4& v);
	float clipFront(glm::vec4& v);
	float clipBack(glm::vec4& v);
	bool clipping(glm::vec4& A, glm::vec4& B);
	glm::vec4 normalizeW(glm::vec4& v);
	void drawInViewPort(glm::vec2& A, glm::vec2& B);
	glm::vec2 mapToViewport(glm::vec2& v);

	void reset();
};
