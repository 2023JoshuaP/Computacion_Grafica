#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat rotate_image_antihorary(const Mat& image) {
    int rows = image.rows;
    int columns = image.cols;

    Mat image_rotate(columns, rows, CV_8UC3);
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            image_rotate.at<Vec3b>(columns - 1 - j, i) = image.at<Vec3b>(i, j);
        }
    }

    return image_rotate;
}

int main() {
    int number_of_rotations;
    string image_file;
    cout << "Ingrese el nombre de la imagen (con extension): ";
    cin >> image_file;
    cout << "Cuantas rotaciones desea hacer? (1-4): ";
    cin >> number_of_rotations;

    string image_path = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 1/Imagenes con OpenCV/Imagenes/" + image_file;

    Mat image_original = imread(image_path);
    if (image_original.empty()) {
        cout << "No se pudo abrir la imagen." << endl;
        return -1;
    }

    imshow("Imagen Original", image_original);

    if (number_of_rotations < 1 || number_of_rotations > 4) {
        cout << "Numero de rotaciones no valido." << endl;
        return -1;
    }

    Mat image_rotated_antihorary = image_original;
    for (int i = 0; i < number_of_rotations; i++) {
        image_rotated_antihorary = rotate_image_antihorary(image_rotated_antihorary);
    }

    imshow("Imagen Rotada Antihorario", image_rotated_antihorary);

    waitKey(0);

    return 0;
}