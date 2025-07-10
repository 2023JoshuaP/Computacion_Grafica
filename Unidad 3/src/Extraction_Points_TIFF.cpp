#include <tiffio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

struct Point3D {
    int coordinate_x, coordinante_y, coordinante_z;
};

bool is_eedge_pixel(uint8_t** binary_image, int x, int y, int width, int height) {
    if (binary_image[y][x] == 0) {
        return false;
    }

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                return true;
            }
            if (binary_image[ny][nx] == 0) {
                return true;
            }
        }
    }
    return false;
}

vector<Point3D> extraction_points_edge(const string& filename) {
    vector<Point3D> points;

    TIFF* tif = TIFFOpen(filename.c_str(), "r");
    if (!tif) {
        cerr << "Don't open the TIFF file." << endl;
        return points;
    }

    int index_page = 0;
    do {
        uint32_t width, height;
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

        uint32_t* raster = (uint32_t*)_TIFFmalloc(width * height * sizeof(uint32_t));
        if (!raster) {
            cerr << "Memory error." << endl;
            break;
        }

        if (!TIFFReadRGBAImageOriented(tif, width, height, raster, ORIENTATION_TOPLEFT, 0)) {
            cerr << "Error reading image." << endl;
            _TIFFfree(raster);
            break;
        }

        uint8_t** binary_image = new uint8_t*[height];
        for (uint32_t y = 0; y < height; ++y) {
            binary_image[y] = new uint8_t[width];
            for (uint32_t x = 0; x < width; ++x) {
                uint32_t pixel = raster[y * width + x];
                uint8_t r = TIFFGetR(pixel);
                uint8_t g = TIFFGetG(pixel);
                uint8_t b = TIFFGetB(pixel);

                binary_image[y][x] = (r > 0 || g > 0 || b > 0) ? 255 : 0;
            }
        }

        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                if (is_eedge_pixel(binary_image, x, y, width, height)) {
                    points.push_back({ (int)x, (int)(height - y), index_page });
                }
            }
        }

        for (uint32_t y = 0; y < height; ++y) {
            delete[] binary_image[y];
        }
        delete[] binary_image;

        _TIFFfree(raster);
        index_page++;
    } while (TIFFReadDirectory(tif));

    TIFFClose(tif);
    return points;
}

bool save_file_coordinates(const string& filename, const vector<Point3D>& points) {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& point : points) {
        file << point.coordinate_x << " " << point.coordinante_y << " " << point.coordinante_z << "\n";
    }

    file.close();
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <TIFF file>" << endl;
        return 1;
    }

    string file_tiff = argv[1];
    string base_name = file_tiff.substr(file_tiff.find_last_of("/\\") + 1);
    base_name = base_name.substr(0, base_name.find_last_of('.'));
    string file_xyz = "coordenadas/" + base_name + "_edges.xyz";

    vector<Point3D> points = extraction_points_edge(file_tiff);
    cout << "Total points extracted: " << points.size() << endl;

    system("mkdir coordenadas >nul 2>nul");

    if (!save_file_coordinates(file_xyz, points)) {
        cerr << "Error saving coordinates to " << file_xyz << endl;
    }
    else {
        cout << "Coordinates file XYZ saved to " << file_xyz << endl;
    }
}