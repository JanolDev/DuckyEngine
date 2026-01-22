//flagi w cmake do linuxa
//zainstalowac loader glad do projektu
//zbudowac cmake (moze sa bledy)
//uruchomic podstawowy program(w konsoli najlepiej jakby pokazalo statusy kart graficznych)
//check jakie flagi trzeba dodawac dla maca
//wrzucic pierwsza wersje na githuba juz z opanowanym cmake, glad, bibliotekami, linkerami

//testowy komentarz, czy go widzisz?
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << "\n";
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        std::cerr << "Nie udało się uruchomić GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720,
        "OpenGL Test | RTX 4050",
        nullptr, nullptr
    );

    if (!window) {
        std::cerr << "Nie udało się stworzyć okna GLFW\n";
        glfwTerminate();
        std::cout <<" alabama:";
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Nie udało się załadować GLAD\n";
        glfwTerminate();
        return 1;
    }
    glClearColor(0.08f, 0.14f, 0.22f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


