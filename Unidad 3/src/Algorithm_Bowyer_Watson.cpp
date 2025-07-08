#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
using namespace std;

constexpr double EPSILON = 1e-8;

struct Point2D {
    double x, y;
    bool operator==(const Point2D& other) const {
        return abs(x - other.x) < EPSILON && abs(y - other.y) < EPSILON;
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
    double circumradius;

    Triangle(const Point2D& a, const Point2D& b, const Point2D& c) : p1(a), p2(b), p3(c) {
        computeCircumcircle();
    }

    void computeCircumcircle() {
        double ax = p1.x, ay = p1.y;
        double bx = p2.x, by = p2.y;
        double cx = p3.x, cy = p3.y;

        double d = 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));
        if (abs(d) < EPSILON) {
            circumcenter = {0, 0}; // Degenerate case
            circumradius = -1;
            return;
        }

        double ux = ((ax * ax + ay * ay) * (by - cy) + (bx * bx + by * by) * (cy - ay) + (cx * cx + cy * cy) * (ay - by)) / d;
        double uy = ((ax * ax + ay * ay) * (cx - bx) + (bx * bx + by * by) * (ax - cx) + (cx * cx + cy * cy) * (bx - ax)) / d;

        circumcenter = {ux, uy};
        double dx = ax - ux;
        double dy = ay - uy;
        circumradius = dx * dx + dy * dy;
    }

    bool contains(const Point2D& p) const {
        double dx = p.x - circumcenter.x;
        double dy = p.y - circumcenter.y;
        return (dx * dx + dy * dy) <= circumradius + EPSILON;
    }

    bool containsVertex(const Point2D& p) const {
        return p == p1 || p == p2 || p == p3;
    }

    bool operator==(const Triangle& other) const {
        return (p1 == other.p1 && p2 == other.p2 && p3 == other.p3) ||
               (p1 == other.p2 && p2 == other.p3 && p3 == other.p1) ||
               (p1 == other.p3 && p2 == other.p1 && p3 == other.p2);
    }

    vector<Edge> edges() const {
        return {
            {p1, p2},
            {p2, p3},
            {p3, p1}
        };
    }
};

vector<Triangle> bowyerWatson(const vector<Point2D>& points) {
    vector<Triangle> triangulation;
    double min_x = points[0].x, max_x = points[0].x;
    double min_y = points[0].y, max_y = points[0].y;

    for (const auto& p : points) {
        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.y < min_y) min_y = p.y;
        if (p.y > max_y) max_y = p.y;
    }

    double dx = max_x - min_x;
    double dy = max_y - min_y;
    double dmax = max(dx, dy);
    double mid_x = (max_x + min_x) / 2.0;
    double mid_y = (max_y + min_y) / 2.0;

    Point2D p1 = {mid_x - 20 * dmax, mid_y - dmax};
    Point2D p2 = {mid_x, mid_y + 20 * dmax};
    Point2D p3 = {mid_x + 20 * dmax, mid_y - dmax};
    Triangle superTriangle(p1, p2, p3);
    triangulation.push_back(superTriangle);

    for (const auto& p : points) {
        vector<Triangle> badTriangles;
        for (const auto& t : triangulation) {
            if (t.contains(p)) {
                badTriangles.push_back(t);
            }
        }

        vector<Edge> polygonEdges;
        for (const auto& t : triangulation) {
            for (const auto& e : t.edges()) {
                bool isShared = false;
                for (const auto& t2 : badTriangles) {
                    if (t2 == t) {
                        continue;
                    }
                    for (const auto& e2 : t2.edges()) {
                        if (e == e2) {
                            isShared = true;
                            break;
                        }
                    }
                    if (isShared) {
                        break;
                    }
                }
                if (!isShared) {
                    polygonEdges.push_back(e);
                }
            }
        }

        triangulation.erase(remove_if(triangulation.begin(), triangulation.end(),
            [&](const Triangle& t) {
                return find(badTriangles.begin(), badTriangles.end(), t) != badTriangles.end();
            }), triangulation.end());
        
        for (const auto& e : polygonEdges) {
            triangulation.emplace_back(e.p1, e.p2, p);
        }
    }

    triangulation.erase(remove_if(triangulation.begin(), triangulation.end(),
        [&](const Triangle& t) {
            return t.containsVertex(p1) || t.containsVertex(p2) || t.containsVertex(p3);
        }), triangulation.end());
    
    triangulation.erase(std::unique(triangulation.begin(), triangulation.end()), triangulation.end());
    return triangulation;
}

int main() {
    vector<Point2D> points = {
        {0.1, 0.2}, 
        {0.4, 0.1}, 
        {0.3, 0.5}, 
        {0.7, 0.2}, 
        {0.8, 0.6}
    };

    auto triangles = bowyerWatson(points);

    for (const auto& t : triangles) {
        cout << "Triangle: (" 
             << t.p1.x << ", " << t.p1.y << "), ("
             << t.p2.x << ", " << t.p2.y << "), ("
             << t.p3.x << ", " << t.p3.y << ")\n";
    }

    return 0;
}