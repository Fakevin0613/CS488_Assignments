// Termm-Fall 2024

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <queue>

using namespace glm;

static bool gui_hide = false;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),

	  mode(None),
	  trackball(true),
	  zBuffer(true),
	  backfaceCulling(false),
	  frontfaceCulling(false),
	  puppetTranslation(glm::mat4(1.0f)),
	  puppetRotation(glm::mat4(1.0f)),
	  picking(false),
	  isMouseDown(false),
	  isFirstClick(true)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

void A3::changePostion(double xPos, double yPos) {
	double xOffset = xPos - previous_x;
    double yOffset = yPos - previous_y;
	if (leftMousePressed) {
		puppetTranslation = translate(mat4(1.0f), vec3(xOffset * 0.01f, -yOffset * 0.01f, 0)) * puppetTranslation;
	}
	if (middleMousePressed) {
		puppetTranslation = translate(mat4(1.0f), vec3(0, 0, yOffset * 0.01f)) * puppetTranslation;
	}
	if (rightMousePressed) {
		float center_x = m_framebufferWidth / 2.0f;
		float center_y = m_framebufferHeight / 2.0f;
		float radius = std::min(m_framebufferWidth / 4.0f, m_framebufferHeight / 4.0f);

		vec3 trackball_position = vec3(xPos - center_x, center_y - yPos, 0.0f) /= radius;
		float xy_sqr = trackball_position.x * trackball_position.x + trackball_position.y * trackball_position.y;

		if (xy_sqr > 1) {
			// outside the circle
			trackball_position /= sqrt(xy_sqr);
		} else {
			// within the circle
			trackball_position.z = sqrt(1 - xy_sqr);
		}

		if (isFirstClick) {
			lastTrackball = trackball_position;
			isFirstClick = false;
			return;
		}

		float cosine = glm::clamp(dot(trackball_position, lastTrackball), -1.0f, 1.0f);
		float angle = acos(cosine);
		glm::vec3 axis = glm::cross(trackball_position, lastTrackball);
		if (axis != vec3(0.0f)) {
			axis = glm::normalize(axis);
			puppetRotation = glm::rotate(mat4(1.0f), angle, axis) * puppetRotation;
		}
		lastTrackball = trackball_position;
	}
	
	// cout << "puppetTranslation: " << puppetTranslation << endl;
	// cout << "puppetRotation: " << puppetRotation << endl;
}

void A3::changeJoints(double xPos, double yPos) {
	if (middleMousePressed) {
		for (JointNode* joint : selectedJoints) {
			joint->rotateX(yPos - previous_y);
		}
	}
	if (rightMousePressed) {
		for (JointNode* joint : selectedJoints) {
			joint->rotateY(xPos - previous_x);
		}
	}
}

void A3::leftPicking() {
	picking = true;

	// clear the screen color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw picking color
	uploadCommonSceneUniforms();
	CHECK_GL_ERRORS;
	draw();
	CHECK_GL_ERRORS;

	double xpos, ypos;
	glfwGetCursorPos( m_window, &xpos, &ypos );

	float color3[3];
	glReadPixels( (int) xpos, (int) m_windowHeight - ypos, 1, 1, GL_RGB, GL_FLOAT, color3);
	CHECK_GL_ERRORS;
	// cout << "color3: " << color3[0] << " " << color3[1] << " " << color3[2] << endl;

	unsigned int red = static_cast<unsigned int>(color3[0] * 255.0f);
	unsigned int green = static_cast<unsigned int>(color3[1] * 255.0f);
	unsigned int blue = static_cast<unsigned int>(color3[2] * 255.0f);
	unsigned int nodeID = red + (green << 8) + (blue << 16);

	// cout << "nodeID: " << nodeID << endl;
	
	std::queue<SceneNode*> container;
	container.push(m_rootNode.get());
	while (container.size() > 0) {
		SceneNode* node = container.front();
		container.pop();
		// cout << node->m_nodeId << endl;
		// bool temp = node->m_nodeType == NodeType::JointNode;
		// cout << temp<< endl;
		if (node->m_nodeId == (unsigned int) 5) {
			JointNode* jointNode = dynamic_cast<JointNode*>(node); 
			jointNode->isNeck = true;
		}
		for (SceneNode* child : node->children) {
			if (child->m_nodeId == nodeID) {
				if (node->m_nodeType == NodeType::JointNode) {
					child->isSelected = !child->isSelected;
					JointNode* jointNode = dynamic_cast<JointNode*>(node); 
					if (selectedJoints.find(jointNode) == selectedJoints.end()) {
						selectedJoints.insert(jointNode);
					} else {
						selectedJoints.erase(jointNode);
					}
					break;
				}
			}
			else{
				container.push(child);
			}
		}
	}

	cout << "Selected Joint: ";
	for (JointNode* joint : selectedJoints) {
		cout << joint->m_nodeId << " ";
	}
	cout << endl;
	

	picking = false;
}

