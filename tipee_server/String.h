// -*- C++ -*-
#ifndef STRING_HEADER
#define STRING_HEADER

class String {
private:
	// バッファオーバーフローを起こさない様に
	// サイズをバッファサイズを制限
	enum {
		MAX_BUFFER_SIZE = 1024,
	};

	int bufferSize; // 文字サイズ+1
	int lineSize;
	char* buffer;
	
	int calcUTFf8Length(int startPos, int endPos) const;
	
public:
	String(const char* str);
	~String();
	int getLineSize() const;
	int getLineStartBufferPos(int lineIndex) const;
	int getBufferSizeAtLine(int lineIndex) const;
	int getUTF8LineLength(int lineIndex) const;
	const char* getLineBuffer(int lineIndex) const;
	int getMaxUTF8LineLength() const;
};


#endif
