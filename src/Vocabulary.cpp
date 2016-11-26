/*
 * Vocabulary.cpp
 *
 *  Created on: 2008.07.25.
 *      Author: ibiro
 */

#include "Vocabulary.h"
#include "Vocabulary-inl.h"

Vocabulary::Vocabulary(string vocabFile){
	vocabSize_ = 0;
	assert(read(vocabFile)==0);
}

int Vocabulary::insert(string word){

	if(word2id_.find(word)==word2id_.end()){
		uint32_t newID = vocabSize_;
		word2id_.insert(make_pair(word, vocabSize_));
		id2word_.insert(make_pair(vocabSize_,word));
		vocabSize_++;
	    return newID;
	}
	else {
		return word2id_[word];
	}
}

int Vocabulary::insert(string word, uint32_t wordID){
	assert(word2id_.find(word)==word2id_.end());
	word2id_.insert(make_pair(word, wordID));
	id2word_.insert(make_pair(wordID,word));
	return 0;
}


int Vocabulary::write(string vocabFile){
	ofstream out(vocabFile.c_str());
	assert(out);
	for(vocabType::iterator iter=word2id_.begin();iter!=word2id_.end();++iter){
		out << iter->first << " " << iter->second << endl;
	}
	out.close();
	return 0;
}

//TODO: instead of a single space, use special separator in the vocabulary file (e.g. double colon)
int Vocabulary::read(string vocabFile){
	ifstream in(vocabFile.c_str());
	assert(in);
	string line;
	while(getline(in, line)){
		VS items = split(line);
		assert(items.size() == 2);
		insert(items[0], atoi(items[1].c_str()));
	}
	assert(word2id_.size()==id2word_.size());
	vocabSize_ = word2id_.size();
	in.close();
	return 0;
}

