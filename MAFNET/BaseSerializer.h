#pragma once
#include <stdint.h>
#include <memory>

namespace MAFNET {
	uint32_t calculateNumBits(uint64_t value) {
		if (value == 0)
			return 0;

		static const int tab64[64] = {
			63,  0, 58,  1, 59, 47, 53,  2,
			60, 39, 48, 27, 54, 33, 42,  3,
			61, 51, 37, 40, 49, 18, 28, 20,
			55, 30, 34, 11, 43, 14, 22,  4,
			62, 57, 46, 52, 38, 26, 32, 41,
			50, 36, 17, 19, 29, 10, 13, 21,
			56, 45, 25, 31, 35, 16,  9, 12,
			44, 24, 15,  8, 23,  7,  6,  5 };

		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		value |= value >> 32;
		return tab64[((uint64_t)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58] + 1;
	}

	class BitPacker {
	private:
		const uint64_t dataArraySize;
		uint64_t* dataArray;
		uint64_t posInArray;
		uint64_t bitsIntoCurrentData;
		uint64_t numBitsWritten;
	public:
		BitPacker(uint64_t* dataArray, const uint64_t dataArraySize) : dataArray(dataArray), bitsIntoCurrentData(0), posInArray(0), dataArraySize(dataArraySize), numBitsWritten(0) {}

		void setPackerPosition(uint64_t posInArray, uint64_t bitsIntoDWORD) {
			this->bitsIntoCurrentData = bitsIntoDWORD;
			this->posInArray = posInArray;
		}

		bool serializeUint64(const uint64_t value, const uint64_t min = 0, const uint64_t max = UINT64_MAX) {
			if (value < min || value > max)
				return false;

			const uint64_t range = max;
			const uint64_t valBitSize = calculateNumBits(range);
			const uint64_t valToCopy = value;

			dataArray[posInArray] |= (valToCopy << bitsIntoCurrentData);
			bitsIntoCurrentData += valBitSize;

			if (bitsIntoCurrentData >= 64) {
				uint64_t mask = (1ULL << (bitsIntoCurrentData - 64ULL)) - 1ULL;

				uint64_t hi = (valToCopy >> (valBitSize - (bitsIntoCurrentData - 64ULL))) & mask;

				if (posInArray == dataArraySize - 1)
					return false;

				posInArray++;
				dataArray[posInArray] |= hi;
				bitsIntoCurrentData -= 64ULL;
			}

			numBitsWritten += valBitSize;
			return true;
		}

		const uint64_t getNumBitsWritten() const {
			return numBitsWritten;
		}
	};

	class BitUnpacker {
	private:
		const uint64_t dataArraySize;
		uint64_t* dataArray;
		uint64_t posInArray;
		uint64_t bitsIntoCurrentData;
		uint64_t numBitsRead;
	public:
		BitUnpacker(uint64_t* dataArray, const uint64_t dataArraySize) : dataArray(dataArray), bitsIntoCurrentData(0), posInArray(0), dataArraySize(dataArraySize), numBitsRead(0) {}

		void setPackerPosition(uint64_t posInArray, uint64_t bitsIntoDWORD) {
			this->bitsIntoCurrentData = bitsIntoDWORD;
			this->posInArray = posInArray;
		}

		bool serializeUint64(uint64_t& value, const uint64_t min = 0, const uint64_t max = UINT64_MAX) {
			const uint64_t range = max;
			const uint64_t valBitSize = calculateNumBits(range);

			if (bitsIntoCurrentData + valBitSize >= 64) {
				const uint64_t numLoBits = 64ULL - bitsIntoCurrentData;
				const uint64_t numHiBits = valBitSize - numLoBits;

				uint64_t mask = (1ULL << numLoBits) - 1ULL;
				if (numLoBits == 64)
					mask = UINT64_MAX;

				value = (dataArray[posInArray] >> bitsIntoCurrentData) & (mask);

				if (posInArray == dataArraySize - 1)
					return false;

				posInArray++;
				bitsIntoCurrentData = 0;

				mask = (1ULL << numHiBits) - 1ULL;
				uint64_t a = ((dataArray[posInArray] >> bitsIntoCurrentData) & mask);
				value |= a << (valBitSize - numHiBits);
				bitsIntoCurrentData = numHiBits;
			}
			else {
				uint64_t mask = (1ULL << valBitSize) - 1ULL;
				value = (dataArray[posInArray] >> bitsIntoCurrentData) & mask;
				bitsIntoCurrentData += valBitSize;
			}

			if (value < min || value > max)
				return false;

			numBitsRead += valBitSize;
			return true;
		}

		const uint64_t getNumBitsRead() const{
			return numBitsRead;
		}
	};

}