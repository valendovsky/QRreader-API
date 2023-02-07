// QR-Reader-API.cpp : Приложение для чтения QR-кодов, работающее через API
// Приложение читает:
//  - картинки форматов: jpg, png, pdf;
//  - ссылки на скачивание картинок типа: jpg, png, pdf;
//  - ссылки на html страницы, где расположены QR-коды.
// Сервер крайне подробно логирует свои действия
// Примеры шаблонов запросов к серверу через curl:
// curl -w'\n' -v -X POST -H "Accept: text/plain" -H "Content-Type: image/png" --data-binary "@C:\\address\\pngFile.png" "http://localhost:9090/api/qr-image"
// curl -w'\n' -v -X POST -H "Accept: text/plain" -H "Content-Type: image/jpeg" --data-binary "@C:\\address\\jpgFile.jpg" "http://localhost:9090/api/qr-image"
// curl -w'\n' -v -X POST -H "Accept: text/plain" -H "Content-Type: application/pdf" --data-binary "@C:\\address\\pdfFile.pdf" "http://localhost:9090/api/qr-image"
// curl -w'\n' -v -X POST -H "Accept: text/plain" -H "Content-Type: text/plain" --data "http://link" "http://localhost:9090/api/qr-image"
//

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <exception>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cstdio>

#include <restbed>

#include "ServerConstants.h"
#include "Constants.h"
#include "ExtensionConstants.h"
#include "ReaderQR.h"
#include "Loader.h"
#include "HtmlConverter.h"
#include "PdfConverter.h"
#include "CustomLogger.h"

#undef ERROR


// Логер
CustomLogger g_logger;
// Конвертер для html-страниц
HtmlConverter g_htmlConvert;


// Чтение содержимого QR-кода из картинки в буфере
std::string decodeImage(const restbed::Bytes& body)
{
    g_logger.log(restbed::Logger::DEBUG, "decodeImage RUN.");

    // Декодируем qr-код
    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "decodeImage decode the qr code from the image.");
    ReaderQR reader(body);

    return reader.getQrContent();
}

// Чтение содержимого QR-кода из списка файлов с картинками
std::string decodeListImages(const std::vector<std::string>& images)
{
    g_logger.log(restbed::Logger::DEBUG, "decodeListImages RUN.");

    // По умолчанию не найдено
    std::string result{ ServerConstants::NONE };

    // Проверяем наличие списка файлов
    if (images.empty())
    {
        g_logger.log(restbed::Logger::WARNING, 
            "decodeListImages: The vector with images is empty. The result of the decode is \"none\".");
        return result;
    }

    // Декодируем qr-код
    g_logger.log(restbed::Logger::DEBUG, "decodeListImages decode qr code(s) from the vector of the images.");
    for (int index = 0; index < images.size(); ++index)
    {
        ReaderQR reader(images[index]);
        result = reader.getQrContent();

        if (result != ServerConstants::NONE && result != "")
        {
            g_logger.log(restbed::Logger::DEBUG, 
                std::string("decodeListImages found qr code at the image number: " + std::to_string(index)).c_str());
            g_logger.log(restbed::Logger::INFO, 
                std::string("decodeListImages: The content of the qr code: " + result).c_str());
            break;
        }

        g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
            std::string("decodeListImages decoded the image number #" + std::to_string(index) + " without qr code.").c_str());
    }

    if (result == "")
    {
        g_logger.log(restbed::Logger::WARNING,
            "decodeListImages can't to find the qr code. The result of the decode is \"none\".");
        result = ServerConstants::NONE;
    }

    // Удаляем временные файлы
    g_logger.log(restbed::Logger::DEBUG, "decodeListImages delete cache image(s).");
    for (const auto image : images)
    {
        remove(image.c_str());
    }

    return result;
}

