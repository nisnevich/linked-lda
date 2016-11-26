/*
 * Document-inl.h
 *
 *  Created on: 2008.09.29.
 *      Author: ibiro
 */

#ifndef DOCUMENTINL_H_
#define DOCUMENTINL_H_

#include "Document.h"

inline uint32_t Document::getWord(uint32_t pos){
	assert(pos>=0 && pos<length_);
	return words_[pos];
}


#endif /* DOCUMENTINL_H_ */
