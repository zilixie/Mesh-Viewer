// #include <GLFW/glfw3.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>
#include <SOIL/SOIL.h>

#include "GUI.h"
#include "Editor.h"

Editor* editor;

// Callback Functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    editor->gui.screen->resizeCallbackEvent(width, height);
    float aspect_ratio = float(height)/float(width);
    editor->cam->view(0,0) = aspect_ratio * editor->cam->view(1,1);
    glViewport(0, 0, width, height);
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (editor->gui.in_window({(int)(editor->cursor->p1(0)), 800 -(int)(editor->cursor->p1(1))})) {
        editor->gui.screen->scrollCallbackEvent(xoffset, yoffset);
    } else {
        if (editor->cam->radius <= 2 && yoffset > 0) return;
        else if (editor->cam->radius >= 16 && yoffset < 0) return;

        if (editor->cam->radius - 0.2 * yoffset <= 2) editor->cam->radius = 2;
        else if (editor->cam->radius - 0.2 * yoffset >= 16) editor->cam->radius = 16;
        else editor->cam->radius = editor->cam->radius - 0.2 * yoffset;
    }
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
    editor->gui.screen->cursorPosCallbackEvent(xpos, ypos);
    int width, height;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwGetWindowSize(window, &width, &height);
    Eigen::Vector2f pixel(xpos, height-1-ypos); // pixel position
    // Track the mouse positions
    editor->cursor->p0 = editor->cursor->p1;
    editor->cursor->p1 = pixel;
    if (editor->cursor->right_key_pressed == 1) {
        float a = editor->cursor->p1(0) - editor->cursor->p0(0);
        float b = editor->cursor->p1(1) - editor->cursor->p0(1);
        editor->cam->alpha_shift += 0.5 * a * M_PI/90.0;
        editor->cam->beta_shift -= 0.5 * b * M_PI/90.0;
    }
}

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods) {
    editor->gui.screen->mouseButtonCallbackEvent(button, action, mods);
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) editor->cursor->right_key_pressed = 1;
        else editor->cursor->right_key_pressed = 0;
    }
}

void setWindowCallbacks() {
    glfwSetCharCallback(editor->window,[](GLFWwindow *, unsigned int codepoint) {
        editor->gui.screen->charCallbackEvent(codepoint);
    });
    glfwSetDropCallback(editor->window,[](GLFWwindow *, int count, const char **filenames) {
        editor->gui.screen->dropCallbackEvent(count, filenames);
    });
    glfwSetKeyCallback(editor->window, [](GLFWwindow*, int key, int scancode, int action, int mods) {
        editor->gui.screen->keyCallbackEvent(key, scancode, action, mods);
    });
    glfwSetMouseButtonCallback(editor->window, mouse_click_callback); // Register the mouse callback
    glfwSetCursorPosCallback(editor->window, mouse_move_callback);    // Register the cursor move callback
    glfwSetFramebufferSizeCallback(editor->window, framebuffer_size_callback); // Update viewport
    glfwSetScrollCallback(editor->window, mouse_scroll_callback);
}


void renderer_draw() {
    editor->resources->_usedPrograms[0]->bind();
    glUniformMatrix4fv(editor->resources->_usedPrograms[0]->uniform("view"), 1, GL_FALSE, editor->cam->view.data());
    glUniformMatrix4fv(editor->resources->_usedPrograms[0]->uniform("camera"), 1, GL_FALSE, editor->cam->camera.data());
    //Vector3f light_d = editor->cam->cam_pos.normalized();
    Vector3f light_pos = editor->cam->cam_pos;
    glUniform3f(editor->resources->_usedPrograms[0]->uniform("light_pos"), light_pos(0), light_pos(1),light_pos(2));
    glUniform1i(editor->resources->_usedPrograms[0]->uniform("render_mode"), editor->render_mode);
    glUniform1i(editor->resources->_usedPrograms[0]->uniform("useTex"), editor->useTex);

    editor->resources->_usedVAOs[editor->cur_mesh]->bind();
    // printf("cur_mesh: %d\n", editor->cur_mesh);
    if (editor->cur_mesh != editor->gui.ml->selectedIndex) {
        editor->cur_mesh = editor->gui.ml->selectedIndex;
        editor->resources->loadMeshes(editor->cur_mesh);
    }
    glDrawArrays(GL_TRIANGLES, 0, editor->mesh_vector[editor->cur_mesh].vec_F.size() * 3);
    // editor->resources->_usedEBOs[editor->cur_mesh]->bind();
    // glDrawElements(GL_TRIANGLES, 90, GL_UNSIGNED_INT, 0);
}

