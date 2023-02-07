#ifndef PDFCONVERTER_H
#define PDFCONVERTER_H

#include <string>
#include <vector>

#include <Magick++.h>

#include "CustomLogger.h"
#include "Constants.h"


extern CustomLogger g_logger;


// Конвертирует pdf в картинку
class PdfConverter
{
private:
	static const std::string FILE_PREFIX;

	const char* m_pathDll;
	

public:
	PdfConverter(const char* pathDll) : m_pathDll(pathDll)
	{
		g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
			std::string("PdfConverter has been created. The path of the shared library: " + std::string(m_pathDll)).c_str());
	}

	std::vector<std::string> convert(const std::string& inFileName, const std::string& extension);
	std::vector<std::string> convert(const void* data, const size_t length, const std::string& extension);
	void convert(const std::string& inFileName, const std::string& extension, std::vector<std::string>& outFileName);
	void convert(const void* data, const size_t length, const std::string& extension, std::vector<std::string>& outFileName);

};

#endif // !PDFCONVERTER_H
