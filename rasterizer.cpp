#include "rasterizer.h"

int rasterizer::pixels = 0;
long rasterizer::start = 0;
long rasterizer::end = 0;

rasterizer::rasterizer(int width, int height) :
    w(width),h(height),
	image(width, height, TGAImage::RGB),
	frame_buffer(std::vector(width* height, Color(0, 0, 0))),
    depth_buffer(std::vector(width* height, -FLT_MAX)) {
    start = clock();
}

void rasterizer::make_file(const std::string& filename) {
    std::vector<uint8_t> data;
	for (int i = 0; i < frame_buffer.size(); i++) {
		uint8_t r = (uint8_t)frame_buffer[i].x();
		uint8_t g = (uint8_t)frame_buffer[i].y();
		uint8_t b = (uint8_t)frame_buffer[i].z();
		data.push_back(b);
		data.push_back(g);
		data.push_back(r);
	}
	image.Data() = data;
	image.write_tga_file(filename);
    end = clock();
	std::cout << "Finish rendering." << std::endl;
    std::cout << "Draw pixels: " << pixels << std::endl;
    std::cout << "Rendering time: " << (double)(end - start) / CLOCKS_PER_SEC << " s" << std::endl;
}

void rasterizer::flip_vertically() {
    int half = image.width() >> 1;
    for (int i = 0; i < half; i++) {
        for (int j = 0; j < image.width(); j++) {
            std::swap(frame_buffer[i * image.width() + j], frame_buffer[(image.width() - i - 1) * image.width() + j]);
        }
    }
}

unsigned char* rasterizer::get_color_from_buffer(int x, int y) {
    unsigned char color[3] = { 0,0,0 };
    if (x < 0 || x >= image.width() || y < 0 || y >= image.height())
        return color;
    color[0] = frame_buffer[x + y * image.width()].x();
    color[1] = frame_buffer[x + y * image.width()].y();
    color[2] = frame_buffer[x + y * image.width()].z();
    return color;
}

void rasterizer::clear_frame_buffer() {
    frame_buffer.assign(w * h, Color(0, 0, 0));
}

void rasterizer::clear_depth_buffer() {
    depth_buffer.assign(w * h, -FLT_MAX);
}

void rasterizer::draw_point(int x, int y, const Color& color = {255,255,255}) {
    if (x < 0 || x >= image.width() || y < 0 || y >= image.height())
        return;
    pixels++;
    frame_buffer[x + y * image.width()] = color;
}

void rasterizer::draw_line(const Vertex& begin, const Vertex& end, const Color& color) {
    int x0 = begin.x(), y0 = begin.y(), x1 = end.x(), y1 = end.y();

    bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x1 < x0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int ystep = y1 > y0 ? 1 : -1;

    int error2 = 0;
    int derror2 = 2 * std::abs(dy);

    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep)
            draw_point(y, x, color);
        else
            draw_point(x, y, color);

        error2 += derror2;
        if (error2 > dx) {
            y += ystep;
            error2 -= 2 * dx;
        }
    }
}

Eigen::Vector3f rasterizer::world_to_screen(const Eigen::Vector3f& worldcoord) {
    Eigen::Vector4f vec(worldcoord.x(), worldcoord.y(), worldcoord.z(), 1);
    vec = projection * view * model * vec;
    vec /= vec.w();
    vec = viewport * vec;
    return Eigen::Vector3f(vec.x(), vec.y(), vec.z());
}

void rasterizer::draw_pointcloud(const Color& color) {
    clear_frame_buffer();
    clear_depth_buffer();

    Eigen::Matrix4f mvp = projection * view * model;
    for (int i = 0; i < loaded_obj->Faces.size(); i++) {
        Face& face = loaded_obj->Faces[i];
        Eigen::Vector3f v[3];
        for (int j = 0; j < face.vIndexs.size(); j++) {
            Vertex& vertex = loaded_obj->Vertices[face.vIndexs[j]];
            v[j] = { vertex.x(),vertex.y(),vertex.z()};
            v[j] = world_to_screen(v[j]);
            draw_point(v[j].x(), v[j].y(), color);
        }
    }
}

