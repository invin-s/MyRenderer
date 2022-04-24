#pragma once
#include<Eigen/eigen>
#include<vector> 

struct Light {
	Light() = default;
	Light(const Eigen::Vector3f& p, const Eigen::Vector3f& i) :
		position(p), intensity(i) {}
	Eigen::Vector3f position;
	Eigen::Vector3f intensity;
};

struct VertexData
{
	VertexData() :
		screenCoords(0, 0, 0, 1), worldCoords(0, 0, 0, 1), normal(0, 0, 0),
		texCoords(0, 0), color(0, 0, 0) {}
	Eigen::Vector4f screenCoords;
	Eigen::Vector4f worldCoords;
	Eigen::Vector3f normal;
	Eigen::Vector2f texCoords;
	Eigen::Vector3f color;
};

struct IShader
{
	typedef Eigen::Vector3f Color;

	Eigen::Matrix4f model;
	Eigen::Matrix4f view;
	Eigen::Matrix4f projection;
	Eigen::Matrix4f viewport;
	std::vector<Light> lights;

	virtual VertexData vertex_shader(const VertexData& v) = 0;
	virtual	Eigen::Vector3f fragment_shader(const VertexData& v) = 0;
	virtual ~IShader() {};
};

struct Gouraud_shader : public IShader {
	VertexData vertex_shader(const VertexData& v);
	Color fragment_shader(const VertexData& v);
};

struct Blinn_phong_shader : public IShader {
	VertexData vertex_shader(const VertexData& v);
	Color fragment_shader(const VertexData& v);
};
