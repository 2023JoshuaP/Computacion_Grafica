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
    imshow("Image Equalized", image_equalized);

    waitKey(0);
    return 0;
}