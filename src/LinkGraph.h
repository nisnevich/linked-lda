/*
 * LinkGraph.h
 *
 *  Created on: 2008.07.25.
 *      Author: ibiro
 */

#ifndef LINKGRAPH_H_
#define LINKGRAPH_H_

#include "Utils.h"
#include "MersenneTwister.h"

typedef map<uint32_t, VI> GraphType;


class LinkGraph {

public:
	//LinkGraph(string graphFileName, uint32_t startDocID);
	explicit LinkGraph(string graphFileName);
	//TODO: why should I use this?
	/*
	LinkGraph(const LinkGraph& linkGraph);
	LinkGraph& operator=(const LinkGraph& linkGraph);
	*/
	~LinkGraph(){graph_.clear();}
	VI& getLinks(uint32_t docID);
	uint32_t getIndegree(uint32_t docID);
	uint32_t getRandomInlinkID(uint32_t docID);
	//inline GraphType graph() const {return graph_;};
	inline GraphType& graph() {return graph_;};
	inline uint32_t graphSize(){return graphSize_;}
	inline MTRand mtRand() {return mtRand_;}
	int readLinkGraph(string linkGraphName);
	bool graphCheck();

private:
	GraphType graph_;
    uint32_t graphSize_;
    MTRand mtRand_;

};

#endif /* LINKGRAPH_H_ */
