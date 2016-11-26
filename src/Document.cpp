/*
 * Document.cpp
 *
 *  Created on: 2008.07.25.
 *      Author: ibiro
 */

#include "Document.h"
#include "Document-inl.h"

Document::Document(uint32_t docID, const VI& words) :docID_(docID), words_(words) {
	length_ = words_.size();
}




