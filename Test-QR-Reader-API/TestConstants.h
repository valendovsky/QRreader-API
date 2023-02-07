#pragma once

#include <string>
#include <vector>


namespace TestConst
{
	const std::string QR_CONTENT          { "https://github.com/valendovsky/QRreader-API" };
	const std::string NO_QR_CONTENT       { "none" };

	const std::string PATH_DLL            { "../Release" };

	const std::string TEST_QR_JPG         { "../test-images/testQrFileJpg.jpg" };
	const std::string TEST_QR_PNG         { "../test-images/testQrFilePng.png" };
	const std::string TEST_QR_PDF         { "../test-images/testQrFilePdf.pdf" };
	const std::string TEST_QR_A_FEW_PDF   { "../test-images/testQrAFewFilePdf.pdf" };
	const std::string TEST_NO_QR_JPG      { "../test-images/testNoQrFileJpg.jpg" };
	const std::string TEST_NO_QR_PNG      { "../test-images/testNoQrFilePng.png" };
	const std::string TEST_NO_QR_PDF      { "../test-images/testNoQrFilePdf.pdf" };
	const std::string TEST_NO_QR_A_FEW_PDF{ "../test-images/testNoQrAFewFilePdf.pdf" };

	const std::vector<std::string> TEST_CACHE_QR_PNG_GROUP
	{
		"../test-images/cache-files/testQrPngGroup_0.png",
		"../test-images/cache-files/testQrPngGroup_1.png",
		"../test-images/cache-files/testQrPngGroup_2.png"
	};
	const std::vector<std::string> TEST_QR_PNG_GROUP
	{ 
		"../test-images/testQrPngGroup_0.png", 
		"../test-images/testQrPngGroup_1.png", 
		"../test-images/testQrPngGroup_2.png" 
	};
	const std::vector<std::string> TEST_CACHE_NO_QR_PNG_GROUP
	{
		"../test-images/cache-files/testNoQrPngGroup_0.png",
		"../test-images/cache-files/testNoQrPngGroup_1.png",
		"../test-images/cache-files/testNoQrPngGroup_2.png"
	};
	const std::vector<std::string> TEST_NO_QR_PNG_GROUP
	{
		"../test-images/testNoQrPngGroup_0.png",
		"../test-images/testNoQrPngGroup_1.png",
		"../test-images/testNoQrPngGroup_2.png"
	};
}