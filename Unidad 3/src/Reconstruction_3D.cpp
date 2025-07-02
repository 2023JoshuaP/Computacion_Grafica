#include <tiffio.h>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    const char* file_tiff = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 3/public/bloodMasks.tiff";
    TIFF* tif = TIFFOpen(file_tiff, "r");
    if (!tif) {
        cerr << "Error opening TIFF file: " << file_tiff << endl;
        return 1;
    }

    uint32 width, height;
    uint16* raster = nullptr;
    int page = 0;

    do {
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        vector<uint32> buffer(width * height);
        if (TIFFReadRGBAImage(tif, width, height, buffer.data(), 0)) {
            cout << "Page " << page << ": " << width << "x" << height << endl;
        }
        else {
            cerr << "Error reading TIFF page " << page << endl;
            break;
        }
        page++;
    } while (TIFFReadDirectory(tif));

    TIFFClose(tif);
    return 0;
}