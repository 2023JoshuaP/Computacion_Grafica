#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Crear imagen negra con texto
    cv::Mat img = cv::Mat::zeros(300, 600, CV_8UC3);
    cv::putText(img, "OpenCV funcionando!", cv::Point(50, 150),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);

    // Mostrar imagen
    cv::imshow("Ventana de prueba", img);
    cv::waitKey(0); // Espera una tecla
    return 0;
}
