// Termm-Fall 2024

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();

	float current_x, current_y;
	bool isNeck = false;

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, max;
	};

	void rotateX(float angle);
	void rotateY(float angle);

	JointRange m_joint_x, m_joint_y;
};
