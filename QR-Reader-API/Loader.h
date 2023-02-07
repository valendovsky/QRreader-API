#ifndef LOADER_H
#define LOADER_H

#include <string>
#include <map>

#include <curl/curl.h>

#include "CustomLogger.h"
#include "Constants.h"


extern CustomLogger g_logger;


// Скачивает файл по предоставленной ссылке
class Loader
{
public:
	static const std::string STATUS;
	static const std::string NAME;
	static const std::string EXTENSION;
	static const std::string CONTENT;

	static const std::string STATUS_FILE;
	static const std::string STATUS_BUFFER;
	
	enum LoadType
	{
		TO_FILE = 0,
		TO_BUF = 1,

	};


private:
	// Адрес ссылки
	const std::string m_fileLink;

	std::string getExtension();
	std::map<std::string, std::string> getFileName();
	template <typename T>
	int loadLink(T* data, size_t(*fncPtr)(char* ptr, size_t size, size_t nmemb, T* data));
	void downloadToFile(std::map<std::string, std::string>& outFileInfo);
	void downloadToBuf(std::map<std::string, std::string>& outFileInfo);

	friend size_t writeFile(char* ptr, size_t size, size_t nmemb, FILE* data);


public:
	Loader(const std::string& fileLink) : m_fileLink(fileLink)
	{
		g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, "Loader: Initialization.");
	}

	std::map<std::string, std::string> download(const int flag = TO_FILE);

};

#endif // !LOADER_H
