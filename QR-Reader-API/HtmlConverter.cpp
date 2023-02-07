#include "HtmlConverter.h"

#include <iostream>
#include <string>
#include <thread>
#include <sstream>

#include <wkhtmltox/image.h>
#include <wkhtmltox/pdf.h>

#include "CustomLogger.h"
#include "Constants.h"
#include "ExtensionConstants.h"


extern CustomLogger g_logger;

// Инициализация статических членов класса
bool HtmlConverter::m_init = false;


// Отображает информацию о загрузке
// Для image
static void progress_changed(wkhtmltoimage_converter* c, int p)
{
    printf("%3d%%\r", p);
    fflush(stdout);
}
// Для pdf
static void progress_changed(wkhtmltopdf_converter* c, int p)
{
    printf("%3d%%\r", p);
    fflush(stdout);
}

// Отображает фазу загрузки
// Для image
static void phase_changed(wkhtmltoimage_converter* c)
{
    int phase = wkhtmltoimage_current_phase(c);
    printf("%s\n", wkhtmltoimage_phase_description(c, phase));
}
// Для pdf
static void phase_changed(wkhtmltopdf_converter* c)
{
    int phase = wkhtmltopdf_current_phase(c);
    printf("%s\n", wkhtmltopdf_phase_description(c, phase));
}

// Выводит сообщение об ошибке
// Для image
static void error(wkhtmltoimage_converter* c, const char* msg)
{
    fprintf(stderr, "Error: %s\n", msg);
}
// Для pdf
static void error(wkhtmltopdf_converter* c, const char* msg)
{
    fprintf(stderr, "Error: %s\n", msg);
}

// Выводит предупреждающие сообщения
// Для image
static void warning(wkhtmltoimage_converter* c, const char* msg)
{
    fprintf(stderr, "Warning: %s\n", msg);
}
// Для pdf
static void warning(wkhtmltopdf_converter* c, const char* msg)
{
    fprintf(stderr, "Warning: %s\n", msg);
}

// Конвертирует html файл в картинку
int HtmlConverter::htmlToImage(const std::string& extension)
{
    g_logger.log(restbed::Logger::DEBUG, "HtmlConverter::htmlToImage RUN.");

    // Формируем название для файла
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    m_image = "../source/" + oss.str() + "_image" + extension;
    g_logger.log(restbed::Logger::DEBUG, 
        std::string("HtmlConverter::htmlToImage created the name of the image:" + m_image).c_str());
    
    wkhtmltoimage_global_settings* globalSettings;
    wkhtmltoimage_converter* converter;
    const unsigned char* data;
    long len;

    // Инициализация объекта глобальных настроек
    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToImage create the settings object.");
    globalSettings = wkhtmltoimage_create_global_settings();
    
    // Задаём файлы для преобразования и сохранения картинки
    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG,
        std::string("HtmlConverter::htmlToImage set the html page with name: " + m_pageFile).c_str());
    wkhtmltoimage_set_global_setting(globalSettings, "in", m_pageFile.c_str());
    wkhtmltoimage_set_global_setting(globalSettings, "out", m_image.c_str());

    // Инициализация конвертера
    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToImage create the converter object.");
    converter = wkhtmltoimage_create_converter(globalSettings, NULL);

    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToImage register functions of the change.");
    // Отображение изменения прогресса
    wkhtmltoimage_set_progress_changed_callback(converter, progress_changed);
    // Отображение фазы процесса изменения
    wkhtmltoimage_set_phase_changed_callback(converter, phase_changed);

    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToImage register the functions of the logging.");
    // Для сохранения ошибок при возникновении
    wkhtmltoimage_set_error_callback(converter, error);
    // Вывод предупреждений
    wkhtmltoimage_set_warning_callback(converter, warning);

    // Преобразование html в картинку
    g_logger.log(restbed::Logger::DEBUG, "HtmlConverter::htmlToImage convert html to the image.");
    if (!wkhtmltoimage_convert(converter))
    {
        g_logger.log(restbed::Logger::ERROR, "HtmlConverter::htmlToImage: The conversion html to image failed.");
    }

    // Вывод содержания возможной ошибки
    printf("httpErrorCode: %d\n", wkhtmltoimage_http_error_code(converter));
    len = wkhtmltoimage_get_output(converter, &data);
    printf("%ld len\n", len);

    // Освобождение конвертера
    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToImage destroy the converter object.");
    wkhtmltoimage_destroy_converter(converter);

    return 0;
}

