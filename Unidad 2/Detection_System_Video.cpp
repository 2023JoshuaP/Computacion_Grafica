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
    string frame_binarized_path = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 2/Frames/";
    string frame_eroded_path = "D:/UNSA EPCC/7mo semestre/Computacion Grafica/Unidad 2/Eroded/";

    Mat frame_captured, frame_gray, frame_binarized, previous_frame_gray;
    vector<Point> trajectory;

    namedWindow("Detection", cv::WINDOW_AUTOSIZE);
    int threshold_value = 100; /* Para el caso de una moneda el umbral debe ser menor, minimo hasta 30 */
    int frame_count = 0;

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
            Mat frame_binarized_copy = frame_binarized.clone();
            Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
            erode(frame_binarized, frame_binarized, kernel);
            string binarized = frame_binarized_path + "binarized_frame_" + to_string(frame_count) + ".png";
            string eroded = frame_eroded_path + "eroded_frame_" + to_string(frame_count) + ".png";
            imwrite(binarized, frame_binarized_copy);
            imwrite(eroded, frame_binarized);
            imshow("Binarized Frame", frame_binarized);

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
                        Point center((int)(m.m10 / m.m00), (int)(m.m01 / m.m00));
                        trajectory.push_back(center);
                        circle(frame_captured, center, 5, Scalar(255, 0, 0), -1);
                    }
                }
            }
        }

        previous_frame_gray = frame_gray.clone();
        frame_count++;

        for (size_t i = 1; i < trajectory.size(); i++) {
            line(frame_captured, trajectory[i - 1], trajectory[i], Scalar(0, 255, 0), 2);
        }
        imshow("Detection", frame_captured);

        if (waitKey(30) >= 27) {
            break;
        }
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