void rasterizer::draw_wireframe(const Color& color) {
    clear_frame_buffer();
    clear_depth_buffer();

    Eigen::Matrix4f mvp = projection * view * model;
    for (int i = 0; i < loaded_obj->Faces.size(); i++) {
        Face& face = loaded_obj->Faces[i];
        Eigen::Vector3f v[3];
        for (int j = 0; j < face.vIndexs.size(); j++) {
            Vertex& vertex = loaded_obj->Vertices[face.vIndexs[j]];
            v[j] = { vertex.x(),vertex.y(),vertex.z() };
            v[j] = world_to_screen(v[j]);
        }

        obj::Vertex vert[3];
        for (int j = 0; j < 3; j++) {
            vert[j] = obj::Vertex(v[j].x(), v[j].y(), v[j].z());
        }

        draw_line(vert[0], vert[1], color);
        draw_line(vert[1], vert[2], color);
        draw_line(vert[2], vert[0], color);

    }
}

rasterizer::Color rasterizer::get_texture(float u, float v) {
    int U = (int)(u * texture.width()), V = (int)(v * texture.height());
    return Color(texture.get(U, V).r(), texture.get(U, V).g(), texture.get(U, V).b());
}

std::tuple<float, float, float> rasterizer::barycentric2D(float x, float y, const Vertex* v) {
    float alpha = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
    float beta = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
    float gamma = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
    return { alpha,beta,gamma };
}

Eigen::Vector4f interpolate(const Eigen::Vector4f& vec0, const Eigen::Vector4f& vec1, const Eigen::Vector4f& vec2, float alpha, float beta, float gamma) {
    return alpha * vec0 + beta * vec1 + gamma * vec2;
}
Eigen::Vector3f interpolate(const Eigen::Vector3f& vec0, const Eigen::Vector3f& vec1, const Eigen::Vector3f& vec2, float alpha, float beta, float gamma) {
    return alpha * vec0 + beta * vec1 + gamma * vec2;
}
Eigen::Vector2f interpolate(const Eigen::Vector2f& vec0, const Eigen::Vector2f& vec1, const Eigen::Vector2f& vec2, float alpha, float beta, float gamma) {
    return alpha * vec0 + beta * vec1 + gamma * vec2;
}

Eigen::Vector4f interpolate(const Eigen::Vector4f& vec0, const Eigen::Vector4f& vec1, const Eigen::Vector4f& vec2, float alpha, float beta, float gamma, float Zp, float* w) {
    return (alpha * vec0 / w[0] + beta * vec1 / w[1] + gamma * vec2 / w[2]) / (1.f / Zp);
}
Eigen::Vector3f interpolate(const Eigen::Vector3f& vec0, const Eigen::Vector3f& vec1, const Eigen::Vector3f& vec2, float alpha, float beta, float gamma,float Zp, float *w) {
    return (alpha * vec0 / w[0] + beta * vec1 / w[1] + gamma * vec2 / w[2]) / (1.f / Zp);
}
Eigen::Vector2f interpolate(const Eigen::Vector2f& vec0, const Eigen::Vector2f& vec1, const Eigen::Vector2f& vec2, float alpha, float beta, float gamma, float Zp, float* w) {
    return (alpha * vec0 / w[0] + beta * vec1 / w[1] + gamma * vec2 / w[2]) / (1.f / Zp);
}

