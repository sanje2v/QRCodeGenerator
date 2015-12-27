#pragma once

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <cassert>
#include <bitset>
#include <unordered_map>
#include <functional>
#include <array>
#include <cmath>

#define NEXTMULTIPLEOF5(x)			(~4 & ((x) + 4))
#define PREVIOUSMULTIPLEOF5(x)		((NEXTMULTIPLEOF5(x)) - 5)
#define MODULE_TO_BOOL(x)			((x) == DARK_MODULE ? true : false)
#define BOOL_TO_MODULE(x)			((x) ? DARK_MODULE : LIGHT_MODULE)
#define BOOL_TO_BIT(x)				((x) ? 1 : 0)

// CAUTION: LIGHT_MODULE represent binary '0' and vice-versa
static const unsigned char LIGHT_MODULE = 219;	// Filled white block character
static const unsigned char DARK_MODULE = 32;	// Space character

static const int QRCODE_VERSION = 1;
static const int MODULE_SIZE = 21;

static const int PADDING_SIZE = 8;
static const int CODEWORD_SIZE = 8;
static const int TOTAL_CODEWORDS = 26;
static const int NUM_MASK_GENERATORS = 8;
static const int FORMAT_SIZE = 15;


// Source: http://www.thonky.com/qr-code-tutorial/log-antilog-table
static const int logTable[] =
{
	 -1,   0,   1,  25,   2,  50,  26, 198,
	  3, 223,  51, 238,  27, 104, 199,  75,
	  4, 100, 224,  14,  52, 141, 239, 129,
	 28, 193, 105, 248,	200,   8,  76, 113,
	  5, 138, 101,	47, 225,  36,  15,  33,
	 53, 147, 142, 218, 240,  18, 130,  69,
	29, 181, 194, 125,	106,  39, 249, 185,
	201, 154,   9, 120,  77, 228, 114, 166,
	  6, 191, 139,	98,	102, 221,  48, 253,
	226, 152,  37, 179,  16, 145,  34, 136,
	 54, 208, 148, 206,	143, 150, 219, 189,
	241, 210,  19,	92,	131,  56,  70,	64,
	 30,  66, 182, 163,	195,  72, 126, 110,
	107,  58,  40,	84,	250, 133, 186,	61,
	202,  94, 155, 159,  10,  21, 121,	43,
	 78, 212, 229, 172,	115, 243, 167,	87,
	  7, 112, 192, 247,	140, 128,  99,	13,
	103,  74, 222, 237,  49, 197, 254,  24,
	227, 165, 153, 119,  38, 184, 180, 124,
	 17,  68, 146, 217,  35,  32, 137,	46,
	 55,  63, 209,	91,	149, 188, 207, 205,
	144, 135, 151, 178,	220, 252, 190,	97,
	242,  86, 211, 171,  20,  42,  93, 158,
	132,  60,  57,	83,  71, 109,  65, 162,
	 31,  45,  67, 216,	183, 123, 164, 118,
	196,  23,  73, 236,	127,  12, 111, 246,
	108, 161,  59,  82,  41, 157,  85, 170,
	251,  96, 134, 177, 187, 204,  62,	90,
	203,  89,  95, 176,	156, 169, 160,	81,
	 11, 245,  22, 235,	122, 117,  44, 215,
	 79, 174, 213, 233,	230, 231, 173, 232,
	116, 214, 244, 234,	168,  80,  88, 175
};

