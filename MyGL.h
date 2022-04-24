#pragma once
#include<Eigen/eigen>

const float PI = 3.1415926;

Eigen::Matrix4f make_model(float alpha,float beta,float gamma,float scaling) {
	Eigen::Matrix4f rotation, scale, translate;
	alpha = alpha * PI / 180.0f;
	beta = beta * PI / 180.0f;
	gamma = gamma * PI / 180.0f;
	Eigen::Matrix4f rotationX, rotationY, rotationZ;
	rotationX <<	1, 0, 0, 0,
					0, cos(alpha), -sin(alpha), 0,
					0, sin(alpha), cos(alpha), 0,
					0, 0, 0, 1;
	rotationY <<    cos(beta), 0, sin(beta), 0,
					0, 1, 0, 0,
					-sin(beta), 0, cos(beta), 0,
					0, 0, 0, 1;
	rotationZ <<	cos(gamma), -sin(gamma), 0, 0,
					sin(gamma), cos(gamma), 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1;
	rotation = rotationX * rotationY * rotationZ;

	float k = scaling;
	scale <<	k, 0, 0, 0,
				0, k, 0, 0,
				0, 0, k, 0,
				0, 0, 0, 1;

	translate <<	1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0,
					0, 0, 0, 1;
	
	return translate * rotation * scale;
}

Eigen::Matrix4f make_view(const Eigen::Vector3f& e, const Eigen::Vector3f& g, const Eigen::Vector3f& t) {
	//e:eye_pos, g: gaze_dir, t: up_dir
	Eigen::Vector3f w = -g.normalized();
	Eigen::Vector3f u = t.cross(w).normalized();
	Eigen::Vector3f v = w.cross(u);

	Eigen::Matrix4f view;
	Eigen::Matrix4f rotation, translate;
	rotation <<	u.x(), u.y(), u.z(), 0,
				v.x(), v.y(), v.z(), 0,
				w.x(), w.y(), w.z(), 0,
				0, 0, 0, 1;

	translate <<1, 0, 0, -e.x(),
				0, 1, 0, -e.y(),
				0, 0, 1, -e.z(),
				0, 0, 0, 1;
	view = rotation*translate;

	return view;
}

Eigen::Matrix4f make_projection(float eye_fov, float aspect_ratio, float zNear, float zFar) {
	float angel = eye_fov * PI / 180;
	float left, right, bottom, top;
	top = std::abs(zNear) * tan(angel * 0.5), bottom = -top;
	left = -top * aspect_ratio, right = -left;
		
	Eigen::Matrix4f p2o;
	p2o<<	zNear, 0, 0, 0,
			0, zNear, 0, 0,
			0, 0, zNear + zFar, -zFar * zNear,
			0, 0, 1, 0;

	Eigen::Matrix4f orth, trans, scale;
	trans <<	1, 0, 0, -(right + left) * 0.5,
				0, 1, 0, -(top + bottom) * 0.5,
				0, 0, 1, -(zNear + zFar) * 0.5,
				0, 0, 0, 1;
	scale << 2 / (right - left), 0, 0, 0,
			0, 2 / (top - bottom), 0, 0,
			0, 0, 2 / (zNear - zFar), 0,
			0, 0, 0, 1;
	orth = scale * trans;

	Eigen::Matrix4f projection =  orth*p2o;
	return projection;
}

Eigen::Matrix4f make_viewport(int width,int height) {
	Eigen::Matrix4f viewport;
	viewport << width * 0.5, 0, 0, width * 0.5,
				0, height * 0.5, 0, height * 0.5,
				0, 0, 1, 0,
				0, 0, 0, 1;
	return viewport;
}

