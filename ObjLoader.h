#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include<string>


namespace obj {

	class Vector2f {
	public:
		Vector2f() = default;
		Vector2f(float x, float y) :
			_x(x), _y(y) {}
		float& x() { return _x; }
		float& y() { return _y; }
		const float& x() const { return _x; }
		const float& y() const { return _y; }
		bool operator==(const Vector2f& rhs) const {
			return _x == rhs._x && _y == rhs._y;
		}
		bool operator!=(const Vector2f& rhs) const {
			return !(*this == rhs);
		}
	private:
		float _x, _y;
	};

	class Vector3f {
	public:
		Vector3f() = default;
		Vector3f(float x,float y,float z):
			_x(x),_y(y),_z(z){}
		float& x() { return _x; }
		float& y() { return _y; }
		float& z() { return _z; }
		const float& x() const { return _x; }
		const float& y() const { return _y; }
		const float& z() const { return _z; }
		bool operator==(const Vector3f& rhs) const {
			return _x == rhs._x && _y == rhs._y && _z == rhs._z;
		}
		bool operator!=(const Vector3f& rhs) const {
			return !(*this == rhs);
		}
		Vector3f operator+(const Vector3f& rhs) const 
		{ return Vector3f(_x + rhs._x, _y + rhs._y, _z + rhs._z); }
		Vector3f operator-(const Vector3f& rhs) const 
		{ return Vector3f(_x - rhs._x, _y - rhs._y, _z - rhs._z); }
		Vector3f& operator+=(const Vector3f& rhs) 
		{ _x += rhs._x; _y += rhs._y; _z += rhs._z; return *this; }
		float dot(const Vector3f& rhs) const 
		{ return _x * rhs._x + _y * rhs._y + _z * rhs._z; }
		Vector3f cross(const Vector3f& rhs) const 
		{ return Vector3f(_y * rhs._z - _z * rhs._y, _z * rhs._x - _x * rhs._z, _x * rhs._y - _y * rhs._x); }
		float norm() const 
		{ return sqrt(_x * _x + _y * _y + _z * _z); }
		Vector3f& normalized()
		{ float n = norm(); _x /= n; _y /= n; _z /= n; return *this; }
	private:
		float _x, _y, _z;
	};

	struct Vertex {
		Vector3f position;
		Vector3f normal;
		Vector2f texcoords;
		Vertex() :
			position(Vector3f(0, 0, 0)), normal(Vector3f(0, 0, 0)), texcoords(Vector2f(0, 0)) {}
		Vertex(float x, float y, float z) :
			position(Vector3f(x, y, z)), normal(Vector3f(0, 0, 0)), texcoords(Vector2f(0, 0)) {}
		Vertex(const Vector3f& p, const Vector3f& n = Vector3f(0, 0, 0), const Vector2f& t = Vector2f(0, 0)) :
			position(p), normal(n), texcoords(Vector2f(0, 0)) {}
		float& x() { return position.x(); }
		float& y() { return position.y(); }
		float& z() { return position.z(); }
		const float& x() const { return position.x(); }
		const float& y() const { return position.y(); }
		const float& z() const { return position.z(); }
	};

	struct Face {
		std::vector<size_t> vIndexs;
		std::vector<size_t> vtIndexs;
		std::vector<size_t> vnIndexs;
		Face(std::vector<size_t> v_indexs) :
			vIndexs(v_indexs) {}
		Face(std::vector<size_t> v_indexs, 
			 std::vector<size_t> vt_indexs, 
			 std::vector<size_t> vn_indexs) :
			vIndexs(v_indexs),vtIndexs(vt_indexs),vnIndexs(vn_indexs) {}
	};

	class Loader {
	public:
		std::vector<Vertex> Vertices;
		std::vector<Face> Faces;
		std::vector<Vector3f> Normal;
		std::vector<Vector2f> TexCroods;		

