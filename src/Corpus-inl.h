/*
 * Corpus-inl.h
 *
 *  Created on: 2008.09.29.
 *      Author: ibiro
 */

#ifndef CORPUSINL_H_
#define CORPUSINL_H_

#include "Corpus.h"

inline Document& Corpus::getDocumentByID(uint32_t docID) {
	Dataset::iterator found = dataset_.find(docID);
	if(found != dataset_.end()){
		return found->second;
	}
	else{
		cerr << "DocID not exist!\n";
		exit(1);
	}
}

inline bool Corpus::isContained(uint32_t docID) {
	Dataset::iterator found = dataset_.find(docID);
	return found != dataset_.end();
}

#endif /* CORPUSINL_H_ */
