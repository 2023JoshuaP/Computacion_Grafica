#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

constexpr double EPSILON = 1e-8;

struct Point2D {
    float x, y;
    bool operator==(const Point2D& other) const {
        return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON;
    }
};

struct Edge {
    Point2D p1, p2;
    bool operator==(const Edge& other) const {
        return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
    }
};

struct Triangle {
    Point2D p1, p2, p3;
    Point2D circumcenter;
    float circumradius;

    Triangle(const Point2D& a, const Point2D& b, const Point2D& c) : p1(a), p2(b), p3(c) {
        computeCircumcircle();
    }

    void computeCircumcircle() {
        float ax = p1.x, ay = p1.y;
        float bx = p2.x, by = p2.y;
        float cx = p3.x, cy = p3.y;

        float d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
        if (std::abs(d) < EPSILON) {
            circumcenter = {0, 0};
            circumradius = -1;
            return;
        }

        float ux = ((ax * ax + ay * ay) * (by - cy) +
                    (bx * bx + by * by) * (cy - ay) +
                    (cx * cx + cy * cy) * (ay - by)) / d;

        float uy = ((ax * ax + ay * ay) * (cx - bx) +
                    (bx * bx + by * by) * (ax - cx) +
                    (cx * cx + cy * cy) * (bx - ax)) / d;

        circumcenter = {ux, uy};
        float dx = ax - ux;
        float dy = ay - uy;
        circumradius = dx * dx + dy * dy;
    }

    bool contains(const Point2D& p) const {
        float dx = p.x - circumcenter.x;
        float dy = p.y - circumcenter.y;
        return (dx * dx + dy * dy) <= circumradius + EPSILON;
    }

    bool containsVertex(const Point2D& p) const {
        return p == p1 || p == p2 || p == p3;
    }

    bool operator==(const Triangle& other) const {
        std::vector<Point2D> this_pts = {p1, p2, p3};
        std::vector<Point2D> other_pts = {other.p1, other.p2, other.p3};
        for (const auto& pt : this_pts) {
            if (std::find(other_pts.begin(), other_pts.end(), pt) == other_pts.end()) {
                return false;
            }
        }
        return true;
    }

    std::vector<Edge> edges() const {
        return { {p1, p2}, {p2, p3}, {p3, p1} };
    }
};

std::vector<Triangle> bowyerWatson(const std::vector<Point2D>& points) {
    std::vector<Triangle> triangulation;
    float min_x = points[0].x, max_x = points[0].x;
    float min_y = points[0].y, max_y = points[0].y;

    for (const auto& p : points) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);
    }

    float dx = max_x - min_x;
    float dy = max_y - min_y;
    float dmax = std::max(dx, dy);
    float mid_x = (max_x + min_x) / 2.0f;
    float mid_y = (max_y + min_y) / 2.0f;

    Point2D p1 = {mid_x - 20 * dmax, mid_y - dmax};
    Point2D p2 = {mid_x, mid_y + 20 * dmax};
    Point2D p3 = {mid_x + 20 * dmax, mid_y - dmax};
    Triangle superTriangle(p1, p2, p3);
    triangulation.push_back(superTriangle);

    for (const auto& p : points) {
        std::cout << "\n\nProcesando punto: (" << p.x << ", " << p.y << ")\n";

        std::vector<Triangle> badTriangles;
        for (const auto& t : triangulation) {
            if (t.contains(p)) badTriangles.push_back(t);
        }
        std::cout << " - Triangulos conflictivos: " << badTriangles.size() << "\n";

        std::vector<Edge> polygonEdges;
        for (const auto& t : badTriangles) {
            for (const auto& e : t.edges()) {
                bool shared = false;
                for (const auto& t2 : badTriangles) {
                    if (t == t2) continue;
                    for (const auto& e2 : t2.edges()) {
                        if (e == e2) { shared = true; break; }
                    }
                    if (shared) break;
                }
                if (!shared) polygonEdges.push_back(e);
            }
        }
        std::cout << " - Bordes del poligono: " << polygonEdges.size() << "\n";

        triangulation.erase(std::remove_if(triangulation.begin(), triangulation.end(),
            [&](const Triangle& t) {
                return std::find(badTriangles.begin(), badTriangles.end(), t) != badTriangles.end();
            }), triangulation.end());

        for (const auto& e : polygonEdges) {
            std::cout << "   -> Nuevo triangulo con: (" << e.p1.x << ", " << e.p1.y << ") y (" << e.p2.x << ", " << e.p2.y << ")\n";
            triangulation.emplace_back(e.p1, e.p2, p);
        }
    }

    triangulation.erase(std::remove_if(triangulation.begin(), triangulation.end(),
        [&](const Triangle& t) {
            return t.containsVertex(p1) || t.containsVertex(p2) || t.containsVertex(p3);
        }), triangulation.end());

    triangulation.erase(std::unique(triangulation.begin(), triangulation.end()), triangulation.end());
    return triangulation;
}

// ------ OpenGL ------

std::vector<Point2D> points;
std::vector<float> lineVertices;
GLuint vao, vbo;

void updateTriangulation() {
    auto triangles = bowyerWatson(points);
    lineVertices.clear();
    for (const auto& tri : triangles) {
        for (const auto& e : tri.edges()) {
            lineVertices.push_back(e.p1.x);
            lineVertices.push_back(e.p1.y);
            lineVertices.push_back(e.p2.x);
            lineVertices.push_back(e.p2.y);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, lineVertices.size() * sizeof(float), lineVertices.data(), GL_STATIC_DRAW);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        int width, height;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &width, &height);
        float x = (float)(xpos / width * 2.0 - 1.0);
        float y = (float)(1.0 - ypos / height * 2.0);
        points.push_back({x, y});
        std::cout << "\nPunto agregado: (" << x << ", " << y << ")" << std::endl;
        updateTriangulation();
    }
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Delaunay Triangulation", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glLineWidth(2);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, lineVertices.size() / 2);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}