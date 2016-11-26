/*
 * Vocabulary-inl.h
 *
 *  Created on: 2008.09.29.
 *      Author: ibiro
 */

#ifndef VOCABULARYINL_H_
#define VOCABULARYINL_H_

#include "Vocabulary.h"

inline int Vocabulary::getWordID(string word){
	vocabType::iterator found = word2id_.find(word);
	if(found!=word2id_.end()){
		return found->second;
	}
	return -1;
}

inline string Vocabulary::getWord(uint32_t wordID){
	invVocabType::iterator found = id2word_.find(wordID);
	if(found!=id2word_.end()){
		return found->second;
	}
	return "__UNKNOWN__";
}

#endif /* VOCABULARYINL_H_ */
