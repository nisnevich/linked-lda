/*
 * Corpus.cpp
 *
 *  Created on: 2008.09.28.
 *      Author: ibiro
 */

#include "Corpus.h"
#include "Corpus-inl.h"


Corpus::Corpus(string corpusFileName, string vocabFileName, Vocabulary* vocab)
	: datasetSize_(0) {
	read(corpusFileName, vocabFileName, vocab);
}

Corpus::Corpus(string corpusFileName, Vocabulary* vocab) : datasetSize_(0) {
	readWithVocab(corpusFileName, vocab);
}

Corpus::~Corpus() {
	// TODO Auto-generated destructor stub
}

//TODO:documentation of corpus-file format.

int Corpus::read(string corpusFileName, string vocabFileName, Vocabulary *vocab) {

	ifstream corpusFile(corpusFileName.c_str());
	assert(corpusFile);
	string line;
	string word;
	VI row;
	uint32_t docID;
	getline(corpusFile, line);
	uint32_t corpusSize = atoi(line.c_str());
	assert(corpusSize);
	while(getline(corpusFile, line)){
		stringstream ss(line);
		row.clear();
		ss >> docID;
		while(ss >> word){
			row.push_back(vocab->insert(word));
		}
		Document doc(docID, row);
		msgerr(doc.length());
		pair<Dataset::iterator, bool> result = dataset_.insert(make_pair(docID, doc));
		assert(result.second);
	}
	assert(dataset_.size() == corpusSize);
	datasetSize_ = corpusSize;
	vocab->write(vocabFileName);
	return 0;
}

int Corpus::readWithVocab(string corpusFileName, Vocabulary* vocab) {

	assert(vocab);
	ifstream corpusFile(corpusFileName.c_str());
	assert(corpusFile);
	string line;
	string word;
	VI row;
	uint32_t docID;
	getline(corpusFile, line);
	uint32_t corpusSize = atoi(line.c_str());
	assert(corpusSize);
	while(getline(corpusFile, line)){
		stringstream ss(line);
		row.clear();
		ss >> docID;
		while(ss >> word){

			int32_t wordID = vocab->getWordID(word);
			if(wordID == -1){
				//if the word is not in the vocab we insert it.
				row.push_back(vocab->insert(word));
			}
			else {
				row.push_back(wordID);
			}
		}
		Document doc(docID, row);
		pair<Dataset::iterator, bool> result = dataset_.insert(make_pair(docID, doc));
		assert(result.second);
	}
	assert(dataset_.size() == corpusSize);
	datasetSize_ = corpusSize;
	return 0;
}

int Corpus::write(ostream& out) {
	for(Dataset::iterator iter=dataset_.begin(); iter!=dataset_.end();++iter){
		for(uint32_t i=0; i<iter->second.length(); ++i){
			out << iter->second.getWord(i) << " ";
		}
		out << endl;
	}
	return 0;
}