static const int antilogTable[] =
{
	  1,   2,   4,   8,  16,  32,  64, 128,
	 29,  58, 116, 232, 205, 135,  19,  38,
	 76, 152,  45,  90,	180, 117, 234, 201,
	143,   3,	6,	12,  24,  48,  96, 192,
	157,  39,  78, 156,  37,  74, 148,	53,
	106, 212, 181, 119,	238, 193, 159,	35,
	 70, 140,	5,	10,  20,  40,  80, 160,
	 93, 186, 105, 210,	185, 111, 222, 161,
	 95, 190,  97, 194,	153,  47,  94, 188,
	101, 202, 137,	15,  30,  60, 120, 240,
	253, 231, 211, 187,	107, 214, 177, 127,
	254, 225, 223, 163,  91, 182, 113, 226,
	217, 175,  67, 134,  17,  34,  68, 136,
	 13,  26,  52, 104,	208, 189, 103, 206,
	129,  31,  62, 124, 248, 237, 199, 147,
	 59, 118, 236, 197, 151,  51, 102, 204,
	133,  23,  46,  92, 184, 109, 218, 169,
	 79, 158,  33,	66,	132,  21,  42,	84,
	168,  77, 154,  41,  82, 164,  85, 170,
	 73, 146,  57, 114,	228, 213, 183, 115,
	230, 209, 191,  99, 198, 145,  63, 126,
	252, 229, 215, 179,	123, 246, 241, 255,
	227, 219, 171,	75,	150,  49,  98, 196,
	149,  55, 110, 220, 165,  87, 174,	65,
	130,  25,  50, 100, 200, 141,	7,	14,
	 28,  56, 112, 224,	221, 167,  83, 166,
	 81, 162,  89, 178,	121, 242, 249, 239,
	195, 155,  43,	86,	172,  69, 138,   9,
	 18,  36,  72, 144,  61, 122, 244, 245,
	247, 243, 251, 235,	203, 139,  11,	22,
	 44,  88, 176, 125,	250, 233, 207, 131,
	 27,  54, 108, 216,	173,  71, 142,   1
};

static std::unordered_map<char, const std::pair<int, int>> mapErrorCorrectionLevels =
{
	// Error code type, { Total data size that can be encoded, Total data bits that can be encoded including mode and size }
	{ 'L', { 25, 152 } },
	{ 'M', { 20, 128 } },
	{ 'Q', { 16, 104 } },
	{ 'H', { 10,  72 } }
};

static std::unordered_map<char, const std::vector<int>> mapErrorCorrectionPolynomialCoefficients =
{
	{ 'L', { 0,  87, 229, 146, 149, 238, 102,  21 } },
	{ 'M', { 0, 251,  67,  46,  61, 118,  70,  64,  94,  32,  45 } },
	{ 'Q', { 0,  74, 152, 176, 100,  86, 100, 106, 104, 130, 218, 206, 140,  78 } },
	{ 'H', { 0,  43, 139, 206,  78,  43, 239, 123, 206, 214, 147,  24,  99, 150,  39, 243, 163, 136 } }
};

// NOTE: These could be generated but a table lookup is far easy to generate as well as reduces load for CPU
static std::unordered_map<char, const std::array<std::bitset<FORMAT_SIZE>, NUM_MASK_GENERATORS>> mapErrorCorrectionFormatString =
{
	{ 'L', { 0b111011111000100, 0b111001011110011, 0b111110110101010, 0b111100010011101, 0b110011000101111, 0b110001100011000, 0b110110001000001, 0b110100101110110 } },
	{ 'M', { 0b101010000010010, 0b101000100100101, 0b101111001111100, 0b101101101001011, 0b100010111111001, 0b100000011001110, 0b100111110010111, 0b100101010100000 } },
	{ 'Q', { 0b011010101011111, 0b011000001101000, 0b011111100110001, 0b011101000000110, 0b010010010110100, 0b010000110000011, 0b010111011011010, 0b010101111101101 } },
	{ 'H', { 0b001011010001001, 0b001001110111110, 0b001110011100111, 0b001100111010000, 0b000011101100010, 0b000001001010101, 0b000110100001100, 0b000100000111011 } }
};

static const std::function<bool(int, int)> maskGeneratorTable[NUM_MASK_GENERATORS] =
{
	[](int i, int j) -> bool { return ((i + j) % 2 == 0); },
	[](int i, int j) -> bool { return (i % 2 == 0); },
	[](int i, int j) -> bool { return (j % 3 == 0); },
	[](int i, int j) -> bool { return (((i + j) % 3) == 0); },
	[](int i, int j) -> bool { return (((i / 2 + j / 3) % 2) == 0); },
	[](int i, int j) -> bool { return (((i * j) % 2 + (i * j) % 3) == 0); },
	[](int i, int j) -> bool { return ((((i * j) % 2 + (i * j) % 3) % 2) == 0); },
	[](int i, int j) -> bool { return ((((i + j) % 2 + (i * j) % 3) % 2) == 0); }
};


