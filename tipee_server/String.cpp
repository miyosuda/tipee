#include "String.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * <!--  calcUTFf8Length():  -->
 *
 * UTF8での文字列長を算出
 */
int String::calcUTFf8Length(int startPos, int endPos) const {
	int length = 0;
	for( int i=startPos; i<endPos; ++i ) { 
		if((buffer[i] & 0xC0) != 0x80) {
			length++;
		}
	}
	return length;
}

/**
 * <!--  String():  -->
 *
 * 複数行のUTF8文字列管理
 */
String::String(const char* str) {
	int len = strlen(str);
	bufferSize = len+1;
	bool trimed = false;
	if( bufferSize > MAX_BUFFER_SIZE ) {
		bufferSize = MAX_BUFFER_SIZE;
		trimed = true;
	}
	buffer = (char*)malloc(bufferSize);
	memcpy(buffer, str, bufferSize);
	lineSize = 1;
		
	// 改行位置をNULL文字に変えておく
	for(int i=0; i<bufferSize; ++i)  {
		if( buffer[i] == '\n' ) {
			buffer[i] = '\0';
			lineSize++;
		}
	}
	if( trimed ) {
		buffer[bufferSize-1] = '\0';
	}
}
	
/**
 * <!--  ~String():  -->
 */
String::~String() {
	free(buffer);
}
	
/**
 * <!--  getLineSize():  -->
 */
int String::getLineSize() const {
	return lineSize;
}

/**
 * <!--  getLineStartBufferPos():  -->
 */
int String::getLineStartBufferPos(int lineIndex) const {
	if( lineIndex == 0 ) {
		return 0;
	} else {
		int count = 0;
		for(int i=0; i<bufferSize; ++i) {
			if( buffer[i] == '\0' ) {
				count++;
				if( count == lineIndex ) {
					return i+1;
				}
			}
		}
		return -1;
	}
}
	
/**
 * @return 各lineの'\0'を含まないバッファ長.
 */
int String::getBufferSizeAtLine(int lineIndex) const {
	int startPos = getLineStartBufferPos(lineIndex);
	for(int i=startPos; i<bufferSize; ++i) {
		if( buffer[i] == '\0' ) {
			return i-startPos;
		}
	}
	return 0;
}
	
/**
 * @return 各lineのUTF8での文字列長.
 */
int String::getUTF8LineLength(int lineIndex) const {
	int startPos = getLineStartBufferPos(lineIndex);
	int lineBufferSize = getBufferSizeAtLine(lineIndex);
	int endPos = startPos + lineBufferSize;
	int utf8Size = calcUTFf8Length(startPos, endPos);
	return utf8Size;
}

/**
 * <!--  getLineBuffer():  -->
 */
const char* String::getLineBuffer(int lineIndex) const {
	int startPos = getLineStartBufferPos(lineIndex);
	const char* buf = buffer;
	return buf + startPos;
}

/**
 * <!--  getMaxUTF8LineLength():  -->
 */
int String::getMaxUTF8LineLength() const {
	int maxSize = 0;
	for(int i=0; i<lineSize; ++i) {
		int size = getUTF8LineLength(i);
		if( size > maxSize ) {
			maxSize = size;
		}
	}
	return maxSize;
}
