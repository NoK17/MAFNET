#pragma once
#include <stdint.h>
#include <memory>

namespace MAFNET {
	uint32_t calculateNumBits(uint64_t value);

	class BitPacker {
	private:
		const uint64_t dataArraySize;
		uint64_t* dataArray;
		uint64_t posInArray;
		uint64_t bitsIntoCurrentData;
		uint64_t numBitsWritten;
	public:
		BitPacker(uint64_t* dataArray, const uint64_t dataArraySize);
		void setPackerPosition(uint64_t posInArray, uint64_t bitsIntoDWORD);
		bool serializeUint64(const uint64_t value, const uint64_t min = 0, const uint64_t max = UINT64_MAX);
		const uint64_t getNumBitsWritten() const;
	};

	class BitUnpacker {
	private:
		const uint64_t dataArraySize;
		uint64_t* dataArray;
		uint64_t posInArray;
		uint64_t bitsIntoCurrentData;
		uint64_t numBitsRead;
	public:
		BitUnpacker(uint64_t* dataArray, const uint64_t dataArraySize);
		void setPackerPosition(uint64_t posInArray, uint64_t bitsIntoDWORD);
		bool serializeUint64(uint64_t& value, const uint64_t min = 0, const uint64_t max = UINT64_MAX);
		const uint64_t getNumBitsRead() const;
	};

}