static const int ALPHANUMERIC_ENCODING_TABLE_SIZE = 45;
static int EncodeAlphanumericValue(char value)
{
	// NOTE: All lowercase characters are encoded as uppercase and unsupported
	//		 symbols are encoded as '$'
	if (value >= '0' && value <= '9')
		return ((int)value - '0') + 0;
	else if (value >= 'A' && value <= 'Z')
		return ((int)value - 'A') + 10;
	else if (value >= 'a' && value <= 'z')
		return ((int)value - 'a') + 10;
	else
	{
		switch (value)
		{
		case ' ': return 36;
		case '$': return 37;
		case '%': return 38;
		case '*': return 39;
		case '+': return 40;
		case '-': return 41;
		case '.': return 42;
		case '/': return 43;
		case ':': return 44;
		default:
			// ERROR: Unsupported character, encode as '$'
			return 37;
		}
	}
}

static void PrintQRCode(const std::vector<bool>& qrcode, int qrcodesize);

void AddQuietZone(unsigned char *qrcode, int qrcodesize, int quietzonesize)
{
	for (int i = 0; i < qrcodesize; ++i)
		for (int j = 0; j < qrcodesize; ++j)
		{
			if (i < quietzonesize || i >= (qrcodesize - quietzonesize) ||
				j < quietzonesize || j >= (qrcodesize - quietzonesize))
				qrcode[i * qrcodesize + j] = LIGHT_MODULE;
		}
}

void AddFinderPattern(unsigned char *qrcode, int qrcodesize, int starti, int startj, int findersize)
{
	for (int i = starti; i < (findersize + starti); ++i)
		for (int j = startj; j < (findersize + startj); ++j)
		{
			if (i == starti || i == (starti + findersize - 1))
				qrcode[i * qrcodesize + j] = DARK_MODULE;
			else if (j == startj || j == (startj + findersize - 1))
				qrcode[i * qrcodesize + j] = DARK_MODULE;
			else
			{
				if (i == (starti + 1) || i == (starti + findersize - 2))
					qrcode[i * qrcodesize + j] = LIGHT_MODULE;
				else if (j == (startj + 1) || j == (startj + findersize - 2))
					qrcode[i * qrcodesize + j] = LIGHT_MODULE;
				else
					qrcode[i * qrcodesize + j] = DARK_MODULE;
			}
		}
}

void AddFinderPatternSeparatorTopLeft(unsigned char *qrcode, int qrcodesize, int quietzonesize, int findersize)
{
	for (int j = quietzonesize; j < (quietzonesize + findersize); ++j)
		qrcode[(findersize + quietzonesize) * qrcodesize + j] = LIGHT_MODULE;

	for (int i = quietzonesize; i <= (quietzonesize + findersize); ++i)
		qrcode[i * qrcodesize + (findersize + quietzonesize)] = LIGHT_MODULE;
}

void AddFinderPatternSeparatorTopRight(unsigned char *qrcode, int qrcodesize, int quietzonesize, int findersize)
{
	for (int i = quietzonesize; i < (quietzonesize + findersize); ++i)
		qrcode[i * qrcodesize + (qrcodesize - (quietzonesize + findersize + 1))] = LIGHT_MODULE;

	for (int j = qrcodesize - (quietzonesize + findersize + 1); j < (qrcodesize - quietzonesize); ++j)
		qrcode[(findersize + quietzonesize) * qrcodesize + j] = LIGHT_MODULE;
}

void AddFinderPatternSeparatorBottomLeft(unsigned char *qrcode, int qrcodesize, int quietzonesize, int findersize)
{
	for (int j = quietzonesize; j < (quietzonesize + findersize); ++j)
		qrcode[(qrcodesize - (quietzonesize + findersize + 1)) * qrcodesize + j] = LIGHT_MODULE;

	for (int i = qrcodesize - (quietzonesize + findersize + 1); i < (qrcodesize - quietzonesize); ++i)
		qrcode[i * qrcodesize + (findersize + quietzonesize)] = LIGHT_MODULE;
}