int main(int /* argc */, char ** /* argv */) {
    glfwInit();
    glfwSetTime(0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    editor = new Editor(Eigen::Vector3f(12.0, 0.0, 0.0));
    editor->setGUICallbacks();
    // editor->gui.nanogui_init(editor->window);

    editor->resources->addProgram(new Program(),"../shader/vertex_shader.glsl","../shader/fragment_shader.glsl","outColor");
    editor->resources->_usedPrograms[0]->bind(); 
    // printf("hello\n");

    // editor->insert_mesh("../models/buddha.obj", 6);
    // editor->insert_mesh("../models/dragon.obj", 6);
    // editor->insert_mesh("../models/bunny3.obj", 6);
    // editor->insert_mesh("../models/nefertiti.obj", 6);
    editor->insert_mesh("../models/cube.obj", 6);
    // editor->insert_mesh("../models/bunny3.obj", 6);
    // editor->insert_mesh("../models/sample3.obj", 6);
    // editor->insert_mesh("../models/sample1.obj", 6);
    // editor->insert_mesh("../models/sample2.obj", 6);
    editor->resources->loadMeshes(0);



    editor->resources->addTexture(new Texture());
    editor->resources->_usedTextures[0]->update("../textures/black_leather_closeup_1/normal.png");
    glActiveTexture(GL_TEXTURE0 + 0);

    editor->resources->addTexture(new Texture());
    editor->resources->_usedTextures[1]->update("../textures/black_leather_closeup_1/diffuse.png");
    glActiveTexture(GL_TEXTURE0 + 1);

    editor->resources->addTexture(new Texture());
    editor->resources->_usedTextures[2]->update("../textures/black_leather_closeup_1/normal.png");
    glActiveTexture(GL_TEXTURE0 + 2);

    editor->resources->addTexture(new Texture());
    editor->resources->_usedTextures[3]->update("../textures/black_leather_closeup_1/glossiness.png");
    glActiveTexture(GL_TEXTURE0 + 3);

    editor->resources->addTexture(new Texture());
    editor->resources->_usedTextures[4]->update("../textures/black_leather_closeup_1/diffuse.png"); // tex_normal3
    glActiveTexture(GL_TEXTURE0 + 4);

    editor->resources->addTexture(new Texture());
    editor->resources->_usedTextures[5]->update("../textures/black_leather_closeup_1/specular.png"); // tex_normal4
    glActiveTexture(GL_TEXTURE0 + 5);


    glUniform1i(editor->resources->_usedPrograms[0]->uniform("tex_normal0"), 0);
    glUniform1i(editor->resources->_usedPrograms[0]->uniform("tex_normal1"), 1);
    glUniform1i(editor->resources->_usedPrograms[0]->uniform("tex_normal2"), 2);
    glUniform1i(editor->resources->_usedPrograms[0]->uniform("tex_normal3"), 3); // d
    glUniform1i(editor->resources->_usedPrograms[0]->uniform("tex_normal4"), 4); // d
    glUniform1i(editor->resources->_usedPrograms[0]->uniform("tex_normal5"), 5); // s


    Eigen::MatrixXf m = Eigen::MatrixXf::Identity(4, 4);
    glUniformMatrix4fv(editor->resources->_usedPrograms[0]->uniform("model"), 1, GL_FALSE, m.data()); 
    glUniformMatrix4fv(editor->resources->_usedPrograms[0]->uniform("proj"), 1, GL_FALSE, editor->cam->persp.data());
    glUniform3f(editor->resources->_usedPrograms[0]->uniform("diffuse"), 110,100,160);
    glUniform3f(editor->resources->_usedPrograms[0]->uniform("specular"), 100,100,100);
    glUniform3f(editor->resources->_usedPrograms[0]->uniform("ambient"), 40,40,40);
    glUniform1f(editor->resources->_usedPrograms[0]->uniform("p"), 100);
    glUniform1f(editor->resources->_usedPrograms[0]->uniform("kg"), 50);

    glUniform1i(editor->resources->_usedPrograms[0]->uniform("render_mode"), editor->render_mode);
    glUniform1i(editor->resources->_usedPrograms[0]->uniform("useTex"), 0);

    glUniformMatrix4fv(editor->resources->_usedPrograms[0]->uniform("view"), 1, GL_FALSE, editor->cam->view.data());

    TimePoint t_start = TimePoint();
    // Game loop
    glClearColor(0.9f, 0.9f, 0.9f, 0.4f);
    glEnable(GL_DEPTH_TEST);

    setWindowCallbacks();
    while (!glfwWindowShouldClose(editor->window)) {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        editor->cam->update_camera(std::chrono::high_resolution_clock::now(), t_start);
        // Draw triangles 

        renderer_draw();
        editor->gui.screen->drawWidgets();
        glfwSwapBuffers(editor->window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
