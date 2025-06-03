#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

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

Mat gray_scale(const Mat& image) {
    Mat gray_image(image.rows, image.cols, CV_8UC1);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            Vec3b pixel = image.at<Vec3b>(i, j);
            uchar gray_value = static_cast<uchar>(0.21 * pixel[2] + 0.72 * pixel[1] + 0.07 * pixel[0]);
            gray_image.at<uchar>(i, j) = gray_value;
        }
    }
    return gray_image;
}

int main() {
    VideoCapture cap(0);
    Mat frame, gray, binarized_frame;
    int threshold = 128;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            cerr << "Error al capturar el frame.\n";
            break;
        }
        gray = gray_scale(frame);
        binarized_frame = binarized(gray, threshold);
        imshow("Webcam", binarized_frame);
        if (waitKey(1) == 'q') {
            break;
        }
    }
    cap.release();
    destroyAllWindows();
    return 0;
}