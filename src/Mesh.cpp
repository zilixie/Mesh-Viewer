#include "Mesh.h"
#include <iostream>

int Mesh::getVBOsize(){
	int n = 0;
	for (int i=0; i<vec_F.size(); i++) {
		n += vec_F[i].num_vertex;
	}
	return n;
}

void Mesh::buildVN() {
	int cur_num_vertex = 0;
	VN.conservativeResize(total_num_vertex, 3);
	if (vec_N.size() == 0) {
		Eigen::MatrixXf mesh_vn;
		per_vertex_normals(vec_V, vec_F, mesh_vn);
		cur_num_vertex = 0;
		for (int i=0; i<vec_F.size(); i++) {
			for (int j=0; j<vec_F[i].num_vertex; j++) {
				int r = vec_F[i]._vertex(j);
				VN.row(cur_num_vertex + j) = mesh_vn.row(r);
			}
			cur_num_vertex += vec_F[i].num_vertex;
		}
	} else {
		//////
		cur_num_vertex = 0;
		for (int i=0; i<vec_F.size(); i++) {
			for (int j=0; j<vec_F[i].num_vertex; j++) {
				int r = vec_F[i]._normal(j);
				VN.row(cur_num_vertex + j) = vec_N[r];
			}
			cur_num_vertex += vec_F[i].num_vertex;
		}
	}
}

void Mesh::buildTC() {
	int cur_num_vertex = 0;
	float maxW = 0.0;
	float maxH = 0.0;
	TC.conservativeResize(total_num_vertex, 2);
	if (vec_TC.size() != 0) {
		cur_num_vertex = 0;
		for (int i=0; i<vec_F.size(); i++) {
			for (int j=0; j<vec_F[i].num_vertex; j++) {
				int r = vec_F[i]._tex_coord(j);
				TC.row(cur_num_vertex + j) = vec_TC[r];
				if (vec_TC[r](0) > maxH) {
					maxH = vec_TC[r](0);
				}
				if (vec_TC[r](1) > maxW) {
					maxW = vec_TC[r](1);
				}
			}
			cur_num_vertex += vec_F[i].num_vertex;
		}
		//printf("%f, %f\n", maxW, maxH);
		TC = TC / std::max(maxW, maxH);
	} else {
		// TC.conservativeResize(total_num_vertex, 2);
		TC = Eigen::MatrixXf::Zero(total_num_vertex, 2);
	}

	cur_num_vertex = 0;
	T.conservativeResize(total_num_vertex, 3);
	for (int i=0; i<vec_F.size(); i++) {
		Eigen::Vector3f pos1 = V.row(cur_num_vertex + 0);
		Eigen::Vector3f pos2 = V.row(cur_num_vertex + 1);
		Eigen::Vector3f pos3 = V.row(cur_num_vertex + 2);

		Eigen::Vector2f uv1 = TC.row(cur_num_vertex + 0);
		Eigen::Vector2f uv2 = TC.row(cur_num_vertex + 1);
		Eigen::Vector2f uv3 = TC.row(cur_num_vertex + 2);

		Eigen::Vector3f edge1 = pos2 - pos1;
		Eigen::Vector3f edge2 = pos3 - pos1;
		Eigen::Vector2f deltaUV1 = uv2 - uv1;
		Eigen::Vector2f deltaUV2 = uv3 - uv1;

		float f = 1/(deltaUV1(0) * deltaUV2(1) - deltaUV1(1) * deltaUV2(0));
		Eigen::Vector3f tangt = (edge1 * deltaUV2(1) - edge2 * deltaUV1(1)) * f;
		tangt.normalize();

		for (int j=0; j<vec_F[i].num_vertex; j++) {
			T.row(cur_num_vertex + j) = tangt;
		}
		cur_num_vertex += vec_F[i].num_vertex;
	}
}

void Mesh::buildN() {
	Eigen::MatrixXf new_fn;
	per_face_normals(vec_V, vec_F, new_fn);

	int cur_num_vertex = 0;
	N.conservativeResize(total_num_vertex, 3);
	for (int i=0; i < vec_F.size(); i++){
		for (int j=0; j<vec_F[i].num_vertex; j++) {
			N.row(cur_num_vertex + j) = new_fn.row(i);
		}
		cur_num_vertex += vec_F[i].num_vertex;
	}
}

