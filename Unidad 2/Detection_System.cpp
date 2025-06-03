#include <iostream>
#include <vector>
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

Mat process_erosion(const Mat& src, int kernel_size = 3) {
    Mat result = src.clone();
    int off_set = kernel_size / 2;
    for (int i = off_set; i < src.rows; i++) {
        for (int j = off_set; j < src.cols; j++) {
            uchar min_value = 255;
            for (int k = -off_set; k <= off_set; k++) {
                for (int l = -off_set; l <= off_set; l++) {
                    min_value = min(min_value, src.at<uchar>(i + k, j + l));
                }
            }
            result.at<uchar>(i, j) = min_value;
        }
    }
    return result;
}

Mat process_dilatation(const Mat& src, int kernel_size = 3) {
    Mat result = src.clone();
    int off_set = kernel_size / 2;
    for (int i = off_set; i < src.rows; i++) {
        for (int j = off_set; j < src.cols; j++) {
            uchar max_value = 0;
            for (int k = -off_set; k <= off_set; k++) {
                for (int l = -off_set; l <= off_set; l++) {
                    max_value = max(max_value, src.at<uchar>(i + k, j + l));
                }
            }
            result.at<uchar>(i, j) = max_value;
        }
    }
    return result;
}

int main() {
    VideoCapture cap(0);
    Mat frame, gray, binarized_frame, eroded_frame, dilated_frame, image_trajectory;
    vector<Point> trajectory;
    int threshold = 100;
    
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            cerr << "Error al capturar el frame.\n";
            break;
        }
        
        resize(frame, frame, Size(640, 480));
        gray = gray_scale(frame);
        binarized_frame = binarized(gray, threshold);
        eroded_frame = process_erosion(binarized_frame, 3);
        dilated_frame = process_dilatation(eroded_frame, 3);
        
        vector<vector<Point>> contours;
        findContours(eroded_frame, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        Point center(-1, -1);
        
        if (!contours.empty()) {
            double max_area = 0;
            for (auto& contour : contours) {
                Moments m = moments(contour);
                double area = contourArea(contour);
                if (area > max_area) {
                    max_area = area;
                    center = Point(m.m10 / m.m00, m.m01 / m.m00);
                }
            }
            if (center.x >= 0 && center.y >= 0) {
                trajectory.push_back(center);
                circle(frame, center, 5, Scalar(0, 255, 0), -1);
            }
        }
        
        imshow("Webcam", binarized_frame);
        imshow("Gray", gray);
        if (waitKey(1) == 'q') {
            break;
        }
    }
    cap.release();

    image_trajectory = Mat::zeros(frame.size(), CV_8UC3);
    for (size_t i = 1; i < trajectory.size(); ++i) {
        line(image_trajectory, trajectory[i - 1], trajectory[i], Scalar(255, 0, 0), 2);
    }
    imshow("Trajectory", image_trajectory);
    waitKey(0);

    //destroyAllWindows();
    return 0;
}