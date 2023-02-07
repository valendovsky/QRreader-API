#include "Loader.h"

#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <sstream>
#include <functional>

#include <curl/curl.h>

#include "Constants.h"
#include "ExtensionConstants.h"


// Определение статических членов класса
const std::string Loader::STATUS{ "status" };
const std::string Loader::NAME{ "fileName" };
const std::string Loader::EXTENSION{ "extension" };
const std::string Loader::CONTENT{ "content" };

const std::string Loader::STATUS_FILE{ "file" };
const std::string Loader::STATUS_BUFFER{ "buffer" };


// Определяет расширение для файла ссылки
std::string Loader::getExtension()
{
	g_logger.log(restbed::Logger::DEBUG, "Loader::getExtension RUN.");

	// Ссылка слишком короткая
	if (m_fileLink.length() < Extension::EXT_JPEG.length())
	{
		g_logger.log(restbed::Logger::WARNING, "Loader::getExtension: The link is too small..");
		return ServerConstants::NONE;
	}

	// Определяет расширение для файла и возвращает его
	std::string extension(m_fileLink.end() - Extension::EXT_PNG.length(), m_fileLink.end());
	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
		std::string("Loader::getExtension get the file extension: " + extension).c_str());

	std::string logMsg;
	std::string fileExtension;
	if (extension == Extension::EXT_PNG ||
		extension == Extension::EXT_JPG ||
		extension == Extension::EXT_BMP ||
		std::string(m_fileLink.end() - Extension::EXT_JPEG.length(), m_fileLink.end()) == Extension::EXT_JPEG)
	{
		logMsg = "Loader::getExtension: The extension - image.";
		fileExtension = Extension::EXT_PNG;
	}
	else if (extension == Extension::EXT_PDF)
	{
		logMsg = "Loader::getExtension: The extension - PDF.";
		fileExtension = Extension::EXT_PDF;
	}
	else
	{
		logMsg = "Loader::getExtension: The extension - HTML.";
		fileExtension = Extension::EXT_HTML;
	}
	g_logger.log(restbed::Logger::DEBUG, logMsg.c_str());

	return fileExtension;
}

// Возвращает информацию по файлу (отделяет картинки от html страниц)
std::map<std::string, std::string> Loader::getFileName()
{
	g_logger.log(restbed::Logger::DEBUG, "Loader::getFileName RUN.");

	// Получаем расширения для файла
	std::map<std::string, std::string> fileInfo;
	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
		"Loader::getFileName get the file expression.");
	fileInfo[EXTENSION] = getExtension();
	if (fileInfo.at(EXTENSION) == ServerConstants::NONE)
	{
		g_logger.log(restbed::Logger::WARNING, "Loader::getFileName: Invalid extension.");
		return std::map<std::string, std::string>{ {STATUS, ServerConstants::NONE} };
	}

	// Формируем название файла
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	fileInfo[NAME] = std::string{ "../source/" + oss.str() + "_file" + fileInfo.at(EXTENSION) };
	fileInfo[STATUS] = STATUS_FILE;
	g_logger.log(restbed::Logger::DEBUG, 
		std::string("Loader::getFileName created the file name: " + fileInfo[NAME]).c_str());

	return fileInfo;
}

// Сохраняет тело запроса в файл
// FILE* outData - параметр вывода
static size_t writeFile(char* ptr, size_t size, size_t nmemb, FILE* outData)
{
	return fwrite(ptr, size, nmemb, outData);
}

// Сохраняет тело запроса в буфер
// std::string* outData - параметр вывода
static size_t writeBuf(char* ptr, size_t size, size_t nmemb, std::string* outData)
{
    if (outData)
    {
        outData->append(ptr, size * nmemb);
        return size * nmemb;
    }
    else
        return 0;
}

// Загрузка по сслыке
// T* outData - параметр вывода
template <typename T>
int Loader::loadLink(T* outData, size_t(*fncPtr)(char* ptr, size_t size, size_t nmemb, T* data))
{
	g_logger.log(restbed::Logger::DEBUG, "Loader::loadLink RUN.");

	CURL* curlHandle = curl_easy_init();
	if (curlHandle)
	{
		g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, "Loader::loadLink initialized curl handle.");

		g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
			std::string("Loader::loadLink set url: " + m_fileLink).c_str());
		curl_easy_setopt(curlHandle, CURLOPT_URL, m_fileLink.c_str());

		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, outData);
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, fncPtr);
		// Игнорируем заголовки
		curl_easy_setopt(curlHandle, CURLOPT_HEADER, 0);

		g_logger.log(restbed::Logger::DEBUG, "Loader::loadLink perform the request.");
		CURLcode response = curl_easy_perform(curlHandle);

		curl_easy_cleanup(curlHandle);

		return response;
	}

	g_logger.log(restbed::Logger::ERROR, "Loader::loadLink: The curl failed init.");
}

