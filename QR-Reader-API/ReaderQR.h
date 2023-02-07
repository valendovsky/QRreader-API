#ifndef READERQR_H
#define READERQR_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>

#include "CustomLogger.h"
#include "Constants.h"


extern CustomLogger g_logger;


// Класс определяет наличие QR-кода на картинке и читает его
class ReaderQR
{
private:
	static const std::string s_windowName;
	static const std::string s_rectifiedWindow;
	static const std::vector<std::uint8_t> s_emptyVec;

	const std::string m_fileName;
	const std::vector<std::uint8_t>& m_fileContent;

	cv::Mat m_getImage;
	cv::Mat m_points;
	cv::Mat m_rectImage;

	void setLogLevel(const int logLevel);
	void display(cv::Mat& im, cv::Mat& points);
	std::string qrDecoder();


public:
	// Инициализация через имя файла с картинкой
	ReaderQR(const std::string& fileName, const int logLevel = 0)
		: m_fileName(fileName),
		m_fileContent(s_emptyVec) // пустой вектор
	{
		g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
			"ReaderQR: Initialization.");

		// Включение логирование OpenCV
		/*
		* LOG_LEVEL_SILENT = 0,              //!< for using in setLogVevel() call
		* LOG_LEVEL_FATAL = 1,               //!< Fatal (critical) error (unrecoverable internal error)
		* LOG_LEVEL_ERROR = 2,               //!< Error message
		* LOG_LEVEL_WARNING = 3,             //!< Warning message
		* LOG_LEVEL_INFO = 4,                //!< Info message
		* LOG_LEVEL_DEBUG = 5,               //!< Debug message. Disabled in the "Release" build.
		* LOG_LEVEL_VERBOSE = 6,             //!< Verbose (trace) messages. Requires verbosity level. Disabled in the "Release" build.
		*/
		setLogLevel(logLevel);
	}

	// Инициализация через передачу картинки, загруженной в буфер
	ReaderQR(const std::vector<std::uint8_t>& fileContent, const int logLevel = 0)
		: m_fileContent(fileContent),
		m_fileName(std::string())  // пустая строка
	{
		g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
			"ReaderQR: Initialization.");

		setLogLevel(logLevel);
	}

	std::string getQrContent();

};

#endif // !READERQR_H
