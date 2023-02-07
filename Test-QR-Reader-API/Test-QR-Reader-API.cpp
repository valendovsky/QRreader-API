#include "pch.h"
#include "CppUnitTest.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdio>

#include <restbed>

#include "TestConstants.h"
#include "../QR-Reader-API/ReaderQR.h"
#include "../QR-Reader-API/ServerConstants.h"
#include "../QR-Reader-API/PdfConverter.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


std::string decodeImage(const restbed::Bytes& body);
std::string decodeListImages(const std::vector<std::string>& images);
std::string decodePdfBuf(const restbed::Bytes& body);
std::string decodePdfFile(const std::string& inFileName);


namespace TestQRReaderAPI
{
	TEST_CLASS(TestClassReaderQR)
	{
	public:
		
		TEST_METHOD(GetQrContentFileNameJpg)
		{
			// Arrange
			ReaderQR reader(TestConst::TEST_QR_JPG);

			// Act
			std::string result = reader.getQrContent();

			// Assert
			Assert::IsTrue(result == TestConst::QR_CONTENT);
		}

		TEST_METHOD(GetQrContentFileNamePng)
		{
			ReaderQR reader(TestConst::TEST_QR_PNG);

			std::string result = reader.getQrContent();

			Assert::IsTrue(result == TestConst::QR_CONTENT);
		}

		TEST_METHOD(GetQrContentFileNameNoJpg)
		{
			ReaderQR reader(TestConst::TEST_NO_QR_JPG);

			std::string result = reader.getQrContent();

			Assert::IsTrue(result == TestConst::NO_QR_CONTENT);
		}

		TEST_METHOD(GetQrContentFileNameNoPng)
		{
			ReaderQR reader(TestConst::TEST_NO_QR_PNG);

			std::string result = reader.getQrContent();

			Assert::IsTrue(result == TestConst::NO_QR_CONTENT);
		}

		TEST_METHOD(GetQrContentBufJpg)
		{
			std::ifstream file(TestConst::TEST_QR_JPG, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			std::vector<std::uint8_t> fileBuf{ fileStr.begin(), fileStr.end() };
			ReaderQR reader(fileBuf);

			std::string result = reader.getQrContent();

			Assert::IsTrue(result == TestConst::QR_CONTENT, L"Assert 2");
		}

		TEST_METHOD(GetQrContentBufPng)
		{
			std::ifstream file(TestConst::TEST_QR_PNG, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			std::vector<std::uint8_t> fileBuf{ fileStr.begin(), fileStr.end() };
			ReaderQR reader(fileBuf);

			std::string result = reader.getQrContent();

			Assert::IsTrue(result == TestConst::QR_CONTENT, L"Assert 2");
		}

		TEST_METHOD(GetQrContentBufNoJpg)
		{
			std::ifstream file(TestConst::TEST_NO_QR_JPG, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			std::vector<std::uint8_t> fileBuf{ fileStr.begin(), fileStr.end() };
			ReaderQR reader(fileBuf);

			std::string result = reader.getQrContent();

			Assert::IsTrue(result == TestConst::NO_QR_CONTENT, L"Assert 2");
		}

		TEST_METHOD(GetQrContentBufNoPng)
		{
			std::ifstream file(TestConst::TEST_NO_QR_PNG, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			std::vector<std::uint8_t> fileBuf{ fileStr.begin(), fileStr.end() };
			ReaderQR reader(fileBuf);

			std::string result = reader.getQrContent();

			Assert::IsTrue(result == TestConst::NO_QR_CONTENT, L"Assert 2");
		}

	};

	TEST_CLASS(TestClassPdfConverter)
	{
	public:

		TEST_METHOD(TestConvertOneFileSize)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());

			std::vector<std::string> fileNames = pdfToPng.convert(TestConst::TEST_QR_PDF, "PNG");
			for (const auto element : fileNames)
			{
				remove(element.c_str());
			}

			Assert::IsTrue(fileNames.size() == 1);
		}

		TEST_METHOD(TestConvertAFewFileSize)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());

			std::vector<std::string> fileNames = pdfToPng.convert(TestConst::TEST_QR_A_FEW_PDF, "PNG");
			for (const auto element : fileNames)
			{
				remove(element.c_str());
			}

			Assert::IsTrue(fileNames.size() == 3);
		}

		TEST_METHOD(TestConvertOneBufSize)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());
			std::ifstream file(TestConst::TEST_QR_PDF, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			file.close();

			std::vector<std::string> fileNames = pdfToPng.convert(static_cast<const void*>(fileStr.data()), fileStr.length(), "PNG");
			for (const auto element : fileNames)
			{
				remove(element.c_str());
			}

			Assert::IsTrue(fileNames.size() == 1, L"Assert 2");
		}

