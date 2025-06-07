#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat convert_gray(const Mat& frame_captured) {
    Mat frame_gray(frame_captured.rows, frame_captured.cols, CV_8UC1);
    int rows = frame_captured.rows;
    int colums = frame_captured.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < colums; j++) {
            Vec3b pixel_value = frame_captured.at<Vec3b>(i, j);
            uchar gray_value = (uchar)(0.21 * pixel_value[2] + 0.72 * pixel_value[1] + 0.07 * pixel_value[0]);
            frame_gray.at<uchar>(i, j) = gray_value;
        }
    }

    return frame_gray;
}

Mat binarized_frame(const Mat& frame_gray, int threshold) {
    Mat frame_binarized(frame_gray.rows, frame_gray.cols, CV_8UC1);
    int rows = frame_gray.rows;
    int colums = frame_gray.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < colums; j++) {
            uchar pixel_value = frame_gray.at<uchar>(i, j);
            if (pixel_value > threshold)
                frame_binarized.at<uchar>(i, j) = 255;
            else
                frame_binarized.at<uchar>(i, j) = 0;
        }
    }

    return frame_binarized;
}

int main() {
    VideoCapture video("D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 2/Videos/video2.mp4");
    string trajectory_path = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 2/";

    Mat frame_captured, frame_gray, frame_binarized, previous_frame_gray;
    vector<Point> trajectory;

    namedWindow("Detection", cv::WINDOW_AUTOSIZE);
    int threshold_value = 100;

    while (true) {
        video >> frame_captured;
        if (frame_captured.empty()) {
            break;
        }
        
        resize(frame_captured, frame_captured, Size(1280, 720));
        frame_gray = convert_gray(frame_captured);

        if (!previous_frame_gray.empty()) {
            absdiff(frame_gray, previous_frame_gray, frame_binarized);
            frame_binarized = binarized_frame(frame_binarized, threshold_value);

            imshow("Binarized Frame", frame_binarized);
        }

        previous_frame_gray = frame_gray.clone();

        if (waitKey(30) >= 27) {
            break;
        }
    }

    video.release();
    destroyAllWindows();

    return 0;
}