void AddTimingPattern(unsigned char *qrcode, int qrcodesize, int quietzonesize, int finderpatternsize)
{
	//NOTE: 'BOOL_TO_MODULE(!(x & 0x1))' generates alternating light and dark modules starting with dark module
	for (int i = quietzonesize + finderpatternsize + 1; i < qrcodesize - (quietzonesize + finderpatternsize + 1); ++i)
		qrcode[i * qrcodesize + (quietzonesize + finderpatternsize - 1)] = BOOL_TO_MODULE(!((i - (quietzonesize + finderpatternsize + 1)) & 0x1));

	for (int j = quietzonesize + finderpatternsize + 1; j < qrcodesize - (quietzonesize + finderpatternsize + 1); ++j)
		qrcode[(quietzonesize + finderpatternsize - 1) * qrcodesize + j] = BOOL_TO_MODULE(!((j - (quietzonesize + finderpatternsize + 1)) & 0x1));
}

void AddCompulsoryDarkModule(unsigned char *qrcode, int qrcodesize, int quietzonesize, int finderpatternsize)
{
	qrcode[(qrcodesize - (quietzonesize + finderpatternsize + 1)) * qrcodesize + (quietzonesize + finderpatternsize + 1)] = DARK_MODULE;
}

void AddCodewords(const std::vector<bool>& datacodewords,
				  const std::vector<bool>& errorcodewords,
				  unsigned char *qrcode,
				  int qrcodesize,
				  int quietzonesize,
				  int finderpatternsize)
{
	auto isModuleOccupied = [&qrcode, &qrcodesize, &quietzonesize, &finderpatternsize](int row, int col) -> bool
	{
		// Check if current coordinate is available for writing by checking if they
		// have been filled with dark or light block. Additionally, make sure we are
		// not writing over version and format information modules - which will be filled
		// later after mixing step.
		if (row == (quietzonesize + finderpatternsize + 1))
		{
			// Format information reserved area under top-left and top-right finder patterns
			if (col >= quietzonesize &&
				col <= (quietzonesize + finderpatternsize))
				return true;
			else if (col >= (qrcodesize - (quietzonesize + finderpatternsize + 1)) &&
					 col < (qrcodesize - quietzonesize))
				return true;
		}
		
		if (col == (quietzonesize + finderpatternsize + 1))
		{
			// Format information reserved area under top-left and bottom-left finder patterns
			if (row >= quietzonesize &&
				row <= (quietzonesize + finderpatternsize + 1))
				return true;
			else if (row >= (qrcodesize - (quietzonesize + finderpatternsize + 1)) &&
					 row < (qrcodesize - quietzonesize))
				return true;
		}
		
		return (qrcode[row * qrcodesize + col] == DARK_MODULE ||
				qrcode[row * qrcodesize + col] == LIGHT_MODULE);
	};

	enum VerticalWalkType
	{
		Up,
		Down
	} verticalWalkType = Up;

	enum HorizontalWalkType
	{
		Current,
		Right
	} horizontalWalkType = Current;

	int rowWalk = qrcodesize - quietzonesize - 1;
	int columnWalk = qrcodesize - quietzonesize - 1;

	for (int i = 0; i < static_cast<int>(datacodewords.size() + errorcodewords.size()); ++i)
	{
		// Determine if current position is writable, if not find next
		while (isModuleOccupied(rowWalk, columnWalk))
		{
			switch (verticalWalkType)
			{
				case Up:
					{
						switch (horizontalWalkType)
						{
							case Current:
							{
								horizontalWalkType = Right;
								--columnWalk;
							}
							break;

							case Right:
							{
								horizontalWalkType = Current;
								--rowWalk;
								++columnWalk;
							}
							break;
						}

						if (rowWalk < quietzonesize)
						{
							// Time to walk down, cannot go up any further
							rowWalk = quietzonesize;
							columnWalk -= (horizontalWalkType == Current ? 2 : 1);
							verticalWalkType = Down;
							horizontalWalkType = Current;
						}
					}
					break;

				case Down:
					{
						switch (horizontalWalkType)
						{
							case Current:
							{
								horizontalWalkType = Right;
								--columnWalk;
							}
							break;

							case Right:
							{
								horizontalWalkType = Current;
								++rowWalk;
								++columnWalk;
							}
							break;
						}

						if (rowWalk >= (qrcodesize - quietzonesize))
						{
							// Time to walk up, cannot go down any further
							rowWalk = qrcodesize - quietzonesize - 1;
							columnWalk -= (horizontalWalkType == Current ? 2 : 1);
							verticalWalkType = Up;
							horizontalWalkType = Current;
						}
					}
					break;
			}

			// If we are in column with vertical timing pattern,
			// skip this column and start from next one
			if (columnWalk == (quietzonesize + finderpatternsize - 1))
				--columnWalk;
		}

		const auto data = (i < static_cast<int>(datacodewords.size()) ? datacodewords[i] : errorcodewords[i - (int)datacodewords.size()]);

		// Write data at current pos
		// NOTE: Viability of current pos is already verified
		qrcode[rowWalk * qrcodesize + columnWalk] = BOOL_TO_MODULE(data);
	}
}

