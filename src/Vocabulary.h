/*
 * Vocabulary.h
 *
 *  Created on: 2008.07.25.
 *      Author: ibiro
 */

#ifndef VOCABULARY_H_
#define VOCABULARY_H_

#include "Utils.h"

typedef map<string, uint32_t> vocabType;
typedef map<uint32_t, string> invVocabType;

class Vocabulary {
public:

	Vocabulary():vocabSize_(0){}
	Vocabulary(string vocabFile);
	~Vocabulary(){}
	int insert(string word);
	int insert(string word, uint32_t wordID);
	int getWordID(string word);
	string getWord(uint32_t wordID);
    inline uint32_t vocabSize() {return vocabSize_;}
	int read(string vocabFile);
	int write(string vocabFile);

private:
	vocabType word2id_;
	invVocabType id2word_;
	uint32_t vocabSize_;

};

#endif /* VOCABULARY_H_ */
