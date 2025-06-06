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
        // imshow("Frame Gray", frame_gray);
        // imshow("Frame Binarized", frame_binarized);

        vector<vector<Point>> contours;
        findContours(frame_binarized, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        Point center;

        if (!contours.empty()) {
            double max_area = 0;
            int max_index = -1;
            for (size_t i = 0; i < contours.size(); i++) {
                double area = contourArea(contours[i]);
                if (area > max_area) {
                    max_area = area;
                    max_index = i;
                }
            }
            if (max_index >= 0) {
                Moments m = moments(contours[max_index]);
                if (m.m00 != 0) {
                    center = Point(m.m10 / m.m00, m.m01 / m.m00);
                    Mat mask(frame_gray.rows + 2, frame_gray.cols + 2, CV_8UC1, Scalar(0));
                    floodFill(frame_binarized, mask, center, Scalar(255), 0, Scalar(5), Scalar(5), FLOODFILL_MASK_ONLY | 4);
                    Mat mask_roi = mask(Rect(1, 1, frame_gray.cols, frame_gray.rows));
                    Moments fm = moments(mask_roi, true);
                    if (fm.m00 != 0) {
                        Point flood_center = Point(fm.m10 / fm.m00, fm.m01 / fm.m00);
                        trajectory.push_back(flood_center);
                        circle(frame_capture, flood_center, 5, Scalar(0, 255, 0), -1);
                    }
                }
            }
        }

        if (waitKey(1) == 'q') {
            cout << "Exiting..." << endl;
            break;
        }

        for (size_t i = 1; i < trajectory.size(); i++) {
            line(frame_capture, trajectory[i - 1], trajectory[i], Scalar(0, 0, 255), 2);
        }

        imshow("Trajectory", frame_capture);
    }

    cap.release();
    destroyAllWindows();
    
    return 0;
}