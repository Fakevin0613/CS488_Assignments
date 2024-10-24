// Termm-Fall 2024

#include "JointNode.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "cs488-framework/MathUtils.hpp"
using namespace std;
using namespace glm;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
	current_x = m_joint_x.init;
	current_y = m_joint_y.init;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

float clamp(float current, float min, float max) {
  if (current > max) {
	return max;
  }
  if (current < min) {
	return min;
  }
  return current;
}

void JointNode::rotateX(float angle) {
    float newAngleX = clamp(current_x + angle, m_joint_x.min, m_joint_x.max);
    float rotationIncrementX = newAngleX - current_x;
    mat4 rot_matrix = glm::rotate(degreesToRadians(rotationIncrementX), vec3(1, 0, 0));
    current_x = newAngleX;
    trans = rot_matrix * trans;
}

void JointNode::rotateY(float angle) {
	if (isNeck) {
        float newAngleY = clamp(current_y + angle, m_joint_y.min, m_joint_y.max);
        float rotationIncrementY = newAngleY - current_y;
        mat4 rot_matrix = glm::rotate(degreesToRadians(rotationIncrementY), vec3(0, 1, 0));
        current_y = newAngleY;
        trans = rot_matrix * trans;
    }
}
