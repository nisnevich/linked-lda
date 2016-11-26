/*
 * LinkGraph-inl.h
 *
 *  Created on: 2008.09.29.
 *      Author: ibiro
 */

#ifndef LINKGRAPHINL_H_
#define LINKGRAPHINL_H_

#include "LinkGraph.h"

inline VI& LinkGraph::getLinks(uint32_t docID){

	GraphType::iterator found = graph_.find(docID);
	if(found != graph_.end()){
		return found->second;
	}
	else {
		cerr << "DocID not exist!\n";
		exit(1);
	}

}

inline uint32_t LinkGraph::getIndegree(uint32_t docID){
	return getLinks(docID).size();
}

inline uint32_t LinkGraph::getRandomInlinkID(uint32_t docID){

	VI links = getLinks(docID);
	return links[mtRand_.randInt(links.size()-1)];
}


#endif /* LINKGRAPHINL_H_ */
