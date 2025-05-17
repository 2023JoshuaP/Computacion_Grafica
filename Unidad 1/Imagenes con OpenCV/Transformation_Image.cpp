#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;



vector<Vec3b> color_map_personalized() {
    vector<Vec3b> color_map(256);
    Mat color_map_image(50, 256, CV_8UC3);
    for (int i = 0; i < 256; i++) {
        float t = i /255.0f;
        uchar b = static_cast<uchar>(255 * (1 - t));
        uchar g = static_cast<uchar>(255*t);
        uchar r = 128;
        Vec3b c(b, g, r);
        color_map[i] = c;
        for (int j = 0; j < 50; j++) {
            color_map_image.at<Vec3b>(j, i) = c;
        }
    }
    imshow("Mapa de Color", color_map_image);
    return color_map;
}

Mat create_image_personalized(const Mat& image, const vector<Vec3b>& color_map) {
    Mat image_gray(image.rows, image.cols, CV_8UC1);
    Mat image_transformed(image.rows, image.cols, CV_8UC3);
    if (image.channels() == 3) {
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                Vec3b pixel = image.at<Vec3b>(i, j);
                uchar gray_value = static_cast<uchar>(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
                image_gray.at<uchar>(i, j) = gray_value;
                Vec3b color_value = color_map[gray_value];
                image_transformed.at<Vec3b>(i, j) = color_value;
            }
        }
    }
    else {
        image_gray = image.clone();
    
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                uchar gray_value = image_gray.at<uchar>(i, j);
                Vec3b color_value = color_map[gray_value];
                image_transformed.at<Vec3b>(i, j) = color_value;
            }
        }
    }
    return image_transformed;
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
    vector<Vec3b> color_map = color_map_personalized();
    Mat image_transformed = create_image_personalized(image_original, color_map);
    imshow("Imagen Original", image_original);
    imshow("Imagen Transformada", image_transformed);
    waitKey(0);
    return 0;
}