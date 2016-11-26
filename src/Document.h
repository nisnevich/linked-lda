#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <climits>
#include "Utils.h"

class Document {

public:
	Document() : docID_(UINT_MAX), length_(0){}
	Document(uint32_t docID, const VI& words);
	~Document(){words_.clear();}

	inline uint32_t length() const { return length_;}
	inline uint32_t docID() const {return docID_;}
	inline VI& words() { return words_;}
	uint32_t getWord(uint32_t pos);

private:
	uint32_t docID_;
	VI words_;
	uint32_t length_;
};

#endif /*DOCUMENT_H_*/
