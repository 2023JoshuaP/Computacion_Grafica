#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
using namespace std;

struct Atom {
    string element;
    float x, y, z;
};

vector<Atom> atoms;
float rotate_x = 0.0f;
float rotate_y = 0.0f;
float offset_x = 0.0f;
float offset_y = 0.0f;
float zoom = -20.0f;

map<string, float> radii = {
    {"H", 0.8f}, {"C", 0.2f}, {"O", 1.0f}, {"N", 0.5f}
};

map<string, vector<float>> colors = {
    {"H", {0.2f, 0.2f, 1.0f}}, {"C", {0.7f, 0.0f, 0.0f}}, {"O", {0.0f, 0.7f, 0.0f}}, {"N", {0.7f, 0.0f, 0.7f}}
};

void read_file_molecule(const string& file_name) {
    ifstream file_molecule(file_name);
    string line;
    while (getline(file_molecule, line)) {
        Atom atom;
        istringstream iss(line);
        if (!(iss >> atom.element >> atom.x >> atom.y >> atom.z)) {
            continue;
        }
        atoms.push_back(atom);
    }
}

int main() {
    return 0;
}