#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stbi/stb_image.h>

#include "../Engine/Engine.hpp"
#include "GameObjectSet.hpp"

namespace GLFW {
    namespace Callbacks {
        void Error(int error, const char* desc) {
            //коллбек, расскажет где ошиблись
            fprintf(stderr, "GLFW Error: %s\n", desc);
        }

        //коллбек, базовые элементы управления ходом работы программы
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

        //коллбек, позволяет безболезненно менять размер окна
        void FramebufferSizeChanged(GLFWwindow* window, int width, int height) {
            glViewport(0,0,width,height);
        }
    }

    GLFWwindow* bootstrap(const char* title) {
        glfwSetErrorCallback(Callbacks::Error);//привязали коллбек
        if (!glfwInit()) return 0;//если не получилось создать контекст - умираем

        //указываем версию, нужно для шейдеров да и в целом хорошая практика
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //указываем, что работать хотим в CORE, а не COMPAT, получаем функционал
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //создаём окошко базовых размеров
        GLFWwindow* window = glfwCreateWindow(640, 480, title, NULL, NULL);
        
        //если не создалось - убиваемся и умираем
        if (!window) {
            glfwTerminate();
            return 0;
        }

        //говорим, что работем с этим окошком
        glfwMakeContextCurrent(window);
        
        //привязали коллбеки
        glfwSetFramebufferSizeCallback(window, Callbacks::FramebufferSizeChanged);
        glfwSetKeyCallback(window, Callbacks::Key);
        //говорим, что курсор нужно скушать и не отдавать (по коллбеку - отдадут)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        //загружаем GLAD
        if (!gladLoadGL()) {
            //если не вышло - умираем и убиваемся, дважды.
            fprintf(stderr, "Error: Bootstrap: Cannot load GLAD!\n");
            glfwDestroyWindow(window);
            glfwTerminate();
            return 0;
        }
        //полезная штучка для импорта моделей, а всё потому что спасибо OpenGL
        stbi_set_flip_vertically_on_load(false);

        //Включаем базовое тестирование глубины, чтобы модели поверх друг друга правильно рисовались
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //Могли и куллинг включить, но у машинки кривые непоправимые нормали
        /*
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        */
        //Говорим, что вертикально хотим синхронизироваться с герцовкой монитора
        glfwSwapInterval(1);
        //Возвращаем готовое окно
        return window;
    }
}

int main(void) {
    GLFWwindow* window = GLFW::bootstrap("TestTitle");
    //если окна не получилось - умираем
    if (!window) {
        return -1;
    }
    /*
    сразу меняем размер. на Windows не страшно,
      а вот Arch+Wayland не дают создать произвольное окно,
      управляя размером сами
    */
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    //Инициализировали шейдеры
    Engine::Render::Shader baseShader = Engine::Render::Shader("resources/Base.vs", "resources/Base.fs");
    Engine::Render::Shader texturedShader = Engine::Render::Shader("resources/Textured.vs", "resources/Textured.fs");

    //Инициализировали модели
    Engine::Render::Model car = Engine::Render::Model("resources/Models/CarReexported/Car.obj");
    Engine::Render::Model town = Engine::Render::Model("resources/Models/Terrain/SmallCity.obj");

    //переменные для кадровой логики
    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;

    //отчитались что всё классно
    printf("Going to window\n");

    //поставили цвет фона(далее в окошке будет именно он)
    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);

    //Инициализируем автомобиль
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
    //Инициализируем камеру(потому что трансформу-то из автомобиля надо взять!)
    JustDrive::CarCamera CarCamera(
        glm::vec3(0.0f, -10.0f, 0.0f),
        3.0f,
        1.0f,
        &Car.GameObject::transform
    );

    //Основной цикл отрисовки
    //Условие может выполниться либо по коллбеку, либо по сигналу типа -USR1
    while (!glfwWindowShouldClose(window)) {
        //Чистим буферы цвета и глубины
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        float frameTime = static_cast<float>(glfwGetTime());
        deltaTime = frameTime-lastFrameTime;
        lastFrameTime = frameTime;

        //придумали наши матрицы машины, окружения, камеры и проекции
        glm::mat4 townModel, carModel,view, projection;
        townModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        townModel = glm::scale(townModel, glm::vec3(1.0f, 1.0f, 1.0f));

        //ну тут понятно, уже говорили
        carModel = Car.getTransform();
        //тут тоже
        view = CarCamera.getViewMatrix();
        //получили размер окошка, для матрицы проекции
        glfwGetFramebufferSize(window, &width, &height);
        //создали матрицу проекции
        projection = glm::perspective(CarCamera.yFov, (float)width/height, 0.1f, 100.0f);
        //это шейдер автомобиля, делаем:
        baseShader.use();
        //присваиваем матрицы в нужные места в шейдере
        baseShader.setMat4("mProjection", projection);
        baseShader.setMat4("mView", view);
        baseShader.setMat4("mModel", carModel);
        //рисуем машину с заданным шейдером
        car.Draw(baseShader);
        //теперь шейдер города, он использует текстуры
        texturedShader.use();
        //аналогично, присваиваем матрицы, разве что mModel другая
        texturedShader.setMat4("mProjection", projection);
        texturedShader.setMat4("mView", view);
        texturedShader.setMat4("mModel", townModel);
        //рисуем город с заданным шейдером
        town.Draw(texturedShader);

        //вызываем методы покадрового изменения в нашей машине и камере
        Car.run(window, deltaTime);
        CarCamera.run(window, deltaTime);

        //подменяем текущий кадр новым
        glfwSwapBuffers(window);
        //получаем "ивенты" - нажатие кнопок, сигналы извне и т.д.
        glfwPollEvents();
    }

    //чистим память от шейдеров
    glDeleteProgram(baseShader.ID);
    glDeleteProgram(texturedShader.ID);

    //а вот буферы GLFW почистит сам в момент glfwTerminate()
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