void A3::resetJoints(SceneNode * node) {
	undo = std::stack<std::vector<JointNode*>>();
	redo = std::stack<std::vector<JointNode*>>();
	if (node->m_nodeType == NodeType::JointNode) {
		JointNode* jointNode = dynamic_cast<JointNode*>(node);
		jointNode->current_x = jointNode->m_joint_x.init;
		jointNode->current_y = jointNode->m_joint_y.init;
		jointNode->set_transform(mat4(1.0f));
		jointNode->rotateX(jointNode->current_x);
		jointNode->rotateY(jointNode->current_y);
		jointNode->isSelected = false;
	}
	for (SceneNode* child : node->children) {
		resetJoints(child);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(0.0f, 0.0f, -1.0f);
	m_light.rgbIntensity = vec3(0.5f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		if (picking) {
			glUniform1i(location, 1);
		} else {
			glUniform1i(location, 0);
		}
		CHECK_GL_ERRORS;

		// all lighting and materials are discarded when picking
		if(!picking) {
				//-- Set LightSource uniform for the scene:
			{
				location = m_shader.getUniformLocation("light.position");
				glUniform3fv(location, 1, value_ptr(m_light.position));
				location = m_shader.getUniformLocation("light.rgbIntensity");
				glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
				CHECK_GL_ERRORS;
			}

			//-- Set background light ambient intensity
			{
				location = m_shader.getUniformLocation("ambientIntensity");
				vec3 ambientIntensity(0.25f);
				glUniform3fv(location, 1, value_ptr(ambientIntensity));
				CHECK_GL_ERRORS;
			}
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( gui_hide ) {
		return;
	}

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
		// Create Button, and check if it was clicked:

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Application")) {
				if (ImGui::Button("Reset Position (keyboard shortcut I)")) {
					puppetTranslation = glm::mat4(1.0f);
				}
				if (ImGui::Button("Reset Orientation (O)")) {
					puppetRotation = glm::mat4(1.0f);
				}
				if (ImGui::Button("Reset Joints (S)")) {
					resetJoints(m_rootNode.get());
				}
				if (ImGui::Button("Reset All (A)")) {
					puppetTranslation = glm::mat4(1.0f);
					puppetRotation = glm::mat4(1.0f);
					resetJoints(m_rootNode.get());
				}
				if( ImGui::Button( "Quit Application" ) ) {
					glfwSetWindowShouldClose(m_window, GL_TRUE);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Undo (U)")) {
					// undo
				}

				if (ImGui::MenuItem("Redo (R)")) {
					// redo
				}
				ImGui::EndMenu();
			}

		
			if (ImGui::BeginMenu("Options")) {
				ImGui::Checkbox("Circle (C)", &trackball);
				ImGui::Checkbox("Z-buffer (Z)", &zBuffer);
				ImGui::Checkbox("Backface culling (B)", &backfaceCulling);
				ImGui::Checkbox("Frontface culling (F)", &frontfaceCulling);
				ImGui::EndMenu();
			}
			
			ImGui::EndMainMenuBar();
		}
		ImGui::RadioButton( "None", (int*)&mode, None);
		ImGui::RadioButton( "Position/Orientation (P)", (int*)&mode, POSITION);
		ImGui::RadioButton( "Joints (J)", (int*)&mode, JOINTS);
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
		const glm::mat4 & transformationMatrix,
		bool picking
) {
	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * transformationMatrix;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		if (picking) {
			int id = node.m_nodeId;
			float r = ((id & 0x000000FF) >>  0) / 255.0f;
			float g = ((id & 0x0000FF00) >>  8) / 255.0f;
			float b = ((id & 0x00FF0000) >> 16) / 255.0f;
			location = shader.getUniformLocation("pickingColour");
			glUniform3f( location, r, g, b );
			CHECK_GL_ERRORS;
		}
		else if (node.isSelected) {
			location = shader.getUniformLocation("material.ks");
			vec3 ks = node.material.ks;
			glUniform3fv(location, 1, value_ptr(ks));
			CHECK_GL_ERRORS;
		}
		else{
			location = shader.getUniformLocation("picking");
			glUniform1f(location, 0);
			//-- Set NormMatrix:
			location = shader.getUniformLocation("NormalMatrix");
			mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
			glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
			CHECK_GL_ERRORS;


			//-- Set Material values:
			location = shader.getUniformLocation("material.kd");
			vec3 kd = node.material.kd;
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.ks");
			vec3 ks = node.material.ks;
			glUniform3fv(location, 1, value_ptr(ks));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.shininess");
			float shininess = node.material.shininess;
			glUniform1f(location, shininess);
			CHECK_GL_ERRORS;
		}

		
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
	if (zBuffer) {
		glEnable(GL_DEPTH_TEST);
	}

	if (frontfaceCulling || backfaceCulling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT_AND_BACK);
	}
	else{
		if (backfaceCulling) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		if (frontfaceCulling) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}
	}
	
	glBindVertexArray(m_vao_meshData);
	renderSceneGraph(*m_rootNode, puppetTranslation *  puppetRotation * m_rootNode->get_transform());
	glBindVertexArray(0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (trackball) {
		renderArcCircle();
	}
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode &root, const mat4 &parentTransform) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	// glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
	mat4 currentTransform = parentTransform * root.get_transform();
	if (root.m_nodeType == NodeType::GeometryNode) {
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&root);

		updateShaderUniforms(m_shader, *geometryNode, m_view, currentTransform, picking);


		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}

	for (const SceneNode *childNode : root.children) {
        renderSceneGraph(*childNode, currentTransform);
    }

	// glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		switch(mode) {
			case POSITION:
				changePostion(xPos, yPos);
				previous_x = xPos;
				previous_y = yPos;
				eventHandled = true;
				break;
			case JOINTS:
				changeJoints(xPos, yPos);
				previous_x = xPos;
				previous_y = yPos;
				eventHandled = true;
				break;
			default:
				previous_x = xPos;
				previous_y = yPos;
				break;
		}
	}
	return true;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()){
		if (actions == GLFW_PRESS) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				leftMousePressed = true;
				if (mode == JOINTS) {
					leftPicking();
				}
				eventHandled = true;
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				middleMousePressed = true;
				eventHandled = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				rightMousePressed = true;
				isMouseDown = true;
    			isFirstClick = true;
				eventHandled = true;
			}
		}
		if (actions == GLFW_RELEASE) {
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				leftMousePressed = false;
				eventHandled = true;
			}
			if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
				middleMousePressed = false;
				eventHandled = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT) {
				rightMousePressed = false;
				isMouseDown = false;
				eventHandled = true;
			}
		}
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_A ) {
			puppetTranslation = glm::mat4(1.0f);
			puppetRotation = glm::mat4(1.0f);
			resetJoints(m_rootNode.get());
			eventHandled = true;
		}
		if (key == GLFW_KEY_S) {
			resetJoints(m_rootNode.get());
			eventHandled = true;
		}
		if (key == GLFW_KEY_I) {
			puppetTranslation = glm::mat4(1.0f);
			eventHandled = true;
		}
		if (key == GLFW_KEY_O) {
			puppetRotation = glm::mat4(1.0f);
			eventHandled = true;
		}


		if( key == GLFW_KEY_M ) {
			gui_hide = !gui_hide;
			eventHandled = true;
		}
		if( key == GLFW_KEY_C ) {
			trackball = !trackball;
			eventHandled = true;
		}
		if( key == GLFW_KEY_Z ) {
			zBuffer = !zBuffer;
			eventHandled = true;
		}
		if( key == GLFW_KEY_B ) {
			backfaceCulling = !backfaceCulling;
			eventHandled = true;
		}
		if( key == GLFW_KEY_F ) {
			frontfaceCulling = !frontfaceCulling;
			eventHandled = true;
		}
		if (key == GLFW_KEY_P) {
			mode = POSITION;
			cout << "mode: POSITION" << endl;
			eventHandled = true;
		}
		if (key == GLFW_KEY_J) {
			mode = JOINTS;
			cout << "mode: JOINTS" << endl;
			eventHandled = true;
		}
		if (key == GLFW_KEY_R) {
			// todo
			eventHandled = true;
		}
		if (key == GLFW_KEY_U) {
			// todo
			eventHandled = true;
		}
	}
	// Fill in with event handling code...
	return eventHandled;
}
