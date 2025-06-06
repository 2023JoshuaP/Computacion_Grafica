#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat convert_gray(const Mat& frame) {
    Mat frame_gray(frame.rows, frame.cols, CV_8UC1);
    int rows = frame.rows;
    int colums = frame.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < colums; j++) {
            Vec3b pixel_value = frame.at<Vec3b>(i, j);
            uchar gray_value = (uchar)(0.21 * pixel_value[2] + 0.72 * pixel_value[1] + 0.07 * pixel_value[0]);
            frame_gray.at<uchar>(i, j) = gray_value;
        }
    }

    return frame_gray;
}

Mat binarized_frame(const Mat& frame, int threshold) {
    Mat frame_binarized(frame.rows, frame.cols, CV_8UC1);
    int rows = frame.rows;
    int colums = frame.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < colums; j++) {
            uchar pixel_value = frame.at<uchar>(i, j);
            if (pixel_value > threshold) 
                frame_binarized.at<uchar>(i, j) = 255;
            else 
                frame_binarized.at<uchar>(i, j) = 0;
        }
    }

    return frame_binarized;
}

int main() {
    VideoCapture cap(0);
    Mat frame_capture, frame_gray, frame_binarized;
    vector<Point> trajectory;
    int threshold_value;

    while (true) {
        cap >> frame_capture;
        if (frame_capture.empty()) {
            cerr << "Error: Could not capture frame." << endl;
            break;
        }

        resize(frame_capture, frame_capture, Size(640, 480));
        frame_gray = convert_gray(frame_capture);
        threshold_value = threshold(frame_gray, frame_binarized, 0, 255, THRESH_BINARY | THRESH_OTSU);
        frame_binarized = binarized_frame(frame_gray, (int)threshold_value);

        // imshow("Frame Capture", frame_capture);
        imshow("Frame Gray", frame_gray);
        // imshow("Frame Binarized", frame_binarized);
        if (waitKey(1) == 'q') {
            cout << "Exiting..." << endl;
            break;
        }
    }

    cap.release();
    destroyAllWindows();
    
    return 0;
}