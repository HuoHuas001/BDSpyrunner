#pragma once
#include <string>
#include <memory>

struct ReadOnlyBinaryStream {
public:
	int (**_vptr$ReadOnlyBinaryStream)(void);
	size_t mReadPointer;
	size_t unk1;
	const std::string mOwnedBuffer;
	const std::string* mBuffer;
	ReadOnlyBinaryStream();
	ReadOnlyBinaryStream(std::string* buffer);
	ReadOnlyBinaryStream(const std::string* buffer, bool copyBuffer);
	~ReadOnlyBinaryStream();
	inline size_t getLength() const { return mBuffer->size(); }
	inline void setReadPointer(std::size_t size) {
		auto len = getLength();
		if (size <= len)
			mReadPointer = size;
		else
			mReadPointer = len;
	}
};

class BinaryStream : public ReadOnlyBinaryStream {
public:
	std::string mOwnedBuffer;
	std::string* mBuffer;
	BinaryStream();
	BinaryStream(std::string* buffer, bool copy);
	~BinaryStream();
	std::string getAndReleaseData();
	void writeUnsignedInt(unsigned int num);
	inline void reset() {
		mOwnedBuffer.clear();
		setReadPointer(0);
	}
	inline void reserve(size_t size) { mOwnedBuffer.reserve(size); }
};

template <typename T>
class serialize {
public:
	static void write(T* val, BinaryStream* stream);
	static std::unique_ptr<T> read(BinaryStream* stream);
};

