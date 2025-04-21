#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

void show_channels(const Mat& image) {
    int columns = image.cols;
    int rows = image.rows;
    int minimun_value_blue = 255, maximun_value_blue = 0;
    int minimun_value_green = 255, maximun_value_green = 0;
    int minimun_value_red = 255, maximun_value_red = 0;

    Mat blue_channel(rows, columns, CV_8UC1);
    Mat green_channel(rows, columns, CV_8UC1);
    Mat red_channel(rows, columns, CV_8UC1);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int channel_blue = image.at<Vec3b>(i, j)[0];
            int channel_green = image.at<Vec3b>(i, j)[1];
            int channel_red = image.at<Vec3b>(i, j)[2];

            blue_channel.at<uchar>(i, j) = channel_blue;
            green_channel.at<uchar>(i, j) = channel_green;
            red_channel.at<uchar>(i, j) = channel_red;

            if (channel_blue < minimun_value_blue) {
                minimun_value_blue = channel_blue;
            }
            if (channel_blue > maximun_value_blue) {
                maximun_value_blue = channel_blue;
            }
            if (channel_green < minimun_value_green) {
                minimun_value_green = channel_green;
            }
            if (channel_green > maximun_value_green) {
                maximun_value_green = channel_green;
            }
            if (channel_red < minimun_value_red) {
                minimun_value_red = channel_red;
            }
            if (channel_red > maximun_value_red) {
                maximun_value_red = channel_red;
            }
        }
    }

    imshow("Canal Azul", blue_channel);
    imshow("Canal Verde", green_channel);
    imshow("Canal Rojo", red_channel);
    waitKey(0);

    cout << "Valores de los canales:" << endl;
    cout << "Canal Azul: Minimo: " << minimun_value_blue << ", Maximo: " << maximun_value_blue << endl;
    cout << "Canal Verde: Minimo: " << minimun_value_green << ", Maximo: " << maximun_value_green << endl;
    cout << "Canal Rojo: Minimo: " << minimun_value_red << ", Maximo: " << maximun_value_red << endl;
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

    imshow("Imagen Original", image_original);
    waitKey(0);

    show_channels(image_original);
    return 0;
}