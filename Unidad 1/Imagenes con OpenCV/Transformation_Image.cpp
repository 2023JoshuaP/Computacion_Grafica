#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat transformation_color(const Mat& image, const Mat& colorMap) {
    Mat image_gray, image_color;
    if (image.channels() == 3) {
        cvtColor(image, image_gray, COLOR_BGR2GRAY);
    }
    else if (image.channels() == 1) {
        image_gray = image;
    }
    if (image_gray.type() != CV_8UC1) {
        image_gray.convertTo(image_gray, CV_8UC1);
    }
    LUT(image_gray, colorMap, image_color);
    return image_color;
}

Mat create_color_map() {
    Mat colorMap(1, 256, CV_8UC3);
    for (int i = 0; i < 256; i++) {
        colorMap.at<Vec3b>(0, i) = Vec3b(255- i, i, 128);
    }
    return colorMap;
}

int main() {
    string image_file;
    cout << "Ingrese el nombre de la imagen (con extension): ";
    cin >> image_file;
    string image_path = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 1/Imagenes con OpenCV/Imagenes/" + image_file;
    
    Mat image_original = imread(image_path);
    if (image_original.empty()) {
        cout << "No se pudo cargar la imagen." << endl;
        return -1;
    }

    Mat colorMap = create_color_map();
    Mat image_transformed = transformation_color(image_original, colorMap);
        imshow("Imagen Original", image_original);
    imshow("Imagen Transformada", image_transformed);
    waitKey(0);

    return 0;
}