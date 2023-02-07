#ifndef SERVERCONSTANTS_H
#define SERVERCONSTANTS_H

#include <string>


namespace ServerConstants
{
	const unsigned int PORT          { 9090 };
	const char* const PATH_DLL       { "./" };
	const char* const LOG_FILE       { "../source/log.txt" };

	// Эндпоинты
	const std::string API_HELLOWORLD { "/api/helloworld" };
	const std::string API_READIMAGE  { "/api/qr-image" };

	// Префиксы файлов
	const std::string PREFIX_PATH    { "../source/" };

}

namespace HeaderConstants
{
	const std::string ACCEPT         { "Accept" };
	const std::string CONTENT_TYPE   { "Content-Type" };
	const std::string CONTENT_LENGTH { "Content-Length" };
	const std::string CONNECTION     { "Connection" };

	const std::string TYPE_TXT_PLAIN { "text/plain" };
	const std::string TYPE_IMG_PNG   { "image/png" };
	const std::string TYPE_IMG_JPEG  { "image/jpeg" };
	const std::string TYPE_APP_PDF   { "application/pdf" };
	const std::string CLOSE          { "close" };

}

#endif // !SERVERCONSTANTS_H
