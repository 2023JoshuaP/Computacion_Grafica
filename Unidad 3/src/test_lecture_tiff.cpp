#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tiffio.h>
#include <iostream>
#include <cstdint>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)glsl";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Variables globales
int totalPages = 0;
int currentPage = 0;
GLuint textureID = 0;
TIFF* tif = nullptr;

GLuint loadTIFFPageAsTexture(TIFF* tif, int pageIndex, int& width, int& height)
{
    TIFFSetDirectory(tif, pageIndex);

    TIFFRGBAImage img;
    char emsg[1024];
    if (!TIFFRGBAImageBegin(&img, tif, 0, emsg))
    {
        std::cerr << "Error en página " << pageIndex << ": " << emsg << std::endl;
        return 0;
    }

    width = img.width;
    height = img.height;

    uint32_t npixels = width * height;
    uint32_t* raster = (uint32_t*)_TIFFmalloc(npixels * sizeof(uint32_t));
    if (!raster)
    {
        std::cerr << "Error de memoria\n";
        TIFFRGBAImageEnd(&img);
        return 0;
    }

    if (!TIFFRGBAImageGet(&img, raster, width, height))
    {
        std::cerr << "Error leyendo TIFF\n";
        _TIFFfree(raster);
        TIFFRGBAImageEnd(&img);
        return 0;
    }

    if (textureID == 0)
        glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, raster);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    _TIFFfree(raster);
    TIFFRGBAImageEnd(&img);

    return textureID;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TIFF Multipagina", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Fallo al crear ventana GLFW\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Fallo al inicializar GLAD\n";
        return -1;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
       -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
       -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Abrir TIFF y contar páginas
    tif = TIFFOpen("D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 3/Public/skeletonMasks.tiff", "r");
    if (!tif)
    {
        std::cerr << "No se pudo abrir TIFF\n";
        return -1;
    }

    do { totalPages++; } while (TIFFReadDirectory(tif));
    TIFFSetDirectory(tif, 0);

    std::cout << "Total de páginas: " << totalPages << std::endl;

    int imgWidth, imgHeight;
    loadTIFFPageAsTexture(tif, 0, imgWidth, imgHeight);

    // Iniciar temporizador
    double lastPageTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        double currentTime = glfwGetTime();
        if (currentTime - lastPageTime >= 0.5) // cada 2 segundos
        {
            currentPage = (currentPage + 1) % totalPages;
            loadTIFFPageAsTexture(tif, currentPage, imgWidth, imgHeight);
            std::cout << "Mostrando página " << currentPage + 1 << " / " << totalPages << std::endl;
            lastPageTime = currentTime;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &textureID);
    glfwDestroyWindow(window);
    glfwTerminate();

    TIFFClose(tif);

    return 0;
}