// Скачивание по ссылке в файл
// std::map<std::string, std::string>& outFileInfo - параметр вывода
void Loader::downloadToFile(std::map<std::string, std::string>& outFileInfo)
{
	g_logger.log(restbed::Logger::DEBUG, "Loader::downloadToFile RUN.");

	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
		"Loader::downloadToFile get the file name.");
	outFileInfo = getFileName();
	if (outFileInfo.at(STATUS) == ServerConstants::NONE)
	{
		g_logger.log(restbed::Logger::WARNING, "Loader::downloadToFile: Error get file name.");

		return;
	}

	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
		std::string("Loader::downloadToFile save the qr content into the file with the name: " + outFileInfo.at(NAME)).c_str());
	FILE* qrFile = fopen(outFileInfo.at(NAME).c_str(), "wb");
	if (qrFile == NULL)
	{
		g_logger.log(restbed::Logger::ERROR, "Loader::downloadToFile failed open the file.");
		outFileInfo[STATUS] = ServerConstants::NONE;

		return;
	}

	// Скачиваем по ссылке
	g_logger.log(restbed::Logger::DEBUG, "Loader::downloadToFile download the file.");
	int result = loadLink(qrFile, writeFile);
	fclose(qrFile);

	if (result != CURLE_OK)
	{
		g_logger.log(restbed::Logger::ERROR, "Loader::downloadToFile failed download the link.");
		outFileInfo[STATUS] = ServerConstants::NONE;

		return;
	}

	// Файл успешно загружен
	outFileInfo[STATUS] = STATUS_FILE;

	g_logger.log(restbed::Logger::DEBUG, "Loader::downloadToFile: The file downloaded successfully.");
}

// Скачивание по ссылке в буфер
// std::map<std::string, std::string>& outFileInfo - параметр вывода
void Loader::downloadToBuf(std::map<std::string, std::string>& outFileInfo)
{
	g_logger.log(restbed::Logger::DEBUG, "Loader::downloadToBuf RUN.");

	// Получаем тип ссылки
	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
		"Loader::downloadToBuf get the file extension.");
	outFileInfo[EXTENSION] = getExtension();
	if (outFileInfo.at(EXTENSION) == ServerConstants::NONE)
	{
		g_logger.log(restbed::Logger::WARNING, "Loader::downloadToBuf got the invalid extension.");
		outFileInfo[STATUS] = ServerConstants::NONE;

		return;
	}

	// Скачиваем по ссылке
	std::string linkContent;
	g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
		"Loader::downloadToBuf download a content from the link.");
	int result = loadLink(&linkContent, writeBuf);
	if (result != CURLE_OK)
	{
		g_logger.log(restbed::Logger::ERROR, "Loader::downloadToBuf failed download the link.");
		outFileInfo[STATUS] = ServerConstants::NONE;

		return;
	}

	outFileInfo[STATUS] = STATUS_BUFFER;
	outFileInfo[CONTENT] = linkContent;

	g_logger.log(restbed::Logger::DEBUG, "Loader::downloadToBuf: The link downloaded successfully.");
}

// Скачивает файл по предоставленной ссылке по умолчанию сохраняет в файл (TO_FILE)
std::map<std::string, std::string> Loader::download(const int flag)
{
	g_logger.log(restbed::Logger::DEBUG, "Loader::download RUN.");

	std::map<std::string, std::string> fileInfo;
	fileInfo[STATUS] = ServerConstants::NONE;

	if (flag == TO_FILE)
	{
		g_logger.log(restbed::Logger::DEBUG, "Loader::download download to the file.");
		downloadToFile(fileInfo);
	}
	else
	{
		g_logger.log(restbed::Logger::DEBUG, "Loader::download download to the buffer.");
		downloadToBuf(fileInfo);
	}

	return fileInfo;
}
