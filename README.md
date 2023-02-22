# QRreader-API

<p align="center">
   <img src="https://img.shields.io/badge/C%2B%2B-14-blue" alt="C++ Version">
   <img src="https://img.shields.io/badge/Restbed-4.8-green" alt="Restbed Version">
   <img src="https://img.shields.io/badge/ImageMagick-7.1.0--62-yellowgreen" alt="ImageMagick Version">
   <img src="https://img.shields.io/badge/Ghostscript-10.0.0-informational" alt="Ghostscript Version">
   <img src="https://img.shields.io/badge/Wkhtmltox-0.12.6-blueviolet" alt="Wkhtmltox Version">
</p>

<p align="center">
   <img src="https://img.shields.io/badge/OpenCV-4.6.0-orange" alt="OpenCV Version">
   <img src="https://img.shields.io/badge/curl-7.84.0-lightgrey" alt="curl Version">
   <img src="https://img.shields.io/badge/version-1.0-yellow" alt="Application Version">
   <img src="https://img.shields.io/badge/license-MIT-red" alt="License">
</p>

## The application for reading QR codes. 

### About
This is a REST API application.
The application detects and decodes QR codes.
The application accepts jpeg, png and pdf images.
The application accepts links to images of the same formats, as well as to html pages containing QR codes.

### Documentation
The application uses the wkhtmltox server to convert html pages.
And for detecting and decoding QR codes - OpenCV.
The application accepts a POST request with the appropriate headers.
Requests with invalid headers are rejected.

### Developers
- [Valendovsky](https://github.com/valendovsky)

### License
Project QRreader-API is distributed under the MIT license.

---

## Приложение для считывания QR кодов.

### О проекте
Серверное API приложение, декодирующее предоставленные QR-коды.
Программа принимает изображения форматов: jpeg, png и pdf.
Принимает интернет-ссылки на картинки тех же форматов, а также на html страницы содержащие QR-коды.

### Документация
Программа в своей работе использует wkhtmltox сервер для конвертации html страниц.
А для распознавания и чтения QR-кодов - OpenCV.
Сервер принимает POST запрос с соответствующими контенту заголовками.
Запросы с заголовками неустановленного формата отклоняются независимо от содержания тела запроса.


### Разработчики
- [Valendovsky](https://github.com/valendovsky)

### Лицензия
Проект QRreader-API распространяется под лицензией MIT.
