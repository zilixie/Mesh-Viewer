#ifndef EDITOR_H
#define EDITOR_H

#include <fstream>
#include <string>
#include <iostream>

#include "GUI.h"
#include "Mesh.h"
// #include "Object.h"
#include "Camera.h"
#include "Cursor.h"
#include "ResourceManager.h"

using namespace std;
using namespace Eigen;

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

class Editor {
	public:
		int cur_mesh;
		int render_mode;
		int useTex;
		Cursor* cursor;
		Camera* cam;
		ResourceManager *resources;
		std::vector<Mesh> mesh_vector;
		GLFWwindow* window;
		GUI gui;

		Editor(Eigen::Vector3f cam_init_pos); // Eigen::Vector3f 
		void insert_mesh(std::string filename, float scale);
		void setGUICallbacks();
};

#endif