// Возвращает содержание QR-кода из буфера с pdf
std::string decodePdfBuf(const restbed::Bytes& body)
{
    g_logger.log(restbed::Logger::DEBUG, "decodePdfBuf RUN.");

    // Конвертируем pdf в png
    g_logger.log(restbed::Logger::DEBUG, "decodePdfBuf convert the pdf file from the buffer to the png format.");
    PdfConverter pdfToPng(ServerConstants::PATH_DLL);
    std::vector<std::string> images = pdfToPng.convert(static_cast<const void*>(body.data()), body.size(), "PNG");

    g_logger.log(restbed::Logger::DEBUG, "decodePdfBuf read qr code(s) from the png file(s) from the buffer.");
    return decodeListImages(images);
}

// Возвращает содержание QR-кода из pdf-файла
std::string decodePdfFile(const std::string& inFileName)
{
    g_logger.log(restbed::Logger::DEBUG, "decodePdfFile RUN.");

    // Конвертируем pdf в png
    g_logger.log(restbed::Logger::DEBUG, 
        std::string("decodePdfFile convert the pdf file with the name: " + inFileName + " to the png format.").c_str());
    PdfConverter pdfToPng(ServerConstants::PATH_DLL);
    std::vector<std::string> images = pdfToPng.convert(inFileName, "PNG");

    g_logger.log(restbed::Logger::DEBUG, "decodePdfFile read qr code(s) from the png file(s) from the buffer.");
    return decodeListImages(images);
}

// Конвертирует html-страницу в pdf-файл
std::string convertHtmlToPdf(const std::string& htmlContent)
{
    g_logger.log(restbed::Logger::DEBUG, "convertHtmlToPdf RUN.");

    // Формируем имя html файла
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    const std::string htmlFileName{ ServerConstants::PREFIX_PATH + "htmlToPdf_" + oss.str() + Extension::EXT_HTML };
    g_logger.log(restbed::Logger::DEBUG, 
        std::string("convertHtmlToPdf created the name of the html cache-file: " + htmlFileName).c_str());

    // Сохраняем html в файл
    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "convertHtmlToPdf write the html cache-file.");
    std::ofstream htmlFile(htmlFileName, std::ios::binary);
    if (!htmlFile.is_open())
    {
        g_logger.log(restbed::Logger::ERROR, "convertHtmlToPdf can not save html cache-file.");
        return ServerConstants::NONE;
    }
    htmlFile << htmlContent;
    htmlFile.close();

    std::string pdfFileName = g_htmlConvert.getImage(htmlFileName, HtmlConverter::PDF);
    g_logger.log(restbed::Logger::DEBUG, 
        std::string("convertHtmlToPdf converted the html cache-file to pdf cache-file with the name: " + pdfFileName).c_str());
    
    // Удаление временного html файла
    remove(htmlFileName.c_str());
    g_logger.log(restbed::Logger::DEBUG, std::string("convertHtmlToPdf deleted html cache-file: " + htmlFileName).c_str());

    return pdfFileName;
}

