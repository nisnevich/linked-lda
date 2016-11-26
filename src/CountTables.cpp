/*
 * CountTables.cpp
 *
 *  Created on: 2008.09.25.
 *      Author: ibiro
 */

#include "CountTables.h"
#include "CountTables-inl.h"

/*
CountTables::CountTables(uint32_t topicSize, uint32_t vocabSize,
						 LinkGraph& linkGraph, Corpus& corpus)
	: topicSize_(topicSize),
	  vocabSize_(vocabSize),
	  linkGraph_(linkGraph),
	  corpus_(corpus) {

	init();

}
*/

CountTables::CountTables(uint32_t topicSize, uint32_t vocabSize,
						 LinkGraph* linkGraph, Corpus* corpus)
	: topicSize_(topicSize),
	  vocabSize_(vocabSize) {

	linkGraph_ = linkGraph;
	corpus_ = corpus;
	init();

}

//TODO:better code-design for handling the training and test initialization (init() and initInf())
CountTables::CountTables(uint32_t topicSize, uint32_t vocabSize, LinkGraph* linkGraph,
						 Corpus* corpus,
						 istream& topicLabelFile, istream& refDocLabelFile)
	: topicSize_(topicSize), vocabSize_(vocabSize) {

	linkGraph_ = linkGraph;
	corpus_ = corpus;
	read(topicLabelFile, topicLabels_);
	read(refDocLabelFile, refDocLabels_);
	init();
	fillCounters();
}

void CountTables::change_ns(uint32_t docID, uint32_t inlinkID, int32_t value) {
	//ns update
	MIMII::iterator foundDocIter = ns_.find(docID);
	if(foundDocIter!=ns_.end()){
		MII::iterator foundRefDocIter = foundDocIter->second.find(inlinkID);
		if(foundRefDocIter!=foundDocIter->second.end()){
			foundRefDocIter->second += value;
		}
		else {
			foundDocIter->second.insert(make_pair(inlinkID, value));
		}
	}
	else {
		MII inlinkMap;
		inlinkMap.insert(make_pair(inlinkID, value));
		ns_.insert(make_pair(docID, inlinkMap));
	}
	//nsSum update
	MII::iterator found = nsSum_.find(docID);
	if(found != nsSum_.end()){
		found->second += value;
	}
	else{
		nsSum_.insert(make_pair(docID, value));
	}
}

void CountTables::fillCounters() {

	MIVI::iterator refDocLabelsIter = refDocLabels_.begin();
	for(MIVI::iterator topicLabelsIter = topicLabels_.begin();
		topicLabelsIter != topicLabels_.end(); ++topicLabelsIter, ++refDocLabelsIter) {

		uint32_t docID = topicLabelsIter->first;
		Document currentDoc = corpus_->getDocumentByID(docID);
		VI words = currentDoc.words();
		VI::iterator wordsIter=words.begin();
		VI::iterator refDocPosIter = refDocLabelsIter->second.begin();
		for(VI::iterator topicPosIter = topicLabelsIter->second.begin();
			topicPosIter != topicLabelsIter->second.end(); ++topicPosIter, ++wordsIter, ++refDocPosIter) {
			change_nw(*wordsIter, *topicPosIter, 1);
			change_nd(*refDocPosIter, *topicPosIter, 1);
			change_ns(docID, *refDocPosIter, 1);
		}
		assert(wordsIter==words.end());
		assert(refDocPosIter==refDocLabelsIter->second.end());
	}
	assert(refDocLabelsIter==refDocLabels_.end());
}

/*
CountTables::CountTables(uint32_t topicSize, uint32_t vocabSize,
						 LinkGraph *linkGraph, Corpus *corpus,
						 istream & nwFile, istream & nwSumFile,
						 istream & ndFile, istream & ndSumFile)

	: topicSize_(topicSize),
	  vocabSize_(vocabSize) {

	linkGraph_ = linkGraph;
	corpus_ = corpus;
	read(nwFile, nw_);
	read(nwSumFile, nwSum_);
	read(ndFile, nd_);
	read(ndSumFile, ndSum_);
	update();
}
*/

CountTables::~CountTables() {
	// TODO Auto-generated destructor stub
}

