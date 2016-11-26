/*
 * Corpus.h
 *
 *  Created on: 2008.09.28.
 *      Author: ibiro
 */

#ifndef CORPUS_H_
#define CORPUS_H_

#include "Document.h"
#include "Document-inl.h"
#include "Vocabulary.h"
#include "Vocabulary-inl.h"

typedef map<uint32_t, Document> Dataset;

class Corpus {
public:
	Corpus(string corpusFileName, string vocabFileName, Vocabulary* vocab);
	Corpus(string corpusFileName, Vocabulary* vocab);
	virtual ~Corpus();
	int read(string corpusFileName, string vocabFileName, Vocabulary* vocab);
	int readWithVocab(string corpusFileName, Vocabulary* vocab);
	int write(ostream& out);
	inline Dataset&  dataset() {return dataset_;}
	Document& getDocumentByID(uint32_t docID);
	inline uint32_t datasetSize() { return datasetSize_;}
	bool isContained(uint32_t docID);

private:
	Dataset dataset_;
	uint32_t datasetSize_;
};

#endif /* CORPUS_H_ */