std::vector<bool> GenerateMask(int indexMask, int modulepatternsize)
{
	std::vector<bool> mask(modulepatternsize * modulepatternsize);

	for (int i = 0; i < static_cast<int>(mask.size()); ++i)
	{
		int row = i / modulepatternsize;
		int column = i % modulepatternsize;

		mask[i] = maskGeneratorTable[indexMask](row, column);
	}

	return mask;
}

int DetermineMaskPatternWithLowestPenaltyScore(unsigned char *qrcode, int qrcodesize, int quietzonesize, int finderpatternsize)
{
	std::pair<int, int> maskIndexWithLowestPenaltyScore = std::make_pair(-1, std::numeric_limits<int>::max());	// NOTE: '-1' indicates invalid index

	// Apply different masks one-by-one and determine the one with lowest penalty score
	for (int indexMask = 0; indexMask < NUM_MASK_GENERATORS; ++indexMask)
	{
		const int modulessize = (qrcodesize - quietzonesize);

		// Generate mask
		std::vector<bool> mask = GenerateMask(indexMask, modulessize);

		// Copy QR Code data as boolean without quiet zone
		std::vector<bool> modules(modulessize * modulessize);
		for (int i = 0; i < modulessize; ++i)
			for (int j = 0; j < modulessize; ++j)
			{
				modules[i * modulessize + j] = MODULE_TO_BOOL(qrcode[(i + quietzonesize) * qrcodesize + (j + quietzonesize)]);
			}

		// Apply mask
		for (int i = 0; i < modulessize; ++i)
			for (int j = 0; j < modulessize; ++j)
			{
				// Non-data and error codeword areas must not be masked
				if (i == (finderpatternsize - 1) ||
					j == (finderpatternsize - 1))
				{
					// These are timing patterns, ignore
					continue;
				}
				else if (i == (modulessize - (finderpatternsize + 1)) &&
						 j == (finderpatternsize + 1))
				{
					// Compulsory dark module, ignore
					continue;
				}
				else if (i >= 0 && i <= finderpatternsize)
				{
					if ((j >= 0 && j <= finderpatternsize) ||
						(j >= (modulessize - (finderpatternsize + 1)) && j < modulessize))
					{
						// Top left and top right alignment pattern with separators, ignore
						continue;
					}
				}
				else if (i >= (modulessize - (finderpatternsize + 1)) && i < modulessize)
				{
					if (j >= 0 && j <= finderpatternsize)
					{
						// Bottom left alignment pattern with separators, ignore
						continue;
					}
				}
				
				// Check if in reserved (format information) areas - they are always light modules
				if (((i >= 0 && i < (finderpatternsize - 1)) || (i >= finderpatternsize && i <= (finderpatternsize + 1)) &&
					 j == (finderpatternsize + 1)))
				{
					// Top left side format information
					modules[i * modulessize + j] = MODULE_TO_BOOL(LIGHT_MODULE);
					continue;
				}
				else if ((i == (finderpatternsize + 1)) &&
						 ((j >= 0 && j < (finderpatternsize - 1)) || j == finderpatternsize))
				{
					// Top bottom format information
					modules[i * modulessize + j] = MODULE_TO_BOOL(LIGHT_MODULE);
					continue;
				}
				else if (((i >= (modulessize - finderpatternsize)) && (i < modulessize)) &&
						 (j == (finderpatternsize + 1)))
				{
					// Bottom right side format information
					modules[i * modulessize + j] = MODULE_TO_BOOL(LIGHT_MODULE);
					continue;
				}
				else if ((i == (finderpatternsize + 1)) &&
						 (j >= (modulessize - (finderpatternsize + 1)) && j < modulessize))
				{
					// Top right bottom format information
					modules[i * modulessize + j] = MODULE_TO_BOOL(LIGHT_MODULE);
					continue;
				}

				// Else, XOR with mask
				modules[i * modulessize + j] = modules[i * modulessize + j] ^ mask[i * modulessize + j];
			}

		// Calculate penalty
		int penaltyScore = 0;

		// Evaluation condition #1
		for (int i = 0; i < modulessize; ++i)
			for (int j = 0; j < (modulessize - 5); ++j)
			{
				// Check for 5 consecutive modules of same color, columnwise
				auto colorModule1 = modules[i * modulessize + j];
				auto colorModule2 = modules[i * modulessize + (j + 1)];
				auto colorModule3 = modules[i * modulessize + (j + 2)];
				auto colorModule4 = modules[i * modulessize + (j + 3)];
				auto colorModule5 = modules[i * modulessize + (j + 4)];

				if (colorModule1 == colorModule2 == colorModule3 == colorModule4 == colorModule5)
				{
					penaltyScore += 3;

					// Check for any modules of same color after the 5 consecutive ones
					int k = (j + 4) + 1;
					for (; k < (modulessize - 5); ++k)
					{
						if (modules[i * modulessize + k] == colorModule1)
							penaltyScore += 1;
						else
							break;
					}

					j = k - 1;	// CAUTION: We need to decrement 1 as the outer for loop will increment it
				}
			}

		for (int j = 0; j < modulessize; ++j)
			for (int i = 0; i < (modulessize - 5); ++i)
			{
				// Check for 5 consecutive modules of same color, rowwise
				auto colorModule1 = modules[i * modulessize + j];
				auto colorModule2 = modules[(i + 1) * modulessize + j];
				auto colorModule3 = modules[(i + 2) * modulessize + j];
				auto colorModule4 = modules[(i + 3) * modulessize + j];
				auto colorModule5 = modules[(i + 4) * modulessize + j];

				if (colorModule1 == colorModule2 == colorModule3 == colorModule4 == colorModule5)
				{
					penaltyScore += 3;

					// Check for any modules of same color after the 5 consecutive ones
					int k = (i + 4) + 1;
					for (; k < (modulessize - 5); ++k)
					{
						if (modules[k * modulessize + j] == colorModule1)
							penaltyScore += 1;
						else
							break;
					}

					i = k - 1;	// CAUTION: We need to decrement 1 as the outer for loop will increment it
				}
			}

		// Evaluation condition #2
		for (int i = 0; i < (modulessize - 1); ++i)
			for (int j = 0; j < (modulessize - 1); ++j)
			{
				auto colorModule = modules[i * modulessize + j];

				// Check 2x2 modules (including overlapping) for same color
				if (modules[i * modulessize + (j + 1)] == colorModule &&
					modules[(i + 1) * modulessize + j] == colorModule &&
					modules[(i + 1) * modulessize + (j + 1)] == colorModule)
					penaltyScore += 3;
			}

		// Evaluation condition #3
		const std::array<bool, 11> pattern1 = { false, true, false, false, false, true, false, true, true, true, true };
		const std::array<bool, 11> pattern2 = { true, true, true, true, false, true, false, false, false, true, false };
		assert(pattern1.size() == pattern2.size());
		for (int i = 0; i < modulessize; ++i)
			for (int j = 0; j < (modulessize - static_cast<int>(pattern1.size())); ++j)
			{
				auto itBegin = modules.cbegin() + (i * modulessize + j);
				auto itEnd = modules.cbegin() + (i * modulessize + (j + static_cast<int>(pattern1.size())));

				// Search columnwise
				if (std::equal(itBegin, itEnd, pattern1.begin()) ||
					std::equal(itBegin, itEnd, pattern2.begin()))
					penaltyScore += 40;
			}

		for (int j = 0; j < modulessize; ++j)
			for (int i = 0; i < (modulessize - static_cast<int>(pattern1.size())); ++i)
			{
				// Search rowwise
				// Gather elements from rows to compare first
				std::array<bool, 11> row;
				assert(row.size() == pattern1.size());
				for (int k = i; k < (i + row.size()); ++k)
					row[k - i] = modules[k * row.size() + j];

				if (std::equal(row.cbegin(), row.cend(), pattern1.begin()) ||
					std::equal(row.cbegin(), row.cend(), pattern2.begin()))
					penaltyScore += 40;
			}

		// Evaluation condition #4
		const int TotalModules = modulessize * modulessize;
		
		// Count dark modules
		int TotalDarkModules = 0;
		for (int i = 0; i < modulessize; ++i)
			for (int j = 0; j < modulessize; ++j)
			{
				if (modules[i * modulessize + j] == false)
					TotalDarkModules += 1;
			}

		// Calculate penalty of dark modules
		const int PercentOfDarkModules = (TotalDarkModules * 100) / TotalModules;
		const int Penalty1 = std::abs((PercentOfDarkModules % 5 == 0 ? (PercentOfDarkModules - 5) : PREVIOUSMULTIPLEOF5(PercentOfDarkModules)) - 50) / 5;
		const int Penalty2 = std::abs((PercentOfDarkModules % 5 == 0 ? (PercentOfDarkModules + 5) : NEXTMULTIPLEOF5(PercentOfDarkModules)) - 50) / 5;
		penaltyScore += std::min(Penalty1, Penalty2) * 10;

		// If this the lowest penalty score, save it
		if (penaltyScore < maskIndexWithLowestPenaltyScore.second)
			maskIndexWithLowestPenaltyScore = std::make_pair(indexMask, penaltyScore);
	}

	return maskIndexWithLowestPenaltyScore.first;
}