/*

void CountTables::init() {

	//initialize nw

	nw_.resize(vocabSize_);
	for(uint32_t i=0; i<vocabSize_; ++i){
		VI row(topicSize_, 0);
		nw_[i]=row;
	}

	//initialize nwSum
	//TODO:consistent matrix formulation: nw should be K x V, so as nwSum
	nwSum_.resize(topicSize_);
	for(uint32_t i=0; i<topicSize_; ++i){
		nwSum_[i]=0;
	}

	//initialize nd, ndSum, ns, nsSum
	GraphType graph = linkGraph_->graph();
	for(GraphType::const_iterator graphIter=graph.begin(); graphIter!=graph.end(); ++graphIter){
		uint32_t docID = graphIter->first;
		uint32_t inDegree = graphIter->second.size();
		//ns, nsSum
		MII nsRow;
		for(VI::const_iterator inlinksIter = graphIter->second.begin();
			inlinksIter!=graphIter->second.end(); ++inlinksIter) {
			nsRow.insert(make_pair(*inlinksIter, 0));
		}
		assert(nsRow.size() == inDegree);
		ns_.insert(make_pair(docID, nsRow));
		nsSum_.insert(make_pair(docID, 0));
		//nd, ndSum
		VI ndRow(topicSize_, 0);
		nd_.insert(make_pair(docID, ndRow));
		ndSum_.insert(make_pair(docID, 0));
	}

	//initialize topicLabels_ and refDocLabels_
	assert(linkGraph_->graphSize() == corpus_->datasetSize());
	Dataset dataset = corpus_->dataset();
	for(Dataset::const_iterator corpusIter = dataset.begin();
		corpusIter!=dataset.end(); ++corpusIter) {
		uint32_t docLength = corpusIter->second.length();
		uint32_t docID = corpusIter->first;
		VI row(docLength, 0);
		topicLabels_.insert(make_pair(docID, row));
		refDocLabels_.insert(make_pair(docID, row));
	}
}

*/

void CountTables::init() {

	//initialize nw_
	nw_.resize(vocabSize_);
	for(uint32_t i=0; i<vocabSize_; ++i){
		VI row(topicSize_, 0);
		nw_[i]=row;
	}
	//initialize nwSum_
	//TODO:consistent matrix formulation: nw should be K x V, so as nwSum
	nwSum_.resize(topicSize_);
	for(uint32_t i=0; i<topicSize_; ++i){
		nwSum_[i]=0;
	}
}

void CountTables::write(ostream& out) {

	out << "nw\n";
	write(nw_, out);
	out << "nwSum\n";
	write(nwSum_, out);
	out << "nd\n";
	write(nd_, out);
	out << "ndSum\n";
	write(ndSum_, out);
	out << "ns\n";
	write(ns_, out);
	out << "nsSum\n";
	write(nsSum_, out);
	out << "topicLabels\n";
	write(topicLabels_, out);
	out << "refDocLabels\n";
	write(refDocLabels_, out);

}

void CountTables::write(VI& data, ostream& out) {
	for(VI::iterator iter = data.begin(); iter!=data.end(); ++iter) {
		out << *iter << " ";
	}
	out << endl;
}

void CountTables::write(VVI& data, ostream& out) {
	for(VVI::iterator iter=data.begin();iter!=data.end();++iter){
		write(*iter,out);
	}
}

void CountTables::write(MII& data, ostream& out) {
	for(MII::iterator iter=data.begin(); iter!=data.end(); ++iter) {
		out << iter->first << ":" << iter->second << " ";
	}
	out << endl;
}

void CountTables::write(MIVI& data, ostream& out) {
	for(MIVI::iterator iter=data.begin(); iter!=data.end();++iter){
		out << iter->first << " ";
		write(iter->second, out);
	}
}

void CountTables::write(MIMII& data, ostream& out) {
	for(MIMII::iterator iter=data.begin();iter!=data.end();++iter){
		out << iter->first << " ";
		write(iter->second, out);
	}
}

void CountTables::read(istream& in, VVI& data) {

	assert(in);
	string line;
	VI row;
	uint32_t value;
	while(getline(in, line)){
		row.clear();
		stringstream ss(line);
		while(ss >> value){
			row.push_back(value);
		}
		data.push_back(row);
	}
}

void CountTables::read(istream& in, VI& data) {

	assert(in);
	string line;
	uint32_t value;
	getline(in, line);
	stringstream ss(line);
	data.clear();
	while(ss >> value){
		data.push_back(value);
	}
}

void CountTables::read(istream& in, MIVI& data) {

	assert(in);
	string line;
	VI row;
	uint32_t docID;
	uint32_t value;
	while(getline(in, line)){
		row.clear();
		stringstream ss(line);
		ss >> docID;
		while(ss >> value){
			row.push_back(value);
		}
		pair<MIVI::iterator, bool> result = data.insert(make_pair(docID, row));
		assert(result.second);
	}
}

void CountTables::read(istream& in, MII& data) {

	assert(in);
	string line;
	string entry;
	getline(in, line);
	data.clear();
	stringstream ss(line);
	while(ss >> entry){
		VS items = split(entry);
		assert(items.size() == 2);
		pair<MII::iterator, bool> result = data.insert(make_pair(atoi(items[0].c_str()),
													   atoi(items[1].c_str())));
		assert(result.second);
	}
}

void CountTables::read(istream& in, MIMII& data) {

	assert(in);
	string line;
	string entry;
	uint32_t docID;
	MII row;
	while(getline(in, line)) {
		row.clear();
		stringstream ss(line);
		ss >> docID;
		while(ss >> entry){
			VS items = split(entry);
			assert(items.size() == 2);
			pair<MII::iterator, bool> result = row.insert(make_pair(atoi(items[0].c_str()),
														  atoi(items[1].c_str())));
			assert(result.second);
		}
		pair<MIMII::iterator, bool> result = data.insert(make_pair(docID, row));
		assert(result.second);
	}
}
