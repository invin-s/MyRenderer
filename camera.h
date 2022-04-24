#pragma once
#include<Eigen/eigen>

#define PI 3.1415926

#define LEFT	0
#define RIGHT	1
#define UP		2
#define DOWN	3
#define FORWARD	4
#define BACK	5

struct Motion {
	Eigen::Vector2f orbit;
	Eigen::Vector2f pan;
	float dolly;
};

struct Camera
{
	typedef int Dir;

	Eigen::Vector3f position;
	Eigen::Vector3f target;
	Eigen::Vector3f up;
	
	Camera(const Eigen::Vector3f& p, const Eigen::Vector3f& t) :
		position(p), target(t), up({ 0,1,0 }) {}

	void camera_transform(const Eigen::Vector3f& p, const Eigen::Vector3f& t) {
		position = p, target = t;
	}
	void camera_orbit(const Motion& motion) {
		Eigen::Vector3f from_target = position - target;
		float radius = from_target.norm();
		float theta = atan2(from_target.x(), from_target.z());
		float phi = acos(from_target.y() / radius);

		theta += motion.orbit.x();
		phi += motion.orbit.y();

		position.x() = radius * sin(phi) * sin(theta);
		position.y() = radius * cos(phi);
		position.z() = radius * sin(phi) * cos(theta);

		target = (target - position).normalized();
	}

};