		Loader(const std::string fileName) {
			if (fileName.substr(fileName.size() - 4, 4) != ".obj") {
				std::cout << "ERR: Wrong file type!" << std::endl;;
				exit(1);
			}

			std::ifstream file(fileName);
			if (!file.is_open()) {
				std::cout << "ERR: open file: " << fileName << " failed!" << std::endl;
				exit(1);
			}

			std::cout << "loading file: " << fileName << std::endl;

			size_t lineCnt = 0;
			std::string line;
			while (std::getline(file,line)) {
				std::cout << "\rLoad line: " << lineCnt++;

				std::string data;
				std::istringstream str(line);
				str >> data;
				if (data == "v") {
					float x, y, z;
					str >> data;
					x = atof(data.c_str());
					str >> data;
					y = atof(data.c_str());
					str >> data;
					z = atof(data.c_str());
					Vertices.push_back(Vertex(x, y, z));
				}
				else if (data == "vt") {
					float x, y;
					str >> data;
					x = atof(data.c_str());
					str >> data;
					y = atof(data.c_str());					
					x = x < 0 ? -x : x;
					y = y < 0 ? -y : y;
					TexCroods.push_back(Vector2f(x, y));
				}
				else if (data == "vn") {
					float x, y, z;
					str >> data;
					x = atof(data.c_str());
					str >> data;
					y = atof(data.c_str());
					str >> data;
					z = atof(data.c_str());
					Normal.push_back(Vector3f(x, y, z));
				}
				else if (data == "f") {
					std::vector<size_t> vIndexs;
					std::vector<size_t> vtIndexs;
					std::vector<size_t> vnIndexs;
					while (str >> data) {
						std::string v, vt, vn;
						std::istringstream iss = std::istringstream(data);
						std::getline(iss, v, '/');
						std::getline(iss, vt, '/');
						std::getline(iss, vn, ' ');
						vIndexs.push_back((size_t)atoi(v.c_str()) - 1);						
						vtIndexs.push_back((size_t)(vt != "" ? atoi(vt.c_str()) - 1 : 0));						
						vnIndexs.push_back((size_t)(vn != "" ? atoi(vn.c_str()) - 1 : 0));
					}

					//多边形三角形化
					std::vector<std::vector<size_t>> triangles_v = sub_pologon(vIndexs);
					std::vector<std::vector<size_t>> triangles_vt = sub_pologon(vtIndexs);
					std::vector<std::vector<size_t>> triangles_vn = sub_pologon(vnIndexs);

					//封装三角形面
					for (int i = 0; i < triangles_v.size(); i++) {
						vertex_faces[triangles_v[i][0]].push_back(triangles_v[i]);
						vertex_faces[triangles_v[i][1]].push_back(triangles_v[i]);
						vertex_faces[triangles_v[i][2]].push_back(triangles_v[i]);

						Faces.push_back(Face(triangles_v[i], triangles_vt[i], triangles_vn[i]));
					}										
				}
			}

			if (Normal.empty())		Normal.assign(Vertices.size(), Vector3f(0, 0, 0));
			if (TexCroods.empty())	TexCroods.assign(Vertices.size(), Vector2f(0, 0));

			for (size_t i = 0; i < Faces.size(); i++) {
				Face& face = Faces[i];
				for (size_t j = 0; j < 3; j++) {
					Vertex& v = Vertices[face.vIndexs[j]];
					//对于未给出法线信息的顶点计算法线
					v.normal = Normal[face.vnIndexs[j]] == Vector3f(0, 0, 0) ? calculate_vertex_normal(face.vIndexs[j]) : Normal[face.vnIndexs[j]];
					v.texcoords = TexCroods[face.vtIndexs[j]];
				}			
			}
			file.close();
			std::cout<< "\nLoad file: " << fileName << " sucessfully.\n";
			std::cout	<< "Vertices: " << Vertices.size() 
						<< " Faces: " << Faces.size() << std::endl;
			std::cout << "--------------------------------------------" << std::endl;
		}

	private:
		std::vector<std::vector<size_t>> sub_pologon(std::vector<size_t> vertices) {
			std::vector<std::vector<size_t>> result;
			std::vector<size_t> triangle = { vertices[vertices.size() - 1],vertices[0],vertices[1] };
			result.push_back(triangle);
			if (vertices.size() == 3)
				return result;
			vertices.erase(vertices.begin());
			std::vector<std::vector<size_t>> sub = sub_pologon(vertices);
			for (auto& tri : sub) {
				result.push_back(tri);
			}
			return result;
		}

		std::map<size_t, std::vector<std::vector<size_t>>> vertex_faces;

		Vector3f calculate_vertex_normal(size_t vIndex) {
			std::vector<std::vector<size_t>>& faces = vertex_faces[vIndex];
			int size = faces.size();
			Vector3f v_normal(0, 0, 0);
			for (auto& face : faces) {
				Vertex& v0 = Vertices[face[0]];
				Vertex& v1 = Vertices[face[1]];
				Vertex& v2 = Vertices[face[2]];
				Vector3f f_normal = ((v1.position - v0.position).cross(v2.position - v1.position)).normalized();
				v_normal += f_normal;
			}
			return Vector3f(v_normal.x() / size, v_normal.y() / size, v_normal.z() / size).normalized();
		}
	};
}