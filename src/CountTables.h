/*
 * CountTables.h
 *
 *  Created on: 2008.09.25.
 *      Author: ibiro
 */

#ifndef COUNTTABLES_H_
#define COUNTTABLES_H_

#include "Corpus.h"
#include "Corpus-inl.h"
#include "LinkGraph.h"
#include "LinkGraph-inl.h"
#include "Utils.h"

class CountTables {
 public:
	 /*
	 CountTables(uint32_t topicSize, uint32_t vocabSize,
				 LinkGraph& linkGraph, Corpus& corpus);
	 */
	 CountTables(uint32_t topicSize, uint32_t vocabSize,
	 			 LinkGraph* linkGraph, Corpus* corpus);

	 /*
	 CountTables(uint32_t topicSize, uint32_t vocabSize,
	 			 LinkGraph* linkGraph, Corpus* corpus,
	 			 istream& nwFile, istream& nwSumFile,
	 			 istream& ndFile, istream& ndSumFile);
	 */
	 CountTables(uint32_t topicSize, uint32_t vocabSize,
				 LinkGraph* linkGraph, Corpus* corpus,
				 istream& topicLabelFile, istream& refDocLabelFile);

	 virtual ~CountTables();

	 void init(); //initilaize nw, nwSum, nd, ndSum, ns, nsSum, topicLabels_, refDocLabels_
	 void loadLabelFile(istream& labelFile, MIVI& labelData);
	 void fillCounters();
	 inline VVI& nw() {return nw_;}
	 inline VI& nwSum() {return nwSum_;}
	 inline MIVI& nd() {return nd_;}
	 inline MII& ndSum() {return ndSum_;}
	 inline MIMII& ns() {return ns_;}
	 inline MII& nsSum() {return nsSum_;}
	 inline MIVI& topicLabels() {return topicLabels_;}
	 inline MIVI& refDocLabels() {return refDocLabels_;}

	 //inline definitions can be found in CountTables-inl.h
	 void change_nw(uint32_t wordID, uint32_t topic, int32_t value);
	 void change_nd(uint32_t docID, uint32_t topic, int32_t value);
	 void change_ns(uint32_t docID, uint32_t inlinkID, int32_t value);
	 void change_topicLabels(uint32_t docID, uint32_t pos, uint32_t topicValue);
	 void change_topicLabels(uint32_t docID, VI& labelVector);
	 void change_refDocLabels(uint32_t docID, uint32_t pos, uint32_t refDocIDValue);
	 void change_refDocLabels(uint32_t docID, VI& labelVector);

	 //TODO:templatization
	 void write(ostream& out);
	 void write(VVI& data, ostream& out);
	 void write(VI& data, ostream& out);
	 void write(MIVI& data, ostream& out);
	 void write(MII& data, ostream& out);
	 void write(MIMII& data, ostream& out);

	 //TODO:repair slow solutions(string conversions, streams...) for reading.
	 void read(istream& in, VVI& data);
	 void read(istream& in, VI& data);
	 void read(istream& in, MIVI& data);
	 void read(istream& in, MII& data);
	 void read(istream& in, MIMII& data);

 private:

	 uint32_t topicSize_;
	 uint32_t vocabSize_;
	 LinkGraph* linkGraph_;
	 Corpus* corpus_;
	 //Corpus* testCorpus_;
	 // nw[i][j]: number of instances of word/term i assigned to topic j, size V x K
	 VVI nw_;
	 // nwsum[j]: \sum_{i=0}^{w-1} nw[i][j], size K
	 VI nwSum_;
	 // nd[i][j]: number of words in document i assigned to topic j,
	 // (including those which is mapped to a word-position in a neighbor of d) size M x K
	 MIVI nd_;
	 // ndsum[i]: \sum_{j=0}^{k-1} nd[i][j], size M
	 MII ndSum_;
	 // ns[i][j]: number of words in document i assigned to inLinkID j, size M x doc.indegree()
	 MIMII ns_;
	 // nsSum[i] = \sum_{j=0}^{doc.indegree()-1} ns[i][j], size M
	 MII nsSum_;
	 MIVI topicLabels_; // topic assignments for words
	 MIVI refDocLabels_; // reference docID assignments for words

};

#endif /* COUNTTABLES_H_ */