// Чтение содержимого QR-кода по ссылке
std::string decodeLink(const restbed::Bytes& body)
{
    g_logger.log(restbed::Logger::DEBUG, "decodeLink RUN.");

    // Получаем ссылку
    std::string linkContent{ body.data(), body.data() + body.size() };
    g_logger.log(restbed::Logger::DEBUG, std::string("decodeLink: got the link: " + linkContent).c_str());


    // Загружаем ссылку
    g_logger.log(restbed::Logger::DEBUG, "decodeLink download the link.");
    Loader dl(linkContent);
    std::map<std::string, std::string> fileInfo = dl.download(Loader::TO_BUF);

    if (fileInfo.at(Loader::STATUS) == ServerConstants::NONE)
    {
        g_logger.log(restbed::Logger::ERROR, "decodeLink: The download failed.");
        return ServerConstants::NONE;
    }


    // Декодируем qr-код
    g_logger.log(restbed::Logger::DEBUG, "decodeLink decode qr code.");
    std::string response{ ServerConstants::NONE };  // по умолчанию
    if (fileInfo.at(Loader::STATUS) == Loader::STATUS_BUFFER)
    {
        std::vector<std::uint8_t> imageQr{ fileInfo.at(Loader::CONTENT).begin(), fileInfo.at(Loader::CONTENT).end() };
        // Декодирование в зависимости от типа ссылки
        if (fileInfo.at(Loader::EXTENSION) == Extension::EXT_HTML)
        {
            // Конвертируем html в pdf
            g_logger.log(restbed::Logger::DEBUG, "decodeLink convert html to pdf.");

            std::string pdfFileName = convertHtmlToPdf(fileInfo.at(Loader::CONTENT));
            if (pdfFileName == ServerConstants::NONE)
            {
                g_logger.log(restbed::Logger::WARNING, "decodeLink: The convert failed, return \"none\".");
                return ServerConstants::NONE;
            }

            g_logger.log(restbed::Logger::DEBUG, "decodeLink decode the pdf file.");
            response = decodePdfFile(pdfFileName);

            // Удаление pdf файла
            remove(pdfFileName.c_str());
            g_logger.log(restbed::Logger::DEBUG, "decodeLink removed the pdf cache-file.");
        }
        else if (fileInfo.at(Loader::EXTENSION) == Extension::EXT_PDF)
        {
            g_logger.log(restbed::Logger::DEBUG, "decodeLink decode the pdf file from the buffer.");
            response = decodePdfBuf(imageQr);
        }
        else
        {
            g_logger.log(restbed::Logger::DEBUG, "decodeLink decode the image.");
            response = decodeImage(imageQr);
        }
    }

    g_logger.log(restbed::Logger::INFO, std::string("decodeLink: the result of the decode: " + response).c_str());

    return response;
}

// Получает картинку из тела запроса и возвращает ответ
void getBodyRequest(const std::shared_ptr<restbed::Session> session, std::string (*fncPtr)(const restbed::Bytes& body))
{
    g_logger.log(restbed::Logger::DEBUG, "getBodyRequest RUN.");

    // Распарсим тело запроса
    const auto request = session->get_request();
    size_t contentLength = request->get_header(HeaderConstants::CONTENT_LENGTH, 0);
    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        std::string("getBodyRequest: Request body length: " + std::to_string(contentLength)).c_str());

    session->fetch(contentLength, [request, fncPtr](const std::shared_ptr<restbed::Session> session, const restbed::Bytes& body)
        {
            g_logger.log(restbed::Logger::DEBUG, "getBodyRequest parsing the request body.");

            // Считываем поученную картинку
            std::string response = fncPtr(body);

            // Проверка успешности декодирования
            if (response == ServerConstants::NONE)
            {
                g_logger.log(restbed::Logger::WARNING, 
                    std::string("getBodyRequest: the parse of the request body failed. Content: " + response).c_str());

                session->close(restbed::BAD_REQUEST);

                g_logger.log(restbed::Logger::INFO, "getBodyRequest: Response - 404 BAD REQUEST");
            }
            else
            {
                g_logger.log(restbed::Logger::DEBUG, "getBodyRequest: the parse of the request body successfully.");
                
                session->close(restbed::CREATED, response,
                    {
                        {HeaderConstants::CONTENT_LENGTH, std::to_string(response.length())},
                        {HeaderConstants::CONTENT_TYPE, HeaderConstants::TYPE_TXT_PLAIN}
                    }
                );

                g_logger.log(restbed::Logger::INFO,
                    std::string("getBodyRequest: Response status: " + std::to_string(restbed::CREATED) + ", message: " + response).c_str());
            }
        }
    );
}


