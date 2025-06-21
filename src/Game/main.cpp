#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glad/gl.h>

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stbi/stb_image.h>

#include "../Engine/Engine.hpp"
#include "GameObjectSet.hpp"

namespace GLFW {
    namespace Callbacks {
        void Error(int error, const char* desc) {
            fprintf(stderr, "GLFW Error: %s\n", desc);
        }

        void Key(GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_O && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            }
        }

        void FramebufferSizeChanged(GLFWwindow* window, int width, int height) {
            glViewport(0,0,width,height);
        }
    }

    GLFWwindow* bootstrap(const char* title) {
        glfwSetErrorCallback(Callbacks::Error);
        if (!glfwInit()) return 0;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(640, 480, title, NULL, NULL);

        if (!window) {
            glfwTerminate();
            return 0;
        }

        glfwMakeContextCurrent(window);
        
        glfwSetFramebufferSizeCallback(window, Callbacks::FramebufferSizeChanged);
        glfwSetKeyCallback(window, Callbacks::Key);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        
        if (!gladLoadGL(glfwGetProcAddress)) {
            fprintf(stderr, "Error: Bootstrap: Cannot load GLAD!\n");
            glfwDestroyWindow(window);
            glfwTerminate();
            return 0;
        }

        stbi_set_flip_vertically_on_load(false);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        /*
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        */
        glfwSwapInterval(1);
        return window;
    }
}

int main(void) {
    GLFWwindow* window = GLFW::bootstrap("TestTitle");
    if (!window) {
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);


    Engine::Render::Shader baseShader = Engine::Render::Shader("../../../../resources/Base.vs", "../../../../resources/Base.fs");
    Engine::Render::Shader texturedShader = Engine::Render::Shader("../../../../resources/Textured.vs", "../../../../resources/Textured.fs");

    Engine::Render::Model car = Engine::Render::Model("../../../../resources/Models/CarReexported/Car.obj");
    Engine::Render::Model town = Engine::Render::Model("../../../../resources/Models/Terrain/SmallCity.obj");

    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;

    printf("Going to window\n");

    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

    
    JustDrive::Car Car(
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,0.0f,0.0f),
        5.0f,
        2.0f,
        2.0f,
        0.8f,
        1.3f,
        20.0f
    );
    JustDrive::CarCamera CarCamera(
        glm::vec3(0.0f, -10.0f, 0.0f),
        3.0f,
        1.0f,
        &Car.GameObject::transform
    );

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwGetFramebufferSize(window, &width, &height);

        float frameTime = static_cast<float>(glfwGetTime());
        deltaTime = frameTime-lastFrameTime;
        lastFrameTime = frameTime;


        glm::mat4 model, view, projection;
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 carModel = Car.getTransform();

        view = CarCamera.getViewMatrix();
        
        projection = glm::perspective(CarCamera.yFov, (float)width/height, 0.1f, 100.0f);

        baseShader.use();
        baseShader.setMat4("mProjection", projection);
        baseShader.setMat4("mView", view);
        baseShader.setMat4("mModel", carModel);
        car.Draw(baseShader);
        texturedShader.use();
        texturedShader.setMat4("mProjection", projection);
        texturedShader.setMat4("mView", view);
        texturedShader.setMat4("mModel", model);
        town.Draw(texturedShader);

        Car.run(window, deltaTime);
        CarCamera.run(window, deltaTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(baseShader.ID);
    glDeleteProgram(texturedShader.ID);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


