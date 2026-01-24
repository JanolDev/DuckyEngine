#include "./src/core/window/Window.hpp"


//“Here is exactly how the GPU shall draw this triangle, with no assumptions, no magic, no MERCY.”

//screen resolution
// int WIDTH = 800;
// int HEIGHT = 600;
//
// //change resolution of the viewport according to changes in window size
// void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
//     glViewport(0, 0, width, height);
// }
//
// // function check if esc was clicked while being inside window
// //If yes then forces it to close/terminate
// void processInput(GLFWwindow *window) {
//     if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//         glfwSetWindowShouldClose(window, true);
//     }
//     if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
//         glClear(GL_COLOR_BUFFER_BIT);
//         glClearColor(1.0f, 0.7f, 0.4, 1.0f);
//     }
// }
int main() {
    //




    //Zanim zaczniesz przeczytaj ten komentarz

    //sciezna w ktorej znajduja sie funkcje uzyte ponizej:
    //src/core/window/Window.hpp/cpp
    //Zrobile inicjalizacje okna, funkcje callback
    //zeby zmianial rozmiar viewporta jak window sie zmienia
    //i analogicznie do tego zrobilem podstawowe finkcje.
    //poll do zbierania imputow - tak mi sie wydaje ze od tego bylo
   //swap buffers - to jest to czego nie rozumielismy
    //a zwyczajnie to zamiania ostatni buffer z pierwszym
    //dzieki temu sie renderuje w oknie(nwm jak)
    // jest jeszcze pare funkcji ktorych nie uzylem bo musialem sie szykowac naimpreze
    //w pliku hpp masz wszystkie ktore zdazylem zrobic a w cpp jak one dzialaja.
    //licze na to ze poprawisz moj kod jesli znajdziesz bledy
    //wprowadzisz komentarze, zmiany ,optymalizacje
    //przepraszam nie zdazylem zrobic zmiany koloru okna :(

    //po przeczytaniu usun ten komentarz i przefiltruj caly main bo nie chcialem usuwac wszystkiego wiec zakomentowalem
    // Powodzenia






    Window window(800, 600,"Engine");

    while (!window.shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);

        window.swapBuffers();
        window.pollEvents();
    }









//     //Initialize Window and by it we load all libraries of glfw
//     //We define version of the glfw major and minor
//     //we initialize profile with all core libraries
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//     //macOS requires forward-compatible core profile for OpenGL
//     //This removes deprecated functions satisfy macOS's OpenGL requirements
// #ifdef __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif
//
//     //Create a windowed mode window with given width, height, and title
//     //last two parameters: first: windowed window, not fullscreen; second: no shared context
//     GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Hello Window", nullptr, nullptr);
//
//     //If by making a window we get some bugs or failures we  terminate the process
//     if (!window) {
//         std::cout << "Failed to create GLFW window\n";
//         glfwTerminate();
//         return -1;
//     }
//
//     // We make our current/main context
//     glfwMakeContextCurrent(window);
//
//     //If glad was not sucessfully initialized we end pro
//     if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
//         std::cout << "Failed to initialize GLAD\n";
//         return -1;
//     }
//
//     //Setting the size of the viewport
//     glViewport(0, 0, WIDTH, HEIGHT);
//
//
//     //Executing viewport resizing according to the size of the window
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//
//     //checks if app should be closed or not
//     //changing pixel colors according to executed functions
//     //listener to keyboard and mouse inputs
//     while (!glfwWindowShouldClose(window)) {
//         processInput(window);
//
//         //rendering commands goes here
//         //glClearColor just sets background color (RGBA)
//         //glClear cleares color buffer (will be filled with glClearColor)a
//         //so clear color ustawiasz kolor a gl clear jakby go wzrucasz? wsenie definicja->uzycie? takkkkkkk:)choc do gory ci wytlumacze
//         //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         //glClear(GL_COLOR_BUFFER_BIT);
//
//         glfwPollEvents();
//         glfwSwapBuffers(window);
//     }
//
//     //terminate all app processes
//     glfwTerminate();
    return 0;
}
