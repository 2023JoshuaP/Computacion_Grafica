#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tiffio.h>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

/* Recoleccion de puntos en 3 dimensiones */
struct Point3D {
    int x, y, z;
};

/* Estructuras para la Triangulacion de Delaunay*/
struct Point {
    int x, y;
};

struct Triangle {
    Point vertex[3];
    Triangle* neighbors[3];
    bool contraints[3];
    bool improvable;

    Triangle(Point* point_a, Point* point_b, Point* point_c);
};

struct Triangulation {
    vector<Point> points;
    vector<Triangle*> triangles;

    void insert(Point point);
    void enforceConstraints();
    void refine();
};

/* Extraccion de los puntos en un TIFF */
vector<Point3D> extraction_points(const string& file) {
    vector<Point3D> points;
    TIFF *tif = TIFFOpen(file.c_str(), "r");
    int count = 0;
    if (tif) {
        do {
            count++;
        } while (TIFFReadDirectory(tif));
    }
    cout << "Total de páginas: " << count << endl;
    uint32 width, height;
    int index_page_tif = 1;

    do {
        if (!TIFFSetDirectory(tif, index_page_tif)) {
            std::cerr << "No se pudo acceder a la página " << index_page_tif << "\n";
            break;
        }
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

        uint32* raster = (uint32*)_TIFFmalloc(width * height * sizeof(uint32));
        if (!raster) {
            cerr << "Error de memoria al leer la página " << index_page_tif << endl;
            break;
        }

        if (!TIFFReadRGBAImageOriented(tif, width, height, raster, ORIENTATION_TOPLEFT, 0)) {
            cerr << "Error al leer la página " << index_page_tif << endl;
            _TIFFfree(raster);
            break;
        }

        for (uint32 i = 0; i < height; i++) {
            for (uint32 j = 0; j < width; j++) {
                uint32 pixel = raster[i * width + j];
                uint8_t red = TIFFGetR(pixel);
                uint8_t green = TIFFGetG(pixel);
                uint8_t blue = TIFFGetB(pixel);

                if (red || green || blue) {
                    points.push_back({
                        static_cast<int>(j),
                        static_cast<int>(i),
                        index_page_tif
                    });
                }
            }
        }

        _TIFFfree(raster);
        ++index_page_tif;
    } while (TIFFReadDirectory(tif) || TIFFLastDirectory(tif) == 0);

    TIFFClose(tif);
    return points;
}

int main() {
    string path = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 3/Public/brainMasks.tiff";
    vector<Point3D> points = extraction_points(path);
    if (points.empty()) {
        cerr << "No se encontraron puntos en el archivo TIFF." << endl;
        return -1;
    }

    map<int, vector<Point>> points_by_page;
    for (const auto& point : points) {
        points_by_page[point.z].push_back({point.x, point.y});
    }

    cout << "Total of points: " << points.size() << endl;
    cout << "Total of slices: " << points_by_page.size() << "\n\n";

    for (const auto& [z, pts] : points_by_page) {
        cout << "Slice z=" << z << " -> points: " << pts.size() << endl;
    }
    return 0;
}