// QRCodeGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "QRCode.h"
#include <iostream>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#include <bitset>
#include <cassert>
#include <Windows.h>

#define ALIGN8(x)		(~7 & ((x) + 7))


void SetConsoleAttributes()
{
	SetConsoleTitle(L"QR Code Generator in version 1 by http://sanje2v.wordpress.com/");

	// Set foreground white and background black
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
							FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void SetupConsoleForQRCode()
{
	auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// NOTE: THe following font info was collected by setting using OS
	CONSOLE_FONT_INFOEX fontinfoex;
	fontinfoex.cbSize = sizeof(fontinfoex);
	fontinfoex.dwFontSize = COORD{ 8, 8 };
	wcscpy_s(fontinfoex.FaceName, L"Terminal");
	fontinfoex.FontFamily = 48;
	fontinfoex.FontWeight = 400;
	fontinfoex.nFont = 2;		// WARNING: This index number is not documented,
								// hopefully it is the same on all systems

	SetCurrentConsoleFontEx(hConsole, FALSE, &fontinfoex);

	auto hwndConsole = GetConsoleWindow();
	SetWindowPos(hwndConsole, NULL, 0, 0, 640, 480, SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOREPOSITION);
}


int main(int argc, wchar_t *argv[])
{
	// Set console window attributes
	SetConsoleAttributes();

	// Get input
	std::cout << "What do I encode? ";
	std::string Data;
	std::getline(std::cin, Data);	// NOTE: We need to use 'std::getline()' 'cause input may have spaces

	std::cout << "What level of error correction should I use?" << std::endl
		<< "'L' number of error correction capacity is 2," << std::endl
		<< "'M' number of error correction capacity is 4," << std::endl
		<< "'Q' number of error correction capacity is 6," << std::endl
		<< "'H' number of error correction capacity is 8" << std::endl
		<< ": ";
	char ErrorCorrectionLevel;
	std::cin >> ErrorCorrectionLevel;
	ErrorCorrectionLevel = toupper(ErrorCorrectionLevel);	// CAUTION: Make sure input is uppercased

	if (mapErrorCorrectionLevels.find(ErrorCorrectionLevel) == mapErrorCorrectionLevels.end())
	{
		std::cout << "Unknown error correction option specified, defaulted to '"
			<< mapErrorCorrectionLevels.cbegin()->first << "' error correction." << std::endl;
		ErrorCorrectionLevel = mapErrorCorrectionLevels.cbegin()->first;
	}

	// Check if given the input size and error correction capacity, the total data will fit in version 1
	if (Data.size() > mapErrorCorrectionLevels[ErrorCorrectionLevel].first)
	{
		std::cout << "The maximum data size that can be encoded is "
			<< mapErrorCorrectionLevels[ErrorCorrectionLevel].first << " under selected option." << std::endl;

		return 0;
	}

	std::cout << std::endl << std::endl;

	static const int QUIETZONE_SIZE = 4;	// Specification says should have atleast 4 modules of surrounding quiet zone
	static const int QRCODE_SIZE = MODULE_SIZE + QUIETZONE_SIZE * 2;
	unsigned char QRCode[QRCODE_SIZE * QRCODE_SIZE] = { 0 };	// IMPORTANT: Fill with 0 to represent unused regions

	// Add Quiet Zone around QR code
	AddQuietZone(QRCode, QRCODE_SIZE, QUIETZONE_SIZE);

	// Write pattern finder and their separators
	static const int FINDERPATTERN_SIZE = 7;
	AddFinderPattern(QRCode, QRCODE_SIZE, QUIETZONE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);										// Top-Left
	AddFinderPattern(QRCode, QRCODE_SIZE, QUIETZONE_SIZE, QRCODE_SIZE - QUIETZONE_SIZE - FINDERPATTERN_SIZE, FINDERPATTERN_SIZE);	// Top-Right
	AddFinderPattern(QRCode, QRCODE_SIZE, QRCODE_SIZE - QUIETZONE_SIZE - FINDERPATTERN_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);	// Bottom-Left
	AddFinderPatternSeparatorTopLeft(QRCode, QRCODE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);
	AddFinderPatternSeparatorTopRight(QRCode, QRCODE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);
	AddFinderPatternSeparatorBottomLeft(QRCode, QRCODE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);

	// Write timing pattern
	AddTimingPattern(QRCode, QRCODE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);

	// Write compulsory dark module
	AddCompulsoryDarkModule(QRCode, QRCODE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);

	// Generate data bits prefixed with mode and size
	static const int BITS_PER_BLOCK = 11;
	static const int BITS_PER_INCOMPLETE_BLOCK = (BITS_PER_BLOCK + 1) / 2;
	static const int ALPHANUMERIC_MODE = 0b0010;
	static const int MODE_SIZE = 4;
	static const int DATASIZE_SIZE = 9;
	const int NumCompleteBlocks = static_cast<int>(Data.size() / 2);
	const int NumIncompleteBlocks = static_cast<int>(Data.size() - (NumCompleteBlocks * 2));
	const int DataBitsSize = MODE_SIZE + DATASIZE_SIZE + NumCompleteBlocks * BITS_PER_BLOCK + NumIncompleteBlocks * BITS_PER_INCOMPLETE_BLOCK;
	const int DataBitsSizeAligned = ALIGN8(DataBitsSize);
	const int TotalDataBitsSize = mapErrorCorrectionLevels[ErrorCorrectionLevel].second;

	std::vector<bool> bitsData;				// 'std::vector<bool>' specializes to use bits
	bitsData.reserve(TotalDataBitsSize);
	std::bitset<MODE_SIZE> bitsDataMode(ALPHANUMERIC_MODE);
	std::bitset<DATASIZE_SIZE> bitsDataSize(Data.size());

	// CAUTION: We are using 'size_t' (typedef unsigned int/long long) for '.size()' below, using '--i' so tread lightly

	// NOTE: When reading 'std::bitset<>' bits, we need to read from most significant bit towards
	//		 least significant bit, so the 'for' loops below go backwards

	// Copy data mode
	for (auto i = bitsDataMode.size(); i > 0 ; --i)
		bitsData.push_back(bitsDataMode[i - 1]);

	// Copy data size
	for (auto i = bitsDataSize.size(); i > 0; --i)
		bitsData.push_back(bitsDataSize[i - 1]);

	// Encode complete blocks
	for (auto i = 0; i < NumCompleteBlocks; ++i)
	{
		std::bitset<BITS_PER_BLOCK> bitsCompleteBlock(EncodeAlphanumericValue(Data[i * 2]) * ALPHANUMERIC_ENCODING_TABLE_SIZE
														+ EncodeAlphanumericValue(Data[i * 2 + 1]));

		for (auto j = bitsCompleteBlock.size(); j > 0; --j)
			bitsData.push_back(bitsCompleteBlock[j - 1]);
	}

	// Encode incomplete block, if any
	if (NumIncompleteBlocks > 0)
	{
		std::bitset<BITS_PER_INCOMPLETE_BLOCK> bitsIncompleteBlock(EncodeAlphanumericValue(Data[NumCompleteBlocks * 2]));
		
		for (auto i = bitsIncompleteBlock.size(); i > 0; --i)
			bitsData.push_back(bitsIncompleteBlock[i - 1]);
	}

	// Add terminator bits (zeros) to make sure 'bitsData' is 8-bit aligned
	// NOTE: Max terminator bits is 4 and add some more to align to 8-bit, if required
	assert(DataBitsSizeAligned >= DataBitsSize);
	for (int i = 0; i < (DataBitsSizeAligned - DataBitsSize); ++i)
		bitsData.push_back(static_cast<bool>(0));

	// Alternatively padding bits sequences, if required, to cover all available data codewords space
	static const std::bitset<PADDING_SIZE> PADDINGBITS[] = { 0b11101100, 0b00010001 };

	// Add padding bit sequences until all available data codeword space is filled
	const int NumOfPaddingSeq = (TotalDataBitsSize - DataBitsSizeAligned) / PADDING_SIZE;
	for (int i = 0; i < NumOfPaddingSeq; ++i)
	{
		int idxPaddingSeq = i & 0x1;	// Alternate between padding sequences

		for (int j = PADDING_SIZE; j > 0; --j)
			bitsData.push_back(PADDINGBITS[idxPaddingSeq][j - 1]);
	}

	// Allocate space for coefficients interms of exponent of 'alpha = 2' of data codewords and generator polynomials
	static const int DATA_CODEWORDS_SIZE = (TotalDataBitsSize / CODEWORD_SIZE);
	static const int ERROR_CODEWORDS_SIZE = TOTAL_CODEWORDS - DATA_CODEWORDS_SIZE;
	const int NumOfCoefficients = TOTAL_CODEWORDS;
	std::vector<int> coefficientsDataPolynomial;
	coefficientsDataPolynomial.reserve(TOTAL_CODEWORDS);

	// Fill message codeword coefficients
	// NOTE: We work with exponent arithematic of 'alpha = 2', so when 'coefficient' is
	//		 mentioned in the following code, depending on which step is being referred to,
	//		 it may be exponent of 'alpha' (needed for convenient mathematics) or an antilog value.
	for (int i = 0; i < NumOfCoefficients; ++i)
	{
		if (i < DATA_CODEWORDS_SIZE)
		{
			std::bitset<CODEWORD_SIZE> dataCodeword;

			for (int j = 0; j < CODEWORD_SIZE; ++j)
			{
				dataCodeword.set((CODEWORD_SIZE - j - 1), bitsData[j + i * CODEWORD_SIZE]);
			}

			coefficientsDataPolynomial.push_back(logTable[dataCodeword.to_ulong()]);
		}
		else
			coefficientsDataPolynomial.push_back(logTable[0]);
	}

	// Divide data polynomial with error codewords generator polynomial
	const int GaloisField_Max = (int)std::pow(2, 8) - 1;
	for (int i = 0; i < DATA_CODEWORDS_SIZE; ++i)
	{
		// Fill generator coefficients at each step of division
		auto coefficientsGeneratorPolynomial = mapErrorCorrectionPolynomialCoefficients[ErrorCorrectionLevel];
		coefficientsGeneratorPolynomial.reserve(TOTAL_CODEWORDS);
		for (int j = 0; j < (TOTAL_CODEWORDS - ERROR_CODEWORDS_SIZE - 1); ++j)
			coefficientsGeneratorPolynomial.push_back(logTable[0]);

		for (int j = 0; j < (ERROR_CODEWORDS_SIZE + 1); ++j)
		{
			if (coefficientsDataPolynomial.front() == logTable[0])
				coefficientsGeneratorPolynomial[j] = logTable[0];
			else if (coefficientsGeneratorPolynomial[j] == logTable[0])
				continue;
			else
			{
				coefficientsGeneratorPolynomial[j] = (coefficientsGeneratorPolynomial[j] + coefficientsDataPolynomial.front()) % GaloisField_Max;
			}
		}

		for (int j = 0; j < coefficientsDataPolynomial.size(); ++j)
		{
			auto antilogDataPolynomialCoefficient = (coefficientsDataPolynomial[j] == logTable[0] ? 0 : antilogTable[coefficientsDataPolynomial[j]]);
			auto antilogGeneratorPolynomialCoefficient = (coefficientsGeneratorPolynomial[j] == logTable[0] ? 0 : antilogTable[coefficientsGeneratorPolynomial[j]]);

			coefficientsDataPolynomial[j] = antilogDataPolynomialCoefficient ^ antilogGeneratorPolynomialCoefficient;

			if (i != (DATA_CODEWORDS_SIZE - 1))	// NOTE: If this is the last one, don't perform a log here
				coefficientsDataPolynomial[j] = logTable[coefficientsDataPolynomial[j]];
		}

		coefficientsDataPolynomial.erase(coefficientsDataPolynomial.begin());	// 'pop_front()' on 'std::vector<bool>'
	}

	// Convert error correction codewords to 'vector<bool>' (i.e. binary string)
	std::vector<bool> bitsErrorCodes;
	bitsErrorCodes.reserve(ERROR_CODEWORDS_SIZE * CODEWORD_SIZE);

	for (int i = 0; i < ERROR_CODEWORDS_SIZE; ++i)
	{
		std::bitset<CODEWORD_SIZE> errorCodeword(coefficientsDataPolynomial[i]);

		for (auto j = CODEWORD_SIZE; j > 0; --j)
			bitsErrorCodes.push_back(errorCodeword[j - 1]);
	}

	// NOTE: Version 1 being implemented doesn't require dividing data into blocks
	//		 so we continue on to putting bits into QR matrix - first data then error codewords


	// Put data codewords followed by error correction codewords into QR matrix
	AddCodewords(bitsData, bitsErrorCodes, QRCode, QRCODE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);

	// Calculate best masking pattern
	auto indexMask = DetermineMaskPatternWithLowestPenaltyScore(QRCode, QRCODE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);

	// Perform masking to remove unwanted pattern artifacts
	ApplyMask(QRCode, QRCODE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE, indexMask);

	// Write format information
	AddFormatInformation(ErrorCorrectionLevel, indexMask, QRCode, QRCODE_SIZE, QUIETZONE_SIZE, FINDERPATTERN_SIZE);

	std::cout << std::endl;

	// Change console output to show a square QR Code properly
	SetupConsoleForQRCode();

	// Print
	PrintQRCode(QRCode, QRCODE_SIZE);

	std::cout << std::endl << std::endl;

    return 0;
}

