#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

// Mat process_erosion(const Mat& src, int kernel_size = 3) {}

// Mat process_dilatation(const Mat& src, int kernel_size = 3) {}

int main() {
    VideoCapture cap(0);
    Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            cerr << "Error al capturar el frame.\n";
            break;
        }
        imshow("Webcam", frame);
        if (waitKey(1) == 'q') {
            break;
        }
    }
    cap.release();
    destroyAllWindows();
    return 0;
}