#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

vector<int> generate_histogram(const Mat& image) {
    vector<int> histogram(256, 0);
    int rows = image.rows;
    int columns = image.cols;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int pixel_value = image.at<uchar>(i, j);
            histogram[pixel_value]++;
        }
    }
    return histogram;;
}

void save_histogram(const vector<int>& histogram, const string& filename) {
    ofstream file(filename);
    for (size_t i = 0; i < histogram.size(); i++) {
        file << i << " " << histogram[i] << endl;
    }
    file.close();
}

Mat equalization(const Mat& image, const vector<int>& histogram) {
    Mat image_equalized = image.clone();
    int total_pixels = image.rows * image.cols;
    vector<uchar> f(256);
    
    int acumulado = histogram[0];
    f[0] = 0;
    for (int i = 1; i < 255; i++) {
        f[i] = (uchar)((255.0f * acumulado) / total_pixels);
        acumulado += histogram[i];
    }
    f[255] = 255;
    
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            uchar pixel_value = image.at<uchar>(i, j);
            image_equalized.at<uchar>(i, j) = f[pixel_value];
        }
    }

    return image_equalized;
}

Mat binarized(const Mat& image, int threshold) {
    Mat image_binarized = image.clone();
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            uchar pixel_value = image.at<uchar>(i, j);
            if (pixel_value > threshold) {
                image_binarized.at<uchar>(i, j) = 255;
            }
            else {
                image_binarized.at<uchar>(i, j) = 0;
            }
        }
    }
    return image_binarized;
}

int main() {
    string image;
    cout << "Enter the name file image (with extension): ";
    cin >> image;
    string path_image = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 1/Imagenes con OpenCV/Imagenes/Equalization Histogram Images/" + image;
    string path_histogram = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 1/Imagenes con OpenCV/Imagenes/Equalization Histogram Images/";
    Mat image_original = imread(path_image, IMREAD_GRAYSCALE);
    
    if (image_original.empty()) {
        cerr << "Error loading image: " << path_image << endl;
        return -1;
    }

    imshow("Image Original", image_original);

    vector<int> histogram = generate_histogram(image_original);
    save_histogram(histogram, path_histogram + "histogram.txt");

    Mat image_equalized = equalization(image_original, histogram);
    vector<int> histogram_equalized = generate_histogram(image_equalized);
    save_histogram(histogram_equalized, path_histogram + "histogram_equalized.txt");
    imshow("Image Equalized", image_equalized);

    Mat image_binarized = binarized(image_equalized, 128);
    imshow("Image Binarized", image_binarized);

    waitKey(0);
    return 0;
}

/*
----------------------------------------
Corresponding Threshold

imagen_0.png = 128
imagen_1.png = 128
imagen_2.png = 128
imagen_3.png = 128
imagen_4.png = 128
imagen_5.png = 128
----------------------------------------
*/