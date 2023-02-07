#ifndef HTMLCONVERTER_H
#define HTMLCONVERTER_H

#include <iostream>
#include <string>
#include <exception>

#include <wkhtmltox/image.h>
#include <wkhtmltox/pdf.h>

#include "CustomLogger.h"


extern CustomLogger g_logger;


// Конвертирует html страницу в картинку
class HtmlConverter
{
public:
	enum FileExtension
	{
		PNG,
		JPG,
		JPEG,
		BMP,
		PDF,

	};

	// Класс исключений для HtmlConverter
	class _converterException : public std::exception
	{
	private:
		std::string m_error;

	public:
		_converterException(std::string error) : m_error(error)
		{

		}

		const char* what() const noexcept { return m_error.c_str(); }
	};


private:
	static bool m_init;

	std::string m_pageFile;
	std::string m_image;
	

	int htmlToImage(const std::string& extension);
	int htmlToPdf();

	friend void progress_changed(wkhtmltoimage_converter* c, int p);
	friend void progress_changed(wkhtmltopdf_converter* c, int p);
	friend void phase_changed(wkhtmltoimage_converter* c);
	friend void phase_changed(wkhtmltopdf_converter* c);
	friend void error(wkhtmltoimage_converter* c, const char* msg);
	friend void error(wkhtmltopdf_converter* c, const char* msg);
	friend void warning(wkhtmltoimage_converter* c, const char* msg);
	friend void warning(wkhtmltopdf_converter* c, const char* msg);


public:
	HtmlConverter()
	{
		// Разрешён только один объект этого класса
		if (m_init)
		{
			throw _converterException("Only one HtmlConverter object is allowed.");
		}
		else
		{
			g_logger.log(restbed::Logger::DEBUG, "HtmlConverter: Initialization.");
			m_init = true;

			// Инициализируем соединение с сервером wkhtmltox без графики
			wkhtmltoimage_init(false);
		}
	}

	~HtmlConverter()
	{
		// Закрытие соединения с wkhtmltox
		g_logger.log(restbed::Logger::DEBUG, "HtmlConverter: Deinitialization.");
		wkhtmltoimage_deinit();
	}

	std::string getImage(const std::string& pageFile, const FileExtension extension);

};

#endif // !HTMLCONVERTER_H
