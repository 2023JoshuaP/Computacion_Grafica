#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW\n";
        return -1;
    }

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ventana OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error al crear la ventana\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error al inicializar GLAD\n";
        return -1;
    }

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // Color de fondo
        glClear(GL_COLOR_BUFFER_BIT);         // Limpiar pantalla

        glfwSwapBuffers(window);  // Mostrar frame
        glfwPollEvents();         // Procesar eventos
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
