#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

Mat convert_gray(const Mat& frame_captured) {
    Mat frame_gray(frame_captured.rows, frame_captured.cols, CV_8UC1);
    int rows = frame_captured.rows;
    int columns = frame_captured.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
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
    int columns = frame_gray.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            uchar pixel_value = frame_gray.at<uchar>(i, j);
            frame_binarized.at<uchar>(i, j) = (pixel_value > threshold) ? 255 : 0;
        }
    }

    return frame_binarized;
}

Point process_frame_trajectory(const Mat& frame_current, const Mat& previous_frame, int threshold, const string& path, int count_frame) {
    Mat frame_binarized, frame_binarized_copy, frame_eroded;
    
    absdiff(frame_current, previous_frame, frame_binarized);
    frame_binarized = binarized_frame(frame_binarized, threshold);
    frame_binarized_copy = frame_binarized.clone();
    
    Mat structure = getStructuringElement(MORPH_RECT, Size(5, 5));
    erode(frame_binarized, frame_eroded, structure);

    // imwrite(path + "Frames/binarized_frame_" + to_string(count_frame) + ".png", frame_binarized_copy);
    // imwrite(path + "Eroded/eroded_frame_" + to_string(count_frame) + ".png", frame_eroded);
    // imshow("Binarized Frame", frame_binarized);

    vector<vector<Point>> contours;
    findContours(frame_binarized, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (!contours.empty()) {
        int contour_index = 0;
        double largest_area = 0;

        for (int i = 0; i < contours.size(); i++) {
            double area = contourArea(contours[i]);
            if (area > largest_area) {
                largest_area = area;
                contour_index = i;
            }
        }

        if (largest_area > 100) {
            Moments m = moments(contours[contour_index]);
            if (m.m00 > 0) {
                return Point((int)(m.m10 / m.m00), (int)(m.m01 / m.m00));
            }
        }
    }

    return Point(-1, -1);
}

int main() {
    VideoCapture video(0); // Para cámara en vivo
    //VideoCapture video("D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 2/Videos/video2.mp4");

    string trajectory_path = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 2/";
    string frame_binarized_path = trajectory_path + "Frames/";
    string frame_eroded_path = trajectory_path + "Eroded/";

    Mat frame_captured, frame_gray, previous_frame_gray;
    vector<Point> trajectory;

    namedWindow("Detection", cv::WINDOW_AUTOSIZE);
    int threshold_value = 10;
    int frame_count = 0;

    while (true) {
        video >> frame_captured;
        if (frame_captured.empty()) break;

        resize(frame_captured, frame_captured, Size(1280, 720));
        frame_gray = convert_gray(frame_captured);

        if (!previous_frame_gray.empty()) {
            Point center = process_frame_trajectory(frame_gray, previous_frame_gray, threshold_value, trajectory_path, frame_count);
            if (center.x != -1 && center.y != -1) {
                trajectory.push_back(center);
                circle(frame_captured, center, 5, Scalar(255, 0, 0), -1);
            }
        }

        previous_frame_gray = frame_gray.clone();
        frame_count++;

        for (size_t i = 1; i < trajectory.size(); i++) {
            line(frame_captured, trajectory[i - 1], trajectory[i], Scalar(0, 255, 0), 2);
        }

        imshow("Detection", frame_captured);
        if (waitKey(30) >= 27) break;
    }

    video.release();

    Mat trajectory_image(720, 1280, CV_8UC3, Scalar(255, 255, 255));
    for (size_t i = 1; i < trajectory.size(); i++) {
        line(trajectory_image, trajectory[i - 1], trajectory[i], Scalar(0, 0, 255), 2);
    }

    imshow("Result trajectory", trajectory_image);
    imwrite(trajectory_path + "trajectory_image.png", trajectory_image);

    waitKey(0);
    destroyAllWindows();

    return 0;
}