/*
 * CountTables-inl.h
 *
 *  Created on: 2008.09.29.
 *      Author: ibiro
 */

#ifndef COUNTTABLESINL_H_
#define COUNTTABLESINL_H_

#include "CountTables.h"

inline void CountTables::change_nw(uint32_t wordID, uint32_t topic, int32_t value) {
	nw_[wordID][topic] += value;
	nwSum_[topic] += value;
}

inline void CountTables::change_nd(uint32_t docID, uint32_t topic, int32_t value) {

	MIVI::iterator found = nd_.find(docID);
	if(found!=nd_.end()) {
		//TODO:change to []
		//found->second.at(topic) += value;
		found->second[topic] += value;
		ndSum_[docID] += value;
	}
	else {
		VI newRow(topicSize_, 0);
		newRow[topic] = value;
		nd_.insert(make_pair(docID, newRow));
		ndSum_.insert(make_pair(docID, value));
	}
}

inline void CountTables::change_topicLabels(uint32_t docID, uint32_t pos, uint32_t topicValue) {

	MIVI::iterator foundDocIter = topicLabels_.find(docID);
	if(foundDocIter != topicLabels_.end()){
		assert(pos>=0 && pos<foundDocIter->second.size());
		foundDocIter->second[pos]=topicValue;
	}
	else{
		cerr << "Bad initialization of topicLabels_!\n";
		exit(1);
	}
}

inline void CountTables::change_refDocLabels(uint32_t docID, uint32_t pos, uint32_t refDocIDValue) {

	MIVI::iterator foundDocIter = refDocLabels_.find(docID);
	if(foundDocIter != refDocLabels_.end()){
		assert(pos>=0 && pos<foundDocIter->second.size());
		foundDocIter->second[pos] = refDocIDValue;
	}
	else{
		cerr << "Bad initialization of refDocLabels_!\n";
		exit(1);
	}
}

inline void CountTables::change_topicLabels(uint32_t docID, VI& labelVector) {
	MIVI::iterator foundDocIter = topicLabels_.find(docID);
	if(foundDocIter != topicLabels_.end()){
		foundDocIter->second = labelVector;
	}
	else{
		topicLabels_.insert(make_pair(docID, labelVector));
	}
}

inline void CountTables::change_refDocLabels(uint32_t docID, VI& labelVector) {
	MIVI::iterator foundDocIter = refDocLabels_.find(docID);
	if(foundDocIter != refDocLabels_.end()){
		foundDocIter->second = labelVector;
	}
	else{
		refDocLabels_.insert(make_pair(docID, labelVector));
	}
}



#endif /* COUNTTABLESINL_H_ */