// Не подходит ни один фильтр хендлера
void failedHandler(const std::shared_ptr<restbed::Session> session)
{
    g_logger.log(restbed::Logger::DEBUG, "failedHandler RUN.");
    session->close(restbed::BAD_REQUEST);
    g_logger.log(restbed::Logger::INFO, "failedHandler: The user sent bad headers. Response - 404 BAD REQUEST");
}

// Принимает картинку формата png
void readPngHandler(const std::shared_ptr<restbed::Session> session)
{
    g_logger.log(restbed::Logger::DEBUG, "readPngHandler RUN.");
    getBodyRequest(session, decodeImage);
}

// Принимает картинку формата jpeg
void readJpegHandler(const std::shared_ptr<restbed::Session> session)
{
    g_logger.log(restbed::Logger::DEBUG, "readJpegHandler RUN.");
    getBodyRequest(session, decodeImage);
}

// Принимает картинку формата pdf
void readPdfHandler(const std::shared_ptr<restbed::Session> session)
{
    g_logger.log(restbed::Logger::DEBUG, "readPdfHandler RUN.");
    getBodyRequest(session, decodePdfBuf);
}

// Принимает ссылку на qr-код
void readLinkHandler(const std::shared_ptr<restbed::Session> session)
{
    g_logger.log(restbed::Logger::DEBUG, "readLinkHandler RUN.");
    getBodyRequest(session, decodeLink);
}


int main()
{
    // Вывод информации в консоль о начале работы
    std::cout << "QR Reader API running." << std::endl;
    g_logger.setLogLevel(1);
    std::cout << "Logging level: " << g_logger.getLogLevel() << std::endl;


    // Перевод логирующего потока в файл
    freopen(ServerConstants::LOG_FILE, "a", stderr);
    // Логирование информации о начале работы сервера
    g_logger.log(restbed::Logger::SECURITY, "main: QR Reader API running.");


    // Порт сервера
    unsigned int port = ServerConstants::PORT;


    // Эндпоинты сервера
    //
    // Эндпоинт принимающий картинки с qr-кодом
    auto readImage = std::make_shared<restbed::Resource>();
    readImage->set_path(ServerConstants::API_READIMAGE);
    readImage->set_failed_filter_validation_handler(failedHandler);
    readImage->set_method_handler("POST", 
        { 
            {HeaderConstants::ACCEPT, HeaderConstants::TYPE_TXT_PLAIN},
            {HeaderConstants::CONTENT_TYPE, HeaderConstants::TYPE_IMG_PNG}
        }, readPngHandler);
    readImage->set_method_handler("POST",
        {
            {HeaderConstants::ACCEPT, HeaderConstants::TYPE_TXT_PLAIN},
            {HeaderConstants::CONTENT_TYPE, HeaderConstants::TYPE_IMG_JPEG}
        }, readJpegHandler);
    readImage->set_method_handler("POST",
        {
            {HeaderConstants::ACCEPT, HeaderConstants::TYPE_TXT_PLAIN},
            {HeaderConstants::CONTENT_TYPE, HeaderConstants::TYPE_APP_PDF}
        }, readPdfHandler);
    readImage->set_method_handler("POST", 
        {
            {HeaderConstants::ACCEPT, HeaderConstants::TYPE_TXT_PLAIN},
            {HeaderConstants::CONTENT_TYPE, HeaderConstants::TYPE_TXT_PLAIN}
        }, readLinkHandler);


    // Настройки сервера
    auto settings = std::make_shared<restbed::Settings>();
    settings->set_port(port);
    settings->set_default_header(HeaderConstants::CONNECTION, HeaderConstants::CLOSE);


    // Инициализация ключей
    restbed::Service service;
    service.publish(readImage);
    service.set_logger(std::make_shared<CustomLogger>());
    

    // Запуск сервера
    g_logger.log(restbed::Logger::SECURITY, "main: Service start.");
    service.start(settings);

    g_logger.log(restbed::Logger::DEBUG, "QR Reader API has finished.");
    std::cin.get();

    return 0;
}
