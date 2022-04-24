#include "Shader.h"

VertexData Gouraud_shader::vertex_shader(const VertexData& v) {
	VertexData result;

	Eigen::Vector4f vec = v.worldCoords;
	vec = view * model * vec;
	result.worldCoords = vec;

	vec = projection * vec;
	vec.x() /= vec.w();
	vec.y() /= vec.w();
	vec.z() /= vec.w();
	float w = vec.w();
	vec.w() /= vec.w(); //不要直接将w归一化，它保存了投影变换前z的信息,它将在透视矫正中起作用，因此这里将w暂时保存起来，在视口变换后再恢复它	
	vec = viewport * vec;
	vec.w() = w;
	result.screenCoords = vec;

	Eigen::Matrix4f inv_trans = (view * model).inverse().transpose();
	Eigen::Vector4f normal = { v.normal.x(),v.normal.y(),v.normal.z(),0 };
	normal = inv_trans * normal;
	result.normal = Eigen::Vector3f(normal.x(), normal.y(), normal.z()).normalized();

	Eigen::Vector3f ka = { 0.005f,0.005f,0.005f };
	Eigen::Vector3f kd = { v.color.x() / 255.f,v.color.y() / 255.f ,v.color.z() / 255.f };
	Eigen::Vector3f ks = { 0.8f,0.8f,0.8f };

	Eigen::Vector3f amb_light_intensity = { 10,10,10 };

	Color color = { 0,0,0 };

	for (auto& light : lights) {
		Eigen::Vector3f point(v.worldCoords.x(), v.worldCoords.y(), v.worldCoords.z());
		Eigen::Vector3f light_dir = (light.position - point).normalized();
		Eigen::Vector3f view_dir = (-point).normalized();
		float r = (light.position - point).norm();

		Eigen::Vector3f La = ka.cwiseProduct(amb_light_intensity);
		Eigen::Vector3f Ld = kd.cwiseProduct((light.intensity / (r * r)) * std::max(0.f, result.normal.dot(light_dir)));
		Eigen::Vector3f h = (view_dir + light_dir).normalized();
		float p = 150;
		Eigen::Vector3f Ls = ks.cwiseProduct((light.intensity / (r * r)) * std::pow(std::max(0.f, result.normal.dot(h)), p));

		color = La + Ld + Ls;
	}
	color = { std::clamp(color.x(),0.f,1.f),std::clamp(color.y(),0.f,1.f),std::clamp(color.z(),0.f,1.f) };
	color *= 255.f;
	result.color = color;

	result.texCoords = v.texCoords;

	return result;
}
IShader::Color Gouraud_shader::fragment_shader(const VertexData& v) {
	return v.color;
}


VertexData Blinn_phong_shader::vertex_shader(const VertexData& v) {
	VertexData result;

	Eigen::Vector4f vec = v.worldCoords;
	vec = view * model * vec;
	result.worldCoords = vec;

	vec = projection * vec;
	vec.x() /= vec.w();
	vec.y() /= vec.w();
	vec.z() /= vec.w();
	float w = vec.w();
	vec.w() /= vec.w();
	vec = viewport * vec;
	vec.w() = w;
	result.screenCoords = vec;

	Eigen::Matrix4f inv_trans = (view * model).inverse().transpose();
	Eigen::Vector4f normal = { v.normal.x(),v.normal.y(),v.normal.z(),0 };
	normal = inv_trans * normal;
	result.normal = Eigen::Vector3f(normal.x(), normal.y(), normal.z()).normalized();
	 
	result.color = v.color; //Eigen::Vector3f(255, 255, 255);//
	result.texCoords = v.texCoords;

	return result;
}
IShader::Color Blinn_phong_shader::fragment_shader(const VertexData& v) {
	Eigen::Vector3f ka = { 0.005f,0.005f,0.005f };
	Eigen::Vector3f kd = { v.color.x() / 255.f,v.color.y() / 255.f ,v.color.z() / 255.f };
	Eigen::Vector3f ks = { 0.8f,0.8f,0.8f };

	Eigen::Vector3f amb_light_intensity = { 10,10,10 };

	Eigen::Vector3f point = { v.worldCoords.x(),v.worldCoords.y(),v.worldCoords.z() };
	Eigen::Vector3f normal = v.normal.normalized();

	Eigen::Vector3f color = { 0,0,0 };

	for (auto& light : lights){
		Eigen::Vector3f light_dir = (light.position - point).normalized();
		Eigen::Vector3f view_dir = (-point).normalized();  //经过mv变换，摄像头位置为（0，0，0）
		float r = (light.position - point).norm();

		Eigen::Vector3f La = ka.cwiseProduct(amb_light_intensity);
		Eigen::Vector3f Ld = kd.cwiseProduct((light.intensity / (r * r)) * std::max(0.f, normal.dot(light_dir)));

		Eigen::Vector3f h = (view_dir + light_dir).normalized();
		float p = 150;
		Eigen::Vector3f Ls = ks.cwiseProduct((light.intensity / (r * r)) * std::pow(std::max(0.f, normal.dot(h)), p));

		color += La + Ld + Ls;
	}
	color = { std::clamp(color.x(),0.f,1.f),std::clamp(color.y(),0.f,1.f),std::clamp(color.z(),0.f,1.f) };
	color *= 255.f;

	return color;
}