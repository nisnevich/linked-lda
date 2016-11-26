/*
 * LinkGraph.cpp
 *
 *  Created on: 2008.07.25.
 *      Author: ibiro
 */

#include "LinkGraph.h"
#include "LinkGraph-inl.h"

/*
 * Graph format : line i contains the inlinks of documents "i", docID range for training corpus
 * [0...corpusSize-1] while for testCorpus [corpusSize...corpusSize+infCorpusSize-1], the graph
 * for trainingCorpus filtered, only the trainingCorpus links kept, links between a training
 * and a test document discarded. The graph for the testCorpus contains inlinks from training
 * as well as test documents.
 * Currently, linkGraphFile should be contained the self links, in order to it let be consistent
 * with the linkLDA model (loop models the situation when the word generated from the own topic of
 * the document.)
 *
 */

LinkGraph::LinkGraph(string linkGraphName) : graphSize_(0) {
	readLinkGraph(linkGraphName);
    graphSize_ = graph_.size();
}

/*
LinkGraph::LinkGraph(const LinkGraph& linkGraph) {
	graph_ = linkGraph.graph();
	graphSize_ = linkGraph.graphSize();
	mtRand_ = linkGraph.mtRand();
}

LinkGraph& LinkGraph::operator=(const LinkGraph& linkGraph) {
	graph_ = linkGraph.graph();
	graphSize_ = linkGraph.graphSize();
	mtRand_ = linkGraph.mtRand();
	return *this;
}
*/
//TODO: documentation for the graph-file format
//TODO: check linkGraph structure: every node has to contain
//always a loop

int LinkGraph::readLinkGraph(string linkGraphName){

	ifstream linkFile(linkGraphName.c_str());
	assert(linkFile);
	VI links;
	string line;
	uint32_t linkID;
	uint32_t docID;
	while(getline(linkFile, line)){
		links.clear();
		stringstream ss(line);
		ss >> docID;
		while(ss >> linkID){
			links.push_back(linkID);
		}
		pair<GraphType::iterator, bool> result = graph_.insert(make_pair(docID, links));
		assert(result.second);
	}
	return 0;

}

bool LinkGraph::graphCheck() {

	for(GraphType::iterator iter=graph_.begin();iter!=graph_.end(); ++iter){
		uint32_t docID = iter->first;
		VI::iterator inlinkIter = iter->second.begin();
		for(; inlinkIter != iter->second.end(); ++inlinkIter) {
			if(*inlinkIter==docID) break;
		}
		if(inlinkIter==iter->second.end()) return false;
	}
	return true;
}







