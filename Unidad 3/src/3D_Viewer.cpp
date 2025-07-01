/* Librerias necesarias para OpenGL */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* Librerias de C++ */
#include <iostream>
#include <vector>
#include <fstream>
using namespace std;
using namespace glm;

const char* vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 mvp;
void main() {
    gl_Position = mvp * vec4(aPos, 1.0);
})";

const char* fragment_shader_source = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 color;
void main() {
    FragColor = vec4(color, 1.0);
})";

struct Atom {
    float position_x, position_y, position_z;
    string symbol;
};

struct Bond {
    int atom_1, atom_2;
};

vector<Atom> atoms;
vector<Bond> bonds;
float rotation_x = 0, rotation_y = 0, zoom = 10.0f;
const float rotation_speed = 0.1f, zoom_speed = 0.02f;

void load_file(const string& file) {
    ifstream file_molecule(file);
    if (!file_molecule.is_open()) {
        cerr << "Error opening file: " << file << endl;
        return;
    }

    string line;
    for (int i = 0; i < 4; i++) {
        getline(file_molecule, line);
    }
    int num_atoms = stoi(line.substr(0, 3));
    int num_bonds = stoi(line.substr(3, 3));

    for (int i = 0; i < num_atoms; i++) {
        getline(file_molecule, line);
        Atom atom;
        atom.position_x = stof(line.substr(0, 10));
        atom.position_y = stof(line.substr(10, 10));
        atom.position_z = stof(line.substr(20, 10));
        atom.symbol = line.substr(31, 3);
        atoms.push_back(atom);
    }
    for (int i = 0; i < num_bonds; i++) {
        getline(file_molecule, line);
        Bond bond;
        bond.atom_1 = stoi(line.substr(0, 3)) - 1;
        bond.atom_2 = stoi(line.substr(3, 3)) - 1;
        bonds.push_back(bond);
    }
}

unsigned int create_shader() {
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

void interface_user(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rotation_y -= rotation_speed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rotation_y += rotation_speed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rotation_x -= rotation_speed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rotation_x += rotation_speed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        zoom -= zoom_speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        zoom += zoom_speed;
}

int main() {
    string file_molecule = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 3/Conformer3D_COMPOUND_CID_2244.sdf";

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window_view = glfwCreateWindow(800, 600, "Molecule Viewer", NULL, NULL);
    if (!window_view) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window_view);
    gladLoadGL();
    glad_glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);

    load_file(file_molecule);
    unsigned int shader_program = create_shader();

    vector<float> atom_vertices;
    for (const auto& atom : atoms) {
        atom_vertices.push_back(atom.position_x);
        atom_vertices.push_back(atom.position_y);
        atom_vertices.push_back(atom.position_z);
    }

    vector<float> bond_vertices;
    for (const auto& bond : bonds) {
        if (bond.atom_1 < atoms.size() && bond.atom_2 < atoms.size()) {
            bond_vertices.push_back(atoms[bond.atom_1].position_x);
            bond_vertices.push_back(atoms[bond.atom_1].position_y);
            bond_vertices.push_back(atoms[bond.atom_1].position_z);
            
            bond_vertices.push_back(atoms[bond.atom_2].position_x);
            bond_vertices.push_back(atoms[bond.atom_2].position_y);
            bond_vertices.push_back(atoms[bond.atom_2].position_z);
        }
    }

    unsigned int atom_VBO, atom_VAO;
    glGenVertexArrays(1, &atom_VAO);
    glGenBuffers(1, &atom_VBO);
    glBindVertexArray(atom_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, atom_VBO);
    glBufferData(GL_ARRAY_BUFFER, atom_vertices.size() * sizeof(float), atom_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int bond_VBO, bond_VAO;
    glGenVertexArrays(1, &bond_VAO);
    glGenBuffers(1, &bond_VBO);
    glBindVertexArray(bond_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, bond_VBO);
    glBufferData(GL_ARRAY_BUFFER, bond_vertices.size() * sizeof(float), bond_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glPointSize(8.0f);

    while (!glfwWindowShouldClose(window_view)) {
        interface_user(window_view);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_program);
        mat4 model = mat4(1.0f);
        model = rotate(model, radians(rotation_x), vec3(1.0f, 0.0f, 0.0f));
        model = rotate(model, radians(rotation_y), vec3(0.0f, 1.0f, 0.0f));
        mat4 view = translate(mat4(1.0f), vec3(0.0f, 0.0f, -zoom));
        mat4 projection = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        mat4 mvp = projection * view * model;

        int mvp_localitation = glGetUniformLocation(shader_program, "mvp");
        glUniformMatrix4fv(mvp_localitation, 1, GL_FALSE, &mvp[0][0]);

        int color_location = glGetUniformLocation(shader_program, "color");
        glUniform3f(color_location, 1.0f, 0.2f, 0.2f);

        glBindVertexArray(atom_VAO);
        glDrawArrays(GL_POINTS, 0, atoms.size());
        glUniform3f(color_location, 1.0f, 1.0f, 1.0f);
        glBindVertexArray(bond_VAO);
        glDrawArrays(GL_LINES, 0, bond_vertices.size() / 3);
        glfwSwapBuffers(window_view);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &atom_VAO);
    glDeleteBuffers(1, &atom_VBO);
    glDeleteVertexArrays(1, &bond_VAO);
    glDeleteBuffers(1, &bond_VBO);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}