		TEST_METHOD(TestConvertAFewBufSize)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());
			std::ifstream file(TestConst::TEST_QR_A_FEW_PDF, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			file.close();

			std::vector<std::string> fileNames = pdfToPng.convert(static_cast<const void*>(fileStr.data()), fileStr.length(), "PNG");
			for (const auto element : fileNames)
			{
				remove(element.c_str());
			}

			Assert::IsTrue(fileNames.size() == 3, L"Assert 2");
		}

		TEST_METHOD(TestConvertOneFile)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());

			std::vector<std::string> fileNames = pdfToPng.convert(TestConst::TEST_QR_PDF, "PNG");
			ReaderQR reader(fileNames[0]);
			std::string result = reader.getQrContent();
			for (const auto element : fileNames)
			{
				remove(element.c_str());
			}

			Assert::IsTrue(result == TestConst::QR_CONTENT && fileNames.size() == 1);
		}

		TEST_METHOD(TestConvertOneNoFile)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());

			std::vector<std::string> fileNames = pdfToPng.convert(TestConst::TEST_NO_QR_PDF, "PNG");
			ReaderQR reader(fileNames[0]);
			std::string result = reader.getQrContent();
			for (const auto element : fileNames)
			{
				remove(element.c_str());
			}

			Assert::IsTrue(result == TestConst::NO_QR_CONTENT && fileNames.size() == 1);
		}

		TEST_METHOD(TestConvertAFewFile)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());

			std::vector<std::string> fileNames = pdfToPng.convert(TestConst::TEST_QR_A_FEW_PDF, "PNG");
			Assert::IsTrue(fileNames.size() == 3, L"Assert 1");
			for (int index = 0; index < fileNames.size(); ++index)
			{
				ReaderQR reader(fileNames[index]);
				std::string result = reader.getQrContent();
				remove(fileNames[index].c_str());


				Assert::IsTrue((result == TestConst::QR_CONTENT && index == 1) || 
					(result == TestConst::NO_QR_CONTENT && index != 1), L"Assert 2");
			}
		}

		TEST_METHOD(TestConvertAFewNoFile)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());

			std::vector<std::string> fileNames = pdfToPng.convert(TestConst::TEST_NO_QR_A_FEW_PDF, "PNG");
			Assert::IsTrue(fileNames.size() == 3, L"Assert 1");
			for (int index = 0; index < fileNames.size(); ++index)
			{
				ReaderQR reader(fileNames[index]);
				std::string result = reader.getQrContent();
				remove(fileNames[index].c_str());


				Assert::IsTrue(result == TestConst::NO_QR_CONTENT, L"Assert 2");
			}
		}

		TEST_METHOD(TestConvertOneBuf)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());
			std::ifstream file(TestConst::TEST_QR_PDF, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			file.close();

			std::vector<std::string> fileNames = pdfToPng.convert(static_cast<const void*>(fileStr.data()), fileStr.length(), "PNG");
			ReaderQR reader(fileNames[0]);
			std::string result = reader.getQrContent();
			for (const auto element : fileNames)
			{
				remove(element.c_str());
			}

			Assert::IsTrue(result == TestConst::QR_CONTENT && fileNames.size() == 1, L"Assert 2");
		}

		TEST_METHOD(TestConvertAFewBuf)
		{
			PdfConverter pdfToPng(TestConst::PATH_DLL.c_str());
			std::ifstream file(TestConst::TEST_QR_A_FEW_PDF, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			file.close();

			std::vector<std::string> fileNames = pdfToPng.convert(static_cast<const void*>(fileStr.data()), fileStr.length(), "PNG");
			Assert::IsTrue(fileNames.size() == 3, L"Assert 2");
			for (int index = 0; index < fileNames.size(); ++index)
			{
				ReaderQR reader(fileNames[index]);
				std::string result = reader.getQrContent();
				remove(fileNames[index].c_str());


				Assert::IsTrue((result == TestConst::QR_CONTENT && index == 1) ||
					(result == TestConst::NO_QR_CONTENT && index != 1), L"Assert 3");
			}
		}

	};

	TEST_CLASS(TestQRReaderAPI)
	{
	public:

		TEST_METHOD(TestDecodeImage)
		{
			std::ifstream file(TestConst::TEST_QR_PNG, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			file.close();
			
			std::vector<std::uint8_t> fileBuf{ fileStr.begin(), fileStr.end() };
			std::string result = decodeImage(fileBuf);

			Assert::IsTrue(result == TestConst::QR_CONTENT, L"Assert 2");
		}

		TEST_METHOD(TestNoDecodeImage)
		{
			std::ifstream file(TestConst::TEST_NO_QR_PNG, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			file.close();

			std::vector<std::uint8_t> fileBuf{ fileStr.begin(), fileStr.end() };
			std::string result = decodeImage(fileBuf);

			Assert::IsTrue(result == TestConst::NO_QR_CONTENT, L"Assert 2");
		}

		TEST_METHOD(TestDecodeListImages)
		{
			for (int index = 0; index < TestConst::TEST_CACHE_QR_PNG_GROUP.size(); ++index)
			{
				std::ifstream cacheFile(TestConst::TEST_CACHE_QR_PNG_GROUP[index], std::ios::binary);
				Assert::IsTrue(cacheFile.is_open(), L"Assert 1");
				std::ofstream file(TestConst::TEST_QR_PNG_GROUP[index], std::ios::binary);
				Assert::IsTrue(file.is_open(), L"Assert 2");
				file << cacheFile.rdbuf();
				cacheFile.close();
				file.close();
			}

			std::string result = decodeListImages(TestConst::TEST_QR_PNG_GROUP);

			Assert::IsTrue(result == TestConst::QR_CONTENT, L"Assert 3");
		}

		TEST_METHOD(TestNoDecodeListImages)
		{
			for (int index = 0; index < TestConst::TEST_CACHE_NO_QR_PNG_GROUP.size(); ++index)
			{
				std::ifstream cacheFile(TestConst::TEST_CACHE_NO_QR_PNG_GROUP[index], std::ios::binary);
				Assert::IsTrue(cacheFile.is_open(), L"Assert 1");
				std::ofstream file(TestConst::TEST_NO_QR_PNG_GROUP[index], std::ios::binary);
				Assert::IsTrue(file.is_open(), L"Assert 2");
				file << cacheFile.rdbuf();
				cacheFile.close();
				file.close();
			}

			std::string result = decodeListImages(TestConst::TEST_NO_QR_PNG_GROUP);

			Assert::IsTrue(result == TestConst::NO_QR_CONTENT, L"Assert 3");
		}

		TEST_METHOD(TestDecodePdfBuf)
		{
			std::ifstream file(TestConst::TEST_QR_PDF, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			file.close();

			std::vector<std::uint8_t> fileBuf{ fileStr.begin(), fileStr.end() };
			std::string result = decodePdfBuf(fileBuf);

			Assert::IsTrue(result == TestConst::QR_CONTENT, L"Assert 2");
		}

		TEST_METHOD(TestNoDecodePdfBuf)
		{
			std::ifstream file(TestConst::TEST_NO_QR_PDF, std::ios::binary);
			Assert::IsTrue(file.is_open(), L"Assert 1");
			std::ostringstream oss;
			oss << file.rdbuf();
			std::string fileStr{ oss.str() };
			file.close();

			std::vector<std::uint8_t> fileBuf{ fileStr.begin(), fileStr.end() };
			std::string result = decodePdfBuf(fileBuf);

			Assert::IsTrue(result == TestConst::NO_QR_CONTENT, L"Assert 2");
		}

		TEST_METHOD(TestDecodePdfFile)
		{
			std::string result = decodePdfFile(TestConst::TEST_QR_PDF);

			Assert::IsTrue(result == TestConst::QR_CONTENT);
		}

		TEST_METHOD(TestNoDecodePdfFile)
		{
			std::string result = decodePdfFile(TestConst::TEST_NO_QR_PDF);

			Assert::IsTrue(result == TestConst::NO_QR_CONTENT);
		}
	};
}