void Mesh::buildV(float scale){
	V.conservativeResize(total_num_vertex, 3);
	int cur_num_vertex = 0;
	float total_area = 0.0;
	Eigen::Vector3f mesh_bc (0.0,0.0,0.0);

	for (int i=0; i<vec_F.size(); i++) {
		for (int j=0; j<vec_F[i].num_vertex; j++) {
			int r = vec_F[i]._vertex(j);
			V.row(cur_num_vertex + j) = scale* vec_V[r];
		}
		if (vec_F[i].num_vertex == 3) {
			Eigen::Vector3f v1(V(cur_num_vertex, 0), V(cur_num_vertex, 1), V(cur_num_vertex, 2));
			Eigen::Vector3f v2(V(cur_num_vertex+1, 0), V(cur_num_vertex+1, 1), V(cur_num_vertex+1, 2));
			Eigen::Vector3f v3(V(cur_num_vertex+2, 0), V(cur_num_vertex+2, 1), V(cur_num_vertex+2, 2));

			Eigen::Vector3f bc ((v1(0)+v2(0)+v3(0))/3, (v1(1)+v2(1)+v3(1))/3, (v1(2)+v2(2)+v3(2))/3);
			float area = 0.5 * ((v2-v1).cross(v3-v1)).norm();
			mesh_bc += (area * bc);
			total_area += area;
		} else if (vec_F[i].num_vertex == 4) {
			Eigen::Vector3f v1(V(cur_num_vertex, 0), V(cur_num_vertex, 1), V(cur_num_vertex, 2));
			Eigen::Vector3f v2(V(cur_num_vertex+1, 0), V(cur_num_vertex+1, 1), V(cur_num_vertex+1, 2));
			Eigen::Vector3f v3(V(cur_num_vertex+2, 0), V(cur_num_vertex+2, 1), V(cur_num_vertex+2, 2));
			Eigen::Vector3f v4(V(cur_num_vertex+3, 0), V(cur_num_vertex+3, 1), V(cur_num_vertex+3, 2));

			Eigen::Vector3f bc ((v1(0)+v2(0)+v3(0)+v4(0))/4, (v1(1)+v2(1)+v3(1)+v4(1))/4, (v1(2)+v2(2)+v3(2)+v4(2))/4);
			float area1 = 0.5 * ((v2-v1).cross(v3-v1)).norm();
			float area2 = 0.5 * ((v4-v1).cross(v3-v1)).norm();
			float area = area1 + area2;
			mesh_bc += (area * bc);
			total_area += area;
		}
		cur_num_vertex += vec_F[i].num_vertex;
	}
	mesh_bc = mesh_bc/total_area;
	for (int i = 0; i < total_num_vertex; i++) {
		V.row(i) << V(i,0) - mesh_bc(0), V(i,1) - mesh_bc(1), V(i,2) - mesh_bc(2); 
	}
}

Mesh::Mesh(std::string filename, float scale) {
	read_obj(filename,vec_V,vec_N,vec_TC,vec_F); // raw

	top_right = Eigen::Vector3f(0,0,0);
	bottom_left = Eigen::Vector3f(0,0,0);

	Eigen::MatrixXf mat_V; // mat_V
	mat_V.resize(vec_V.size(), 3);
	printf("vec_V: %ld, vec_N: %ld, vec_TC: %ld, vec_F: %ld\n", vec_V.size(), vec_N.size(), vec_TC.size(), vec_F.size());
	for (int i=0; i<vec_V.size(); i++) {
		mat_V.row(i) << vec_V[i](0), vec_V[i](1), vec_V[i](2);
		if (vec_V[i](0) > top_right(0)) {top_right(0) = vec_V[i](0);}
		if (vec_V[i](1) > top_right(1)) {top_right(1) = vec_V[i](1);}
		if (vec_V[i](2) > top_right(2)) {top_right(2) = vec_V[i](2);}

		if (vec_V[i](0) < bottom_left(0)) {bottom_left(0) = vec_V[i](0);}
		if (vec_V[i](1) < bottom_left(1)) {bottom_left(1) = vec_V[i](1);}
		if (vec_V[i](2) < bottom_left(2)) {bottom_left(2) = vec_V[i](2);}
	}

	total_num_vertex = getVBOsize();

	buildV(scale);
	buildN();
	buildVN();
	buildTC();

	start = 0;
}
