#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

vector<Vec3b> color_map_personalized() {
    vector<Vec3b> color_map(256);
    Mat color_map_image(50, 256, CV_8UC3);
    for (int i = 0; i < 256; i++) {
        float t = i /255.0f;
        /* Primer Mapa de Color */
        uchar b = static_cast<uchar>(255 * (1 - t));
        uchar g = static_cast<uchar>(255*t);
        uchar r = 128;

        /* Segundo Mapa de Color */
        // uchar b = static_cast<uchar>(255 * (1 - t));
        // uchar g = 64;
        // uchar r = static_cast<uchar>(255*t);
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
        imshow("Imagen en Escala de Grises", image_gray);
    }
    else {
        image_gray = image.clone();
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                uchar gray_value = image.at<uchar>(i, j);
                Vec3b color_value = color_map[gray_value];
                image_transformed.at<Vec3b>(i, j) = color_value;
            }
        }
    }
    return image_transformed;
}

void apply_color_map_channels(const Mat& image, const vector<Vec3b>& color_map) {
    int rows = image.rows;
    int cols = image.cols;

    Mat color_map_channel_red(rows, cols, CV_8UC3);
    Mat color_map_channel_green(rows, cols, CV_8UC3);
    Mat color_map_channel_blue(rows, cols, CV_8UC3);
    Mat imaged_transformed(rows, cols, CV_8UC3);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int channel_blue = image.at<Vec3b>(i, j)[0];
            int channel_green = image.at<Vec3b>(i, j)[1];
            int channel_red = image.at<Vec3b>(i, j)[2];

            Vec3b color_value_blue = color_map[channel_blue];
            Vec3b color_value_green = color_map[channel_green];
            Vec3b color_value_red = color_map[channel_red];

            color_map_channel_blue.at<Vec3b>(i, j) = color_value_blue;
            color_map_channel_green.at<Vec3b>(i, j) = color_value_green;
            color_map_channel_red.at<Vec3b>(i, j) = color_value_red;

            Vec3b color_avg;
            for (int k = 0; k < 3; k++) {
                color_avg[k] = static_cast<uchar>((color_value_blue[k] + color_value_green[k] + color_value_red[k]) / 3);
            }
            imaged_transformed.at<Vec3b>(i, j) = color_avg;
        }
    }

    imshow("Canal Azul con Mapa de Color", color_map_channel_blue);
    imshow("Canal Verde con Mapa de Color", color_map_channel_green);
    imshow("Canal Rojo con Mapa de Color", color_map_channel_red);
    imshow("Imagen Transformada Unida", imaged_transformed);
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
    apply_color_map_channels(image_original, color_map);
    imshow("Imagen Original", image_original);
    imshow("Imagen Transformada", image_transformed);
    waitKey(0);
    return 0;
}