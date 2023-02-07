#include "PdfConverter.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>

#include <Magick++.h>

#include "CustomLogger.h"
#include "Constants.h"


extern CustomLogger g_logger;

const std::string PdfConverter::FILE_PREFIX{ "../source/outFile_" };


// Конвертирует pdf-файл и возвращает адрес нового файла через параметр вывода
// std::vector<std::string>& outFileName - параметр вывода
void PdfConverter::convert(const std::string& inFileName, const std::string& extension, std::vector<std::string>& outFileName)
{
	g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(void from file) RUN.");

	// Инициализация пути до dll
	g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(void from file): Initialization.");
	Magick::InitializeMagick(m_pathDll);

	// Считываем PDF-файл в вектор
	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
		"PdfConverter::convert(void from file) write pdf file(s) to the vector.");
	std::vector<Magick::Image> images;
	Magick::readImages(&images, inFileName);

	std::ostringstream oss;
	oss << std::this_thread::get_id();

	try
	{
		// Конвертируем постранично в отдельные файлы
		g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(void from file) convert pdf file(s) to image(s).");
		for (int pageNum = 0; pageNum < images.size(); ++pageNum)
		{
			g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
				std::string("PdfConverter::convert(void from file) save the pdf page number: " + std::to_string(pageNum)).c_str());

			// Формируем имя новой картинки
			outFileName.push_back(std::string(FILE_PREFIX + oss.str() + "_" + std::to_string(pageNum) + "." + extension));

			images[pageNum].write(outFileName.back());
		}

		g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(void from file) converted pdf file(s) to image(s) successfully.");
	}
	catch (const std::exception& ex)
	{
		g_logger.log(restbed::Logger::ERROR, 
			std::string("PdfConverter::convert(void from file): Standard error: " + std::string(ex.what())).c_str());
		outFileName.pop_back();
	}
}

// Конвертирует pdf-файл и возвращает адрес нового файла
std::vector<std::string> PdfConverter::convert(const std::string& inFileName, const std::string& extension)
{
	g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(vector from file) RUN.");

	std::vector<std::string> imageNames;
	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
		"PdfConverter::convert(vector from file) convert image(s).");
	convert(inFileName, extension, imageNames);

	return imageNames;
}

// Конвертирует буфер с pdf-файлом и возвращает адрес нового файла через параметр вывода
// std::vector<std::string>& outFileName - параметр вывода
void PdfConverter::convert(const void* data, const size_t length, const std::string& extension, std::vector<std::string>& outFileName)
{
	g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(void from buf) RUN.");

	g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(void from buf): Initialization.");
	Magick::InitializeMagick(m_pathDll);

	std::vector<Magick::Image> images;
	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
		"PdfConverter::convert(void from buf) write pdf file(s) from the buffer to the vector.");
	Magick::Blob pdfBuf(data, length);
	Magick::readImages(&images, pdfBuf);

	std::ostringstream oss;
	oss << std::this_thread::get_id();

	try
	{
		g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(void from buf) convert pdf file(s) to image(s).");

		for (int pageNum = 0; pageNum < images.size(); ++pageNum)
		{
			g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
				std::string("PdfConverter::convert(void from buf) save the pdf page number: " + std::to_string(pageNum)).c_str());

			outFileName.push_back(std::string(FILE_PREFIX + oss.str() + "_" + std::to_string(pageNum) + "." + extension));

			images[pageNum].write(outFileName.back());
		}

		g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(void from buf) converted pdf file(s) to image(s) successfully.");
	}
	catch (const std::exception& ex)
	{
		g_logger.log(restbed::Logger::ERROR,
			std::string("PdfConverter::convert(void from buf): Standard error: " + std::string(ex.what())).c_str());
		outFileName.pop_back();
	}
}

// Конвертирует буфер с pdf-файлом и возвращает адрес нового файла
std::vector<std::string> PdfConverter::convert(const void* data, const size_t length, const std::string& extension)
{
	g_logger.log(restbed::Logger::DEBUG, "PdfConverter::convert(vector from buf) RUN.");

	std::vector<std::string> imageNames;
	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
		"PdfConverter::convert(vector from buf) convert image(s).");
	convert(data, length, extension, imageNames);

	return imageNames;
}