void rasterizer::draw_triangle(const VertexData& v0, const VertexData& v1, const VertexData& v2) {
    int left, right, bottom, top;
    left = floor(std::min(v0.screenCoords.x(), std::min(v1.screenCoords.x(), v2.screenCoords.x())));
    right = ceil(std::max(v0.screenCoords.x(), std::max(v1.screenCoords.x(), v2.screenCoords.x())));
    bottom = floor(std::min(v0.screenCoords.y(), std::min(v1.screenCoords.y(), v2.screenCoords.y())));
    top = ceil(std::max(v0.screenCoords.y(), std::max(v1.screenCoords.y(), v2.screenCoords.y())));

    left = std::clamp(left, 0, image.width() - 1);
    right = std::clamp(right, 0, image.width() - 1);
    bottom = std::clamp(bottom, 0, image.height() - 1);
    top = std::clamp(top, 0, image.height() - 1);

    for (int x = left; x <= right; x++) {
        for (int y = bottom; y <= top; y++) {            
            Vertex v[3];
            v[0] = { v0.screenCoords.x(),v0.screenCoords.y(),v0.screenCoords.z() };
            v[1] = { v1.screenCoords.x(),v1.screenCoords.y(),v1.screenCoords.z() };
            v[2] = { v2.screenCoords.x(),v2.screenCoords.y(),v2.screenCoords.z() };
            auto [alpha, beta, gamma] = barycentric2D(x, y, v);
            if (alpha < 0 || beta < 0 || gamma < 0)
                continue;

            float w[3] = { v0.screenCoords.w() ,v1.screenCoords.w() ,v2.screenCoords.w() };
            //float Zp = 1.0 / (alpha * v[0].z() / w[0] + beta * v[1].z() / w[1] + gamma * v[2].z() / w[2]);
            float Zp = 1.0 / (alpha / w[0] + beta / w[1] + gamma / w[2]);
            
            if (depth_buffer[int(x + y * image.width())] < Zp) {
                VertexData interpolated_vertex;

                interpolated_vertex.worldCoords = interpolate(v0.worldCoords, v1.worldCoords, v2.worldCoords, alpha, beta, gamma, Zp, w);
                interpolated_vertex.normal = interpolate(v0.normal, v1.normal, v2.normal, alpha, beta, gamma, Zp, w).normalized();
                interpolated_vertex.texCoords = interpolate(v0.texCoords, v1.texCoords, v2.texCoords, alpha, beta, gamma, Zp, w);
                interpolated_vertex.color = get_texture(interpolated_vertex.texCoords.x(), interpolated_vertex.texCoords.y());
                //color 需要通过纹理来获得(在模型点数较少时会取得明显较好的效果)
                //interpolated_vertex.color = interpolate(v0.color, v1.color, v2.color, alpha, beta, gamma, Zp, w);

                Color color = shader->fragment_shader(interpolated_vertex);

                draw_point(x, y, color);
                depth_buffer[int(x + y * image.width())] = Zp;
            }
        }
    }
}

void rasterizer::draw() {
    clear_frame_buffer();
    clear_depth_buffer();

    shader->model = model;
    shader->view = view;
    shader->projection = projection;
    shader->viewport = viewport;
    shader->lights = lights;

    for (int i = 0; i < loaded_obj->Faces.size(); i++) {
        Face& face = loaded_obj->Faces[i];
        VertexData v[3];
        for (int j = 0; j < 3; j++) {
            obj::Vertex& vertex = loaded_obj->Vertices[face.vIndexs[j]];
            v[j].worldCoords = { vertex.x(),vertex.y(),vertex.z(),1 };
            v[j].normal = { vertex.normal.x(),vertex.normal.y() ,vertex.normal.z()};
            v[j].texCoords = { vertex.texcoords.x(),vertex.texcoords.y() };
            v[j].color = get_texture(vertex.texcoords.x(), vertex.texcoords.y());
        
            v[j] = shader->vertex_shader(v[j]);
        }
    
        Eigen::Vector3f worldcoords[3];
        worldcoords[0] = { v[0].worldCoords.x(),v[0].worldCoords.y() ,v[0].worldCoords.z() };
        worldcoords[1] = { v[1].worldCoords.x(),v[1].worldCoords.y() ,v[1].worldCoords.z() };
        worldcoords[2] = { v[2].worldCoords.x(),v[2].worldCoords.y() ,v[2].worldCoords.z() };

        //背面剔除
        Eigen::Vector3f tri_normal = (worldcoords[1] - worldcoords[0]).cross(worldcoords[2] - worldcoords[0]).normalized();
        Eigen::Vector3f dir = (-worldcoords[0]).normalized();
        float dot = dir.dot(tri_normal);
        if (dot < 0)
            continue;

        draw_triangle(v[0], v[1], v[2]);
    }
}