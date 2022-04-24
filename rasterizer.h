#pragma once
#include<Eigen/eigen>
#include<algorithm>
#include<memory>
#include<string>
#include<vector>
#include<ctime>
#include"ObjLoader.h"
#include"tgaimage.h"
#include"Shader.h"

class rasterizer
{
	typedef obj::Vertex Vertex;
	typedef obj::Face Face;
	typedef Eigen::Vector3f Color;
public:
	rasterizer(int width, int height);

	int width() { return w; }
	int height() { return h; }
	unsigned char* get_color_from_buffer(int x, int y);

	void set_loaded_obj(const obj::Loader& l) { loaded_obj = std::make_shared<obj::Loader>(l); }
	void set_texture(const std::string tex_name) { texture.read_tga_file(tex_name); }

	void set_model(const Eigen::Matrix4f& m) { model = m; }
	void set_view(const Eigen::Matrix4f& v) { view = v; }
	void set_projection(const Eigen::Matrix4f& p) { projection = p; }
	void set_viewport(const Eigen::Matrix4f& v) { viewport = v; }

	void set_shader(const std::shared_ptr<IShader>& s) { shader = s; }
	void set_light(const std::vector<Light>& l) { lights = l; }

	void draw_point(int x, int y, const Color& color);
	void draw_line(const Vertex& begin, const Vertex& end, const Color& color);
	void draw_pointcloud(const Color& color = { 255,255,255 });
	void draw_wireframe(const Color& color = { 255,255,255 });
	void draw_triangle(const VertexData& v0, const VertexData& v1, const VertexData& v2);
	void draw();

	void flip_vertically();
	void clear_frame_buffer();
	void clear_depth_buffer();
	void make_file(const std::string& filename);
private:
	static int pixels;
	static long start, end;
	int w;
	int h;

	std::shared_ptr<obj::Loader> loaded_obj;
	TGAImage image;
	TGAImage texture;

	std::vector<Color> frame_buffer;
	std::vector<float> depth_buffer;

	Eigen::Matrix4f model;
	Eigen::Matrix4f view;
	Eigen::Matrix4f projection;
	Eigen::Matrix4f viewport;

	std::shared_ptr<IShader> shader;	
	std::vector<Light> lights;

	Eigen::Vector3f world_to_screen(const Eigen::Vector3f& worldcoord);
	Color get_texture(float u, float v);
	std::tuple<float, float, float> barycentric2D(float x, float y, const Vertex* v);

};