void ApplyMask(unsigned char *qrcode, int qrcodesize, int quietzonesize, int finderpatternsize, int indexMask)
{
	const auto maskGenerator = maskGeneratorTable[indexMask];

	for (int i = quietzonesize; i < (qrcodesize - quietzonesize); ++i)
		for (int j = quietzonesize; j < (qrcodesize - quietzonesize); ++j)
		{
			// Ignore non-codeword regions
			if (i == (quietzonesize + finderpatternsize - 1) ||
				j == (quietzonesize + finderpatternsize - 1))
			{
				// Timing patterns
				continue;
			}
			else if (i >= quietzonesize &&
					 i <= (quietzonesize + finderpatternsize + 1))
			{
				if ((j >= quietzonesize && j <= (quietzonesize + finderpatternsize + 1)) ||
					(j >= (qrcodesize - (quietzonesize + finderpatternsize + 1)) && j < (qrcodesize - quietzonesize)))
				{
					// Top left and right finder pattern, including format information areas
					continue;
				}
			}
			else if (i >= (qrcodesize - (quietzonesize + finderpatternsize + 1)) &&
					 i < (qrcodesize - quietzonesize))
			{
				if (j >= quietzonesize && j <= (quietzonesize + finderpatternsize + 1))
				{
					// Bottom left finder pattern, including format information areas
					continue;
				}
			}

			// Apply mask on data and error codeword modules
			const bool mask = maskGenerator((i - quietzonesize), (j - quietzonesize));
			const bool value = MODULE_TO_BOOL(qrcode[i * qrcodesize + j]);
			const bool maskedValue = value ^ mask;

			qrcode[i * qrcodesize + j] = BOOL_TO_MODULE(maskedValue);
			/*if (!mask)
				qrcode[i * qrcodesize + j] = BOOL_TO_MODULE(!MODULE_TO_BOOL(qrcode[i * qrcodesize + j]));*/
		}
}

