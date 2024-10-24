// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

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

private:
	Maze maze;
	Maze currentMaze;
	void dig();
	void reset();
	void initGrid();
	void initCube();
	void initFloor();
	glm::vec3 initPosition();
	void initAvatar();
	float getDeltaTime();

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to Cube geometry.
	GLuint m_cube_vao; // Vertex Array Object
	GLuint m_cube_vbo; // Vertex Buffer Object
	GLuint m_cube_ebo; // Element Buffer Object

	// Fields related to Floor geometry.
	GLuint m_floor_vao; // Vertex Array Object
	GLuint m_floor_vbo; // Vertex Buffer Object
	GLuint m_floor_ebo; // Element Buffer Object

	// Fields related to Avatar geometry.
	GLuint m_avatar_vao; // Vertex Array Object
	GLuint m_avatar_vbo; // Vertex Buffer Object
	GLuint m_avatar_ebo; // Element Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float colour[3];
	int current_col;

	// block, floor, avatar color
	glm::vec3 blockColor;
	glm::vec3 floorColor;
	glm::vec3 avatarColor;

	enum SelectedObject
	{
		BLOCK,
		FLOOR,
		AVATAR
	};

	float mazeScale;
	bool isDragging;
	double lastMouseX;
	float rotationAngleX;
	float rotationSpeedX;

	// avatar pos
	glm::vec3 avatar_position;
	glm::vec3 avatar_target_position;
	glm::vec3 avatar_current_position;
	bool is_moving;

	// wall height
	float wall_height;
};
