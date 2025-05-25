#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

vector<int> calculate_histogram(const Mat& image) {
    vector<int> histogram(256, 0);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            int pixel_value = image.at<uchar>(i, j);
            histogram[pixel_value]++;
        }
    }
    return histogram;
}

void save_histogram(const vector<int>& histogram, const string& filename) {
    ofstream file_histogram(filename);
    for (int i = 0; i < histogram.size(); i++) {
        file_histogram << i << " " << histogram[i] << endl;
    }
    file_histogram.close();
}

int main() {
    string file_name;
    cout << "Poner imagen con extensión: ";
    cin >> file_name;
    string path_image = "Imagenes/" + file_name;

    Mat image_original = imread(path_image, IMREAD_GRAYSCALE);
    if (image_original.empty()) {
        cout << "No se pudo abrir la imagen." << endl;
        return -1;
    }

    imshow("Imagen Original", image_original);

    vector<int> histogram = calculate_histogram(image_original);
    save_histogram(histogram, "histograma.txt");
    waitKey(0);
    return 0;
}