void AddVersionInformation(int version, unsigned char *qrcode, int qrcodesize, int quietzonesize, int finderpatternsize)
{
	// NOT REQUIRED for version 1
}

void AddFormatInformation(char format, int indexMask, unsigned char *qrcode, int qrcodesize, int quietzonesize, int finderpatternsize)
{
	const auto formatString = mapErrorCorrectionFormatString[format][indexMask];
	int indexNextBit = 0;	// Start from least significant bit

	// Write at top left finder pattern vertically
	for (int i = quietzonesize; i <= (quietzonesize + finderpatternsize + 1); ++i)
	{
		// Step over timing pattern
		if (i == (quietzonesize + finderpatternsize - 1))
			continue;

		qrcode[i * qrcodesize + (quietzonesize + finderpatternsize + 1)] = BOOL_TO_MODULE(formatString[indexNextBit++]);
	}

	// Write under top left finder pattern horizontally backwards
	for (int j = (quietzonesize + finderpatternsize); j >= quietzonesize; --j)
	{
		// Step over timing pattern
		if (j == (quietzonesize + finderpatternsize - 1))
			continue;

		qrcode[(quietzonesize + finderpatternsize + 1) * qrcodesize + j] = BOOL_TO_MODULE(formatString[indexNextBit++]);
	}
	
	// Reset, we gotta write this information again somewhere else
	indexNextBit = 0;

	// Write under top right finder pattern horizontally backwards
	for (int j = (qrcodesize - quietzonesize - 1); j >= (qrcodesize - (quietzonesize + finderpatternsize + 1)); --j)
	{
		qrcode[(quietzonesize + finderpatternsize + 1) * qrcodesize + j] = BOOL_TO_MODULE(formatString[indexNextBit++]);
	}

	// Write besides bottom left finder pattern vertically
	for (int i = (qrcodesize - (quietzonesize + finderpatternsize)); i < (qrcodesize - quietzonesize); ++i)
	{
		qrcode[i * qrcodesize + (quietzonesize + finderpatternsize + 1)] = BOOL_TO_MODULE(formatString[indexNextBit++]);
	}
}

void PrintQRCode(unsigned char *qrcode, int qrcodesize)
{
	for (int i = 0; i < qrcodesize; ++i)
	{
		std::cout << '\t';

		for (int j = 0; j < qrcodesize; ++j)
			std::cout << qrcode[i * qrcodesize + j];

		std::cout << std::endl;
	}
}

void PrintQRCode(const std::vector<bool>& qrcode, int qrcodesize)
{
	for (int i = 0; i < qrcodesize; ++i)
	{
		std::cout << '\t';

		for (int j = 0; j < qrcodesize; ++j)
			std::cout << BOOL_TO_MODULE(qrcode[i * qrcodesize + j]);

		std::cout << std::endl;
	}
}

#if defined(DEBUG) || defined(_DEBUG)
void PrintVectorBool(const std::vector<bool>& bits)
{
	std::cout << std::endl;

	for (size_t i = 0; i < bits.size(); ++i)
	{
		std::cout << BOOL_TO_BIT(bits[i]);

		if ((i + 1) % CODEWORD_SIZE == 0)
			std::cout << ' ';
	}

	std::cout << std::endl;
}

template <size_t Size>
void PrintBitSet(const std::bitset<Size>& bits)
{
	std::cout << std::endl;

	for (size_t i = bits.size(); i > 0; --i)
	{
		std::cout << BOOL_TO_BIT(bits[i - 1]);

		if ((i + 1) % CODEWORD_SIZE == 0)
			std::cout << ' ';
	}

	std::cout << std::endl;
}
#else
#define PrintVectorBool(expression, ...)	((void)0)
#define PrintBitSet(expression, ...)		((void)0)
#endif