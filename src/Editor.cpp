#include "Editor.h"

Editor::Editor(Eigen::Vector3f cam_init_pos) {
	cursor = new Cursor();
	resources = new ResourceManager();
	cam = new Camera(cam_init_pos);
	cam->view(0,0) = 800.0/1000.0 * cam->view(1,1);
	cur_mesh = 0;
	render_mode = 0;
	useTex = 0;

	window = glfwCreateWindow(1000, 800, "3D Viewer", nullptr, nullptr);    // Create a GLFWwindow object
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeLimits(window, 600, 400, GLFW_DONT_CARE, GLFW_DONT_CARE);

	gui.nanogui_init(window);
}

void Editor::insert_mesh(std::string filename, float scale) {
	printf("%s\n", filename.c_str());
	Mesh new_mesh(filename, scale);

	float m1 = std::max(new_mesh.top_right(0) - new_mesh.bottom_left(0), new_mesh.top_right(1) - new_mesh.bottom_left(1));
	float m2 = std::max(m1, new_mesh.top_right(2) - new_mesh.bottom_left(2));
	new_mesh.V = new_mesh.V * 1.0/m2;

	int n = (int)mesh_vector.size();

	resources->addVAO(new VertexArrayObject());
	resources->addVBO(new VertexBufferObject());//v
	resources->addVBO(new VertexBufferObject());//n
	resources->addVBO(new VertexBufferObject());//tc
	resources->addVBO(new VertexBufferObject());//fn
	resources->addVBO(new VertexBufferObject());//tngt

	resources->_usedVAOs[n]->bind();
	resources->_usedVBOs[n*5]->update(new_mesh.V.transpose());
	resources->_usedVBOs[n*5+1]->update(new_mesh.VN.transpose());
	resources->_usedVBOs[n*5+2]->update(new_mesh.TC.transpose());
	resources->_usedVBOs[n*5+3]->update(new_mesh.N.transpose());
	resources->_usedVBOs[n*5+4]->update(new_mesh.T.transpose());

	new_mesh.V = Eigen::MatrixXf::Identity(4, 4);
	new_mesh.VN = Eigen::MatrixXf::Identity(4, 4);
	new_mesh.TC = Eigen::MatrixXf::Identity(4, 4);
	new_mesh.N = Eigen::MatrixXf::Identity(4, 4);
	new_mesh.T = Eigen::MatrixXf::Identity(4, 4);
	mesh_vector.push_back(new_mesh);
}

void Editor::setGUICallbacks() {
	gui.render_mode->setCallback([this](int a) {
		this->gui.render_mode->setSelectedIndex(a);
		nanogui::Button * buttonA = ((nanogui::Button *)gui.render_mode->popup()->children()[a]);
		buttonA->mouseEnterEvent({0,0}, false);
		this->render_mode = a;

		//std::cout << this->gui.render_mode->items().size() << std::endl;
		if (a == 0) {
			this->gui.glossiness->setVisible(false);
			this->gui.metallic->setVisible(false);
			this->gui.shineness->setVisible(true);
			this->gui.ambient->setVisible(true);
			this->gui.screen->performLayout();
			// this->gui.shineness->setVisible(true);
			//this->gui.nanoguiWindow->setSize(gui.nanoguiWindow->size() - Eigen::Vector2i(0,20));
		} else {
			this->gui.glossiness->setVisible(true);
			this->gui.metallic->setVisible(true);
			this->gui.shineness->setVisible(false);
			this->gui.ambient->setVisible(false);
			this->gui.screen->performLayout();
			//this->gui.nanoguiWindow->setSize(gui.nanoguiWindow->size() + Eigen::Vector2i(0,20));
		}
		// std::cout << this->gui.nanoguiWindow->size() << std::endl;
		// std::cout << this->gui.tb->size() << std::endl;
	});

	gui.texOrColor->setCallback([this](int a) {
		this->gui.texOrColor->setSelectedIndex(a);
		nanogui::Button * buttonA = ((nanogui::Button *)gui.texOrColor->popup()->children()[a]);
		buttonA->mouseEnterEvent({0,0}, false);
		this->useTex = a;
	});
	gui.b1->setCallback([&]{cam->pause_or_resume();});
	gui.tb->setCallback([this](const int n) {
		this->resources->setFloat(0, "p", n);
	});
	nanogui::IntBox<int> * gloss_w = (nanogui::IntBox<int> *)gui.glossiness->w;
	gloss_w->setCallback([this](const int n) {
		// std::cout << "hello" << std::endl;
		this->resources->setFloat(0, "kg", n);
	});
	auto tb_callback = [this](ColorController* cc, std::string s, int i, int n) {
		nanogui::Color c = cc->intbox_callback(i, n);
		this->resources->setFloat(0, s, c.r()*255.0, c.g()*255.0, c.b()*255.0);
	};
	auto cw_callback = [this](ColorController* cc, std::string s, const nanogui::Color &c) {
		cc->colorwheel_callback(c);
		this->resources->setFloat(0, s, c.r()*255.0, c.g()*255.0, c.b()*255.0);
	};
	auto open_mesh_callback = [&]() {
		std::string pathname = nanogui::file_dialog({{"obj", "Object File Format"}}, false);
		if (pathname.length() == 0) return;
		std::string file = pathname.substr(pathname.find_last_of("/") + 1, pathname.length());
		insert_mesh(pathname, 6);
		gui.ml->toolbox->setLayout(new nanogui::GridLayout(nanogui::Orientation::Horizontal, 1,nanogui::Alignment::Maximum, 0, 0));
		gui.ml->addItem(file ,(int)gui.ml->names.size());
		gui.ml->names.push_back(file);
		gui.screen->performLayout();
	};

	gui.cc->tb_r->setCallback([this, tb_callback](const int n) {tb_callback(gui.cc, "diffuse", 0, n);});
	gui.cc->tb_g->setCallback([this, tb_callback](const int n) {tb_callback(gui.cc, "diffuse", 1, n);});
	gui.cc->tb_b->setCallback([this, tb_callback](const int n) {tb_callback(gui.cc, "diffuse", 2, n);});
	gui.cc->cw->setCallback([this, cw_callback](const nanogui::Color &c) {cw_callback(gui.cc, "diffuse", c);});

	gui.cc2->tb_r->setCallback([this, tb_callback](const int n) {tb_callback(gui.cc2, "specular", 0, n);});
	gui.cc2->tb_g->setCallback([this, tb_callback](const int n) {tb_callback(gui.cc2, "specular", 1, n);});
	gui.cc2->tb_b->setCallback([this, tb_callback](const int n) {tb_callback(gui.cc2, "specular", 2, n);});
	gui.cc2->cw->setCallback([this, cw_callback](const nanogui::Color &c) {cw_callback(gui.cc2, "specular", c);});

	gui.cc3->tb_r->setCallback([this, tb_callback](const int n) {tb_callback(gui.cc3, "ambient", 0, n);});
	gui.cc3->tb_g->setCallback([this, tb_callback](const int n) {tb_callback(gui.cc3, "ambient", 1, n);});
	gui.cc3->tb_b->setCallback([this, tb_callback](const int n) {tb_callback(gui.cc3, "ambient", 2, n);});
	gui.cc3->cw->setCallback([this, cw_callback](const nanogui::Color &c) {cw_callback(gui.cc3, "ambient", c);});

	gui.ml->open_mesh->setCallback(open_mesh_callback);
}
