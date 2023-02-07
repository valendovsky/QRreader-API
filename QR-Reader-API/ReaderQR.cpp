#include "ReaderQR.h"

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "CustomLogger.h"
#include "Constants.h"


extern CustomLogger g_logger;

// Определяем статические объекты класса
const std::string ReaderQR::s_windowName{ "Preview" };
const std::string ReaderQR::s_rectifiedWindow{ "Rectified QR-code" };
const std::vector<std::uint8_t> ReaderQR::s_emptyVec{};


// Установка уровня логирования для OpenCV
void ReaderQR::setLogLevel(const int logLevel)
{
    if (logLevel)
    {
        g_logger.log(restbed::Logger::DEBUG, "ReaderQR::setLogLevel init with the opencv log level: Debug.");
        cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_VERBOSE);
    }
    else
    {
        g_logger.log(restbed::Logger::DEBUG, "ReaderQR::setLogLevel init with the opencv log level: Release.");
        cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);
    }
}

// Выводит QR-код на экран
void ReaderQR::display(cv::Mat& image, cv::Mat& points)
{
    g_logger.log(restbed::Logger::DEBUG, "ReaderQR::display RUN.");

    int index;
    int n = points.rows;
    for (index = 0; index < n; index++)
    {
        cv::line(image, cv::Point2i(points.at<float>(index, 0), points.at<float>(index, 1)),
            cv::Point2i(points.at<float>((index + 1) % n, 0), points.at<float>((index + 1) % n, 1)), cv::Scalar(255, 0, 0), 3);
    }

    cv::imshow(s_windowName, image);
}

// Декодирование QR-кода
std::string ReaderQR::qrDecoder()
{
    g_logger.log(restbed::Logger::DEBUG, "ReaderQR::qrDecoder RUN.");

    cv::QRCodeDetector qrDet = cv::QRCodeDetector::QRCodeDetector();
    g_logger.log(restbed::Logger::DEBUG, "ReaderQR::qrDecoder detect and decode the qr code.");
    std::string data = qrDet.detectAndDecode(m_getImage, m_points, m_rectImage);

    // Проверяем успешность обнаружения QR-кода
    if (data.length() > 0)
    {
        // Вывод содержимого QR-кода в консоль
        g_logger.log(restbed::Logger::INFO, 
            std::string("ReaderQR::qrDecoder: The data of qr code: " + data).c_str());

#ifdef _DEBUG
        // Визуальный вывод QR-кода
        // Выводит на экран исходную картинку
        g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
            "ReaderQR::qrDecoder shows a picture on a display.");
        display(m_getImage, m_points);

        // Выводит на экран QR-код
        g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
            "ReaderQR::qrDecoder shows a qr code on a display.");
        m_rectImage.convertTo(m_rectImage, CV_8UC3);
        cv::imshow(s_rectifiedWindow, m_rectImage);

        cv::waitKey(0);
#endif // _DEBUG
    }
    else
    {
        g_logger.log(restbed::Logger::WARNING, "ReaderQR::qrDecoder didn't detect a qr code.");
        data = ServerConstants::NONE;
    }

    return data;
}

// Определяет и считывет QR-код
std::string ReaderQR::getQrContent()
{
    g_logger.log(restbed::Logger::DEBUG, "ReaderQR::getQrContent RUN.");

    // Считываем картинку
    // из файла
    if (!m_fileName.empty())
    {
        g_logger.log(restbed::Logger::DEBUG, 
            std::string("ReaderQR::getQrContent read the image from the file: " + m_fileName).c_str());
        m_getImage = cv::imread(m_fileName.c_str());
    }
    // из буфера
    else if (!m_fileContent.empty())
    {
        g_logger.log(restbed::Logger::DEBUG, "ReaderQR::getQrContent read the image from the buffer.");
        m_getImage = cv::imdecode(m_fileContent, cv::IMREAD_COLOR);
    }
    else
    {
        g_logger.log(restbed::Logger::ERROR, "ReaderQR::getQrContent read nothing.");
        return ServerConstants::NONE;
    }

    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
        "ReaderQR::getQrContent decode the qr code.");

    return qrDecoder();
}