// Конвертация html файл в pdf
int HtmlConverter::htmlToPdf()
{
    g_logger.log(restbed::Logger::DEBUG, "HtmlConverter::htmlToPdf RUN.");

    std::ostringstream oss;
    oss << std::this_thread::get_id();
    m_image = "../source/" + oss.str() + "_image.pdf";
    g_logger.log(restbed::Logger::DEBUG, 
        std::string("HtmlConverter::htmlToPdf created the name of the pdf image:" + m_image).c_str());

    wkhtmltopdf_global_settings* globalSettings;
    wkhtmltopdf_object_settings* objectSettings;
    wkhtmltopdf_converter* converter;

    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToPdf create the settings object.");
    globalSettings = wkhtmltopdf_create_global_settings();
    wkhtmltopdf_set_global_setting(globalSettings, "out", m_image.c_str());
    //wkhtmltopdf_set_global_setting(globalSettings, "load.cookieJar", "myjar.jar");

    objectSettings = wkhtmltopdf_create_object_settings();
    g_logger.log(restbed::Logger::DEBUG,
        std::string("HtmlConverter::htmlToPdf set the html page with name: " + m_pageFile).c_str());
    wkhtmltopdf_set_object_setting(objectSettings, "page", m_pageFile.c_str());

    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToPdf create the converter object.");
    converter = wkhtmltopdf_create_converter(globalSettings);

    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToPdf register functions of the change.");
    wkhtmltopdf_set_progress_changed_callback(converter, progress_changed);
    wkhtmltopdf_set_phase_changed_callback(converter, phase_changed);

    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToPdf register the functions of the logging.");
    wkhtmltopdf_set_error_callback(converter, error);
    wkhtmltopdf_set_warning_callback(converter, warning);

    wkhtmltopdf_add_object(converter, objectSettings, NULL);

    g_logger.log(restbed::Logger::DEBUG, "HtmlConverter::htmlToPdf convert html to pdf.");
    if (!wkhtmltopdf_convert(converter))
    {
        g_logger.log(restbed::Logger::ERROR, "HtmlConverter::htmlToPdf: The conversion html to pdf failed.");
    }

    printf("httpErrorCode: %d\n", wkhtmltopdf_http_error_code(converter));

    g_logger.log_if(ServerConstants::EXPRESSION, restbed::Logger::DEBUG, 
        "HtmlConverter::htmlToPdf destroy the converter object.");
    wkhtmltopdf_destroy_converter(converter);

    return 0;
}

// Возвращает имя конвертированного файла
std::string HtmlConverter::getImage(const std::string& pageFile, const FileExtension extension)
{
    g_logger.log(restbed::Logger::DEBUG, "HtmlConverter::getImage RUN.");

    m_pageFile = pageFile;
    m_image = ServerConstants::NONE;

    // Определяем требуемое расширение для картинки
    g_logger.log(restbed::Logger::DEBUG, "HtmlConverter::getImage get the file extension.");
    std::string fileExtension;
    switch (extension)
    {
    case HtmlConverter::PNG:
        fileExtension = Extension::EXT_PNG;
        break;
    case HtmlConverter::JPG:
        fileExtension = Extension::EXT_JPG;
        break;
    case HtmlConverter::JPEG:
        fileExtension = Extension::EXT_JPEG;
        break;
    case HtmlConverter::BMP:
        fileExtension = Extension::EXT_BMP;
        break;
    case HtmlConverter::PDF:
        g_logger.log(restbed::Logger::DEBUG, "HtmlConverter::getImage convert html to pdf.");
        // Конвертация в pdf
        if (htmlToPdf())
        {
            g_logger.log(restbed::Logger::WARNING, 
                "HtmlConverter::getImage got failed transformation html to pdf.");
            return ServerConstants::NONE;
        }
        else
        {
            return m_image;
        }
    default:
        g_logger.log(restbed::Logger::WARNING, "HtmlConverter::getImage: Invalid extension.");
        return ServerConstants::NONE;
    }

    // Конвертация в картинку
    g_logger.log(restbed::Logger::DEBUG, "HtmlConverter::getImage convert html to an image.");
    if (htmlToImage(fileExtension))
    {
        g_logger.log(restbed::Logger::WARNING, "HtmlConverter::getImage got failed transformation html to an image.");
        return ServerConstants::NONE;
    }
    else
    {
        g_logger.log(restbed::Logger::DEBUG, "HtmlConverter::getImage convert html to an image successfully.");
        return m_image;
    }
}
