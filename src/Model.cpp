/*
 * Model.cpp
 *
 *  Created on: 2008.07.25.
 *      Author: ibiro
 */

#include <stdlib.h>
#include "Model.h"

int Model::init(){

	string modelStatus = modelProperties->getParam("modelStatus");
	//only for checking the built-in rnd generator
	srand(time(0));

	if (modelStatus=="est"){
		cerr<<"init estimation\n";
		initEstimate();
	}
	else if(modelStatus=="estc"){
		initEstimateContinue();
	}
	else if(modelStatus=="inf"){
		cerr<<"init inference\n";
		initInfer();
	}
	else {
		cerr << "Please set the right task in configFile (modelStatus=est/estc/inf)\n";
		return 1;
	}
	return 0;
}

int Model::initEstimate(){

	/**
	* read training corpus, create vocabulary, write vocabulary to File.
	*/

	vocabulary = new Vocabulary();
	trainingCorpus = new Corpus(modelProperties->getParam("trainingCorpusFileName"),
						modelProperties->getParam("vocabFileName"),
						vocabulary);

	//trainingCorpus->write(cerr);

	trainingCorpusSize = trainingCorpus->datasetSize();
    vocabSize = vocabulary->vocabSize();

    //TODO: better solution for reading modelParameters
    topicSize = static_cast<int>(modelProperties->getValue("topicSize"));
    alpha = modelProperties->getValue("alpha");
    Kalpha = topicSize*alpha;
    beta = modelProperties->getValue("beta");
    Vbeta = vocabSize*beta;
    gamma = modelProperties->getValue("gamma");
    topWords = static_cast<int>(modelProperties->getValue("topWords"));
    tmpTopic.resize(topicSize);

    msgerr(trainingCorpusSize);
    msgerr(vocabSize);
    msgerr(topicSize);
    msgerr(alpha);
    msgerr(beta);
    msgerr(gamma);
    msgerr(topWords);
    //load linkgraph
    linkGraph = new LinkGraph(modelProperties->getParam("linkGraphFileName"));

    //check whether graph structure is valid:
    assert(linkGraph->graphCheck());

    //load trainigCorpusTables
    //TODO: when reference linkGraph was used problem arrised: non-assignment...
    countTables = new CountTables(	topicSize, vocabSize,
									linkGraph, trainingCorpus);

    //TODO:write an iterator for the Corpus class.
    for(Dataset::iterator docIter = trainingCorpus->dataset().begin();
		docIter != trainingCorpus->dataset().end(); ++docIter) {

    	uint32_t docID = docIter->first;
		Document currentDoc = docIter->second;
		uint32_t docLength = currentDoc.length();
		VI topicRow(docLength, 0);
		VI refDocRow(docLength, 0);
		for(uint32_t wordPos=0; wordPos<docLength; ++wordPos){
			uint32_t topic = mtRand.randInt(topicSize-1);  //range: [0,topicSize-1]
			uint32_t refDocID = linkGraph->getRandomInlinkID(docID);
			uint32_t wordID = currentDoc.getWord(wordPos);
			topicRow[wordPos] = topic;
			refDocRow[wordPos] = refDocID;
			countTables->change_nw(wordID, topic, 1);
			countTables->change_nd(refDocID, topic, 1);
			countTables->change_ns(docID, refDocID, 1);
		}
		countTables->change_topicLabels(docID, topicRow);
		countTables->change_refDocLabels(docID, refDocRow);
	}
    //check matrices' values
    //countTables->write(cerr);
	return 0;
}

int Model::initEstimateContinue(){
	return 0;
}


int Model::initInfer(){

	//read vocabulary then trainingCorpus

	vocabulary = new Vocabulary(modelProperties->getParam("vocabFileName"));
	//read trainingCorpus
	trainingCorpus = new Corpus(modelProperties->getParam("trainingCorpusFileName"), vocabulary);
	assert(trainingCorpus!=NULL);
	//trainingCorpus->write(cerr);

	//read testCorpus
	testCorpus = new Corpus(modelProperties->getParam("testCorpusFileName"), vocabulary);
	assert(testCorpus!=NULL);
	//testCorpus->write(cerr);
	//write out new vocabulary
	vocabulary->write(modelProperties->getParam("vocabFileName")+
					  modelProperties->getParam("infSuffix"));

	//essential model parameters
	trainingCorpusSize = trainingCorpus->datasetSize();
	testCorpusSize = testCorpus->datasetSize();
    vocabSize = vocabulary->vocabSize();
    topicSize = static_cast<int>(modelProperties->getValue("topicSize"));
    alpha = modelProperties->getValue("alpha");
    beta = modelProperties->getValue("beta");
    gamma = modelProperties->getValue("gamma");
    topWords = static_cast<int>(modelProperties->getValue("topWords"));
    msgerr(trainingCorpusSize);
    msgerr(testCorpusSize);
    msgerr(vocabSize);
    msgerr(topicSize);
    msgerr(alpha);
    msgerr(beta);
    msgerr(gamma);
    msgerr(topWords);
    //load fullLinkGraph : in test phase we use the full LinkGraph containing
    //the training and test document nodes
    //TODO: when reference linkGraph was used problem arrised: non-assignment...

    linkGraph = new LinkGraph(modelProperties->getParam("fullLinkGraphFileName"));

    //check whether graph structure is valid:
    assert(linkGraph->graphCheck());

    // we construct one countTable, this table contains the counts from the model (training phase)
    // and the random counts generated during this init() phase.



    string topicFileName = modelProperties->getParam("dir")+
						   modelProperties->getParam("modelName")+
						   modelProperties->getParam("topicAssignSuffix");

    string refDocFileName = modelProperties->getParam("dir")+
							modelProperties->getParam("modelName")+
							modelProperties->getParam("refDocAssignSuffix");

    msgerr(topicFileName);
    msgerr(refDocFileName);
    ifstream topicLabelFile(topicFileName.c_str());
    assert(topicLabelFile);
    ifstream refDocLabelFile(refDocFileName.c_str());
    assert(refDocLabelFile);
    countTables = new CountTables( topicSize, vocabSize, linkGraph,
								   trainingCorpus,
								   topicLabelFile, refDocLabelFile);

	//TODO:write an iterator for the Corpus class.
    for(Dataset::iterator docIter = testCorpus->dataset().begin();
		docIter != testCorpus->dataset().end(); ++docIter) {
    	uint32_t docID = docIter->first;
		Document currentDoc = docIter->second;
		uint32_t docLength = currentDoc.length();
		VI topicRow(docLength, 0);
		VI refDocRow(docLength, 0);
		for(uint32_t wordPos=0; wordPos<docLength; ++wordPos){
			//TODO: first choose refDocID randomly, then if this document is part of the
			//training corpus, use its topic distribution for choosing a topic.
			uint32_t refDocID = linkGraph->getRandomInlinkID(docID);
			uint32_t topic;

			if(trainingCorpus->isContained(refDocID)){
				MultinomialDistribution ml(countTables->nd()[refDocID]);
				topic = ml.rand();
				//msgerr(topic);
			}
			else {
				topic = mtRand.randInt(topicSize-1);  //range: [0,topicSize-1]
			}

			uint32_t wordID = currentDoc.getWord(wordPos);
			topicRow[wordPos] = topic;
			refDocRow[wordPos] = refDocID;
			countTables->change_nw(wordID, topic, 1);
			countTables->change_nd(refDocID, topic, 1);
			countTables->change_ns(docID, refDocID, 1);
		}
		countTables->change_topicLabels(docID, topicRow);
		countTables->change_refDocLabels(docID, refDocRow);
	}
    //check matrices' values
    //countTables->write(cerr);
	return 0;
}
/*

int Model::initSmoothedMatrices(){

	//initialize phi, theta, chi and ns, nsSum
	GraphType& graph = linkGraph->getGraph();
	chi.resize(corpusSize);
	assert(graph.size()==corpusSize);
	uint32_t idx = 0;
	for(GraphType::iterator iter=graph.begin(); iter!=graph.end(); ++iter){
		uint32_t inDegree = iter->second.size();
		VI nsRow(inDegree,0);
		ns[idx] = nsRow;
		VD chiRow(inDegree,0);
		chi[idx] = chiRow;
		idx++;
	}

	//theta initialization
	VD thetaRow(topicSize,0);
	theta.resize(corpusSize);
	for (uint32_t i = 0; i < corpusSize; ++i) {
		theta[i] = thetaRow;
	}
	//phi initialization
	VD phiRow(vocabSize,0);
	phi.resize(topicSize);
	for (uint32_t i = 0; i < topicSize; ++i) {
		phi[i] = phiRow;
	}
	return 0;
}
*/

/*

int Model::initInfSmoothedMatrices(){
	//initialize infPhi, infTheta, infChi and infns, infnsSum
	GraphType& graph = infLinkGraph->getGraph();
	infChi.resize(infCorpusSize);
	assert(graph.size()==infCorpusSize);
	uint32_t idx = 0;
	for(GraphType::iterator iter=graph.begin(); iter!=graph.end(); ++iter){
		VI infnsRow(iter->second.size(),0);
		infns[idx] = infnsRow;
		VD infChiRow(iter->second.size(),0);
		infChi[idx] = infChiRow;
		idx++;
	}

	//infTheta initialization
	VD infThetaRow(topicSize,0);
	infTheta.resize(infCorpusSize);
	for (uint32_t i = 0; i < infCorpusSize; ++i) {
		infTheta[i] = infThetaRow;
	}
	//infPhi initialization
	VD infPhiRow(vocabSize,0);
	infPhi.resize(topicSize);
	for (uint32_t i = 0; i < topicSize; ++i) {
		infPhi[i] = infPhiRow;
	}
	return 0;
}


*/

/*
int Model::initLabel(VVI& labelMatrix, uint32_t size){

	labelMatrix.resize(size);
	return 0;
}

*/

/*

int Model::initCounters(){
	//initialize nw, nwSum, nd, ndSum, ns, nsSum

	nw.resize(vocabSize);
	for(uint32_t i=0; i<vocabSize; ++i){
		VI row(topicSize,0);
		nw[i]=row;
	}

	//TODO:consistent matrix formulation: nw should be K x V, so as nwSum
	nwSum.resize(topicSize);
	for(uint32_t i=0; i<topicSize; ++i){
		nwSum[i]=0;
	}

	nd.resize(corpusSize);
	ndSum.resize(corpusSize);
	ns.resize(corpusSize); //it will be filled later
	nsSum.resize(corpusSize);
	for (uint32_t i = 0; i < corpusSize; ++i) {
		VI row(topicSize,0);
		nd[i] = row;
		ndSum[i] = 0;
		nsSum[i] = 0;
	}

	return 0;
}

*/

/*

int Model::initInfCounters(){
	//initialize infnw, infnwSum, infnd, infndSum, infns, infnsSum

	infnw.resize(vocabSize);
	for(uint32_t i=0; i<vocabSize; ++i){
		VI row(topicSize,0);
		infnw[i]=row;
	}
	//TODO:consistent matrix formulation: infnw should be K x V, so as infnwSum
	infnwSum.resize(topicSize);
	for(uint32_t i=0; i<topicSize; ++i){
		infnwSum[i]=0;
	}

	infnd.resize(corpusSize+infCorpusSize);
	infndSum.resize(corpusSize+infCorpusSize);

	for (uint32_t i = 0; i < corpusSize+infCorpusSize; ++i) {
		VI row(topicSize,0);
		infnd[i] = row;
		infndSum[i] = 0;
		//infnsSum[i] = 0;
	}

	infns.resize(infCorpusSize); //it will be filled later
	infnsSum.resize(infCorpusSize);

	for(uint32_t i = 0; i < infCorpusSize; ++i){
		infnsSum[i]=0;
	}
	return 0;

}
int Model::updateCounters(){

	//update values of nw, nwSum, nd, ndSum, ns, nsSum based on trainingCorpus

	for(uint32_t i=0; i<corpusSize; ++i){
		//uint32_t docLength = trainingCorpus->getDocumentByID(i).getLength();
		uint32_t docLength = (*trainingCorpus)[i].getLength();
		for(uint32_t j=0; j<docLength; ++j){
			//uint32_t w = trainingCorpus->getDocumentByID(i).getWord(j);
			uint32_t w = (*trainingCorpus)[i].getWord(j);
			uint32_t topic = topicLabels[i][j];
			uint32_t refDoc = refDocLabels[i][j];
			nw[w][topic] += 1;
			nwSum[topic] += 1;
			nd[refDoc][topic] += 1;
			ndSum[refDoc] += 1;
			ns[i][refDoc] += 1;
			nsSum[i] += 1;
		}

	}

	return 0;
}

*/


/*

int Model::loadLabelFile(	string labelFileName,
							VVI& labelMatrix){
	ifstream labelFile(labelFileName.c_str());
	assert(labelFile);
	labelMatrix.resize(trainingCorpusSize);
	string line;
	VI row;
	uint32_t docID = 0;
	string entry;
	while(getline(labelFile, line)){
		row.clear();
		stringstream ss(line);
		while(ss >> entry){
			VS items = split(entry,":");
			assert(items.size()==2);
			row.push_back(atoi(items[1].c_str()));
		}
		labelMatrix[docID++] = row;
	}
	assert(labelMatrix.size()==trainingCorpusSize);
	return 0;
}

*/

int Model::estimate(Corpus* corpus){

	//TODO: better implementation of model parameters
	uint32_t iterNumber = static_cast<uint32_t>(modelProperties->getValue("iterNumber"));
	uint32_t saveStep = static_cast<uint32_t>(modelProperties->getValue("saveSteps"));
	cerr << "Sampling " << iterNumber << " iterations!\n";
	for(uint32_t iter = 0; iter<iterNumber; ++iter){
		cerr << "Iteration " << iter << " ...\n";
		//uint32_t posNumber = 0;
		for(Dataset::iterator docIter = corpus->dataset().begin();
				docIter != corpus->dataset().end(); ++docIter) {

			uint32_t docID = docIter->first;
			//cerr << "docID: " << docID << "started...";
			Document currentDoc = docIter->second;
			uint32_t docLength = currentDoc.length();
			uint32_t inDegree = linkGraph->getIndegree(docID);

			//this modification will speed up the algorithm.
			VI& topicVector = countTables->topicLabels()[docID];
			VI& refDocVector = countTables->refDocLabels()[docID];

			for(uint32_t wordPos = 0; wordPos < docLength; ++wordPos){
				//if(posNumber++%10000==0) msgerr(posNumber);
				uint32_t prevTopic = topicVector[wordPos];
				uint32_t prevRefDoc = refDocVector[wordPos];
				uint32_t wordID = currentDoc.getWord(wordPos);
				//uint32_t topic = gibbsSampling(corpus, docID, wordPos);
				uint32_t newTopic = gibbsSampling(corpus, wordID, prevTopic, prevRefDoc);
				countTables->change_topicLabels(docID, wordPos, newTopic);
				//refDoc sampling iif doc has non-trivial inlink.
				if(inDegree == -12){
					uint32_t newRefDoc = gibbsSamplingRefDoc(docID, wordID, newTopic, prevRefDoc);
					countTables->change_refDocLabels(docID, wordPos, newRefDoc);
				}
			}
			//cerr << "done.\n";
		}
		if((iter+1)%saveStep==0){
			saveModel(iter+1);
		}
	}
	//countTables->write(cerr);
	//saveModel(iterNumber+1);
	return 0;
}

int Model::estimateContinue(){
	return 0;
}

int Model::infer(Corpus* corpus){

	estimate(corpus);
	/*
	cerr << "Sampling " << infIterNumber << " iterations!\n";
	for(uint32_t iter = 0; iter < infIterNumber; ++iter){
		cerr << "Iteration " << iter << " ...\n";
		for(uint32_t docID = 0; docID<infCorpusSize; ++docID){
			//uint32_t docLength = testCorpus->getDocumentByID(docID).getLength();
			uint32_t docLength = (*testCorpus)[docID].getLength();
			for(uint32_t n = 0; n<docLength; ++n){
				uint32_t topic = infGibbsSampling(docID, n);
				//z->at(docID).at(n) = topic;
				infTopicLabels[docID][n] = topic;
				uint32_t refDoc = infGibbsSamplingRefDoc(docID, n);
				//s->at(docID).at(n) = refDoc;
				infRefDocLabels[docID][n] = refDoc;
			}
		}
	}
	computeInfTheta();
	computeInfPhi();
	computeInfChi();
	saveInfModel();
	*/
	return 0;
}

/*

uint32_t Model::infGibbsSampling(uint32_t docID, uint32_t n){

	// remove z_i from the count variables
    uint32_t topic = infTopicLabels[docID][n];
    uint32_t refDoc = infRefDocLabels[docID][n];
    //uint32_t w = testCorpus->getDocumentByID(docID).getWord(n);
    uint32_t w = (*testCorpus)[docID].getWord(n);
    infnw[w][topic] -= 1;
    infnd[refDoc][topic] -= 1;
    infnwSum[topic] -= 1;
    infndSum[refDoc] -= 1;

    double Vbeta = vocabSize * beta;
	double Kalpha = topicSize * alpha;
	// do multinomial sampling via cumulative method
	VD p(topicSize);
	for (uint32_t k = 0; k < topicSize; k++) {
		p[k] = (infnw[w][k]+nw[w][k]+beta) / (infnwSum[k] + nwSum[k] + Vbeta) *
		(infnd[refDoc][k] + alpha) / (infndSum[refDoc] + Kalpha);
	}
	// cumulate multinomial parameters
	for (uint32_t k = 1; k < topicSize; k++) {
	  	p[k] += p[k - 1];
	}
	// scaled sample because of unnormalized p[]
	double u = mtRand.randExc() * p[topicSize - 1];

	for (topic = 0; topic < topicSize; topic++) {
	  	if (p[topic] > u) {
	 		break;
	  	}
	}

	// add newly estimated z_i to count variables
	infnw[w][topic] += 1;
	infnd[refDoc][topic] += 1;
	infnwSum[topic] += 1;
	infndSum[refDoc] += 1;
    return topic;
}

uint32_t Model::infGibbsSamplingRefDoc(uint32_t docID, uint32_t n){

	// remove refdoc_i from the count variables
    uint32_t refDoc = infRefDocLabels[docID][n];
    uint32_t topic = infTopicLabels[docID][n];
    infns[docID][refDoc] -= 1;
    infnsSum[docID] -= 1;
    infnd[refDoc][topic] -= 1;
    infndSum[refDoc] -= 1;

    //test purpose only
	//cerr << "docID: " << docID << "\tn: " << n << endl;
    //uint32_t refDoc = infRefDocLabels.at(docID)[n];
    //uint32_t topic = infTopicLabels.at(docID)[n];
    //cerr << "refDoc: " << refDoc <<"\ttopic: " << topic << endl;
    //infns.at(docID).at(refDoc) -= 1;
    //infnsSum.at(docID) -= 1;
    //infnd.at(refDoc).at(topic) -= 1;
    //infndSum.at(refDoc) -= 1;

    VI& refDocSet = infLinkGraph->getGraph()[docID];
    uint32_t inDegree = refDocSet.size();
    double Dgamma =  inDegree * gamma;
    double Kalpha = topicSize * alpha;

    //VI empCounts(inDegree);
    //copyCounts(empCounts, docID);
    /*
    SI& refDocSet = infLinkGraph->getGraph()[docID];
    uint32_t d = 0;
    for(SI::iterator iter=refDocSet.begin(); iter!=refDocSet.end(); ++iter, ++d){
    	uint32_t empCount = 0;
    	uint32_t empCountSum = 0;
    	if(*iter<corpusSize) {
    		empCount = ns[docID][*iter];
    		empCountSum = nsSum[*iter];
    	}
    	p[d] = (infns[docID][d] + empCount + gamma) / (infnsSum[docID] + empCountSum + Dgamma)*
    	    	(infnd[d][topic] + alpha) / (infndSum[d] + Kalpha);
    }
	*/

    // do multinomial sampling via cumulative method
/*
    VD p(inDegree);
    uint32_t d = 0;

    for(VI::iterator iter=refDocSet.begin(); iter!=refDocSet.end(); ++iter, ++d){
    //for (uint32_t d = 0; d < inDegree; d++) {
    	uint32_t empTheta = 0;
    	uint32_t empThetaSum = 0;
    	if(*iter<corpusSize){
    		empTheta = nd[*iter][topic];
    		empThetaSum = ndSum[*iter];
    	}
    	p[d] = (infns[docID][d] + gamma) / (infnsSum[docID] + Dgamma)*
    	(infnd[*iter][topic] + empTheta + alpha) / (infndSum[*iter] + empThetaSum + Kalpha);
    }


    // cumulate multinomial parameters
    for (uint32_t d = 1; d < inDegree; d++) {
    	p[d] += p[d - 1];
    }
    // scaled sample because of unnormalized p[]
    double u = mtRand.randExc() * p[inDegree - 1];
    for (refDoc = 0; refDoc < inDegree; refDoc++) {
    	if (p[refDoc] > u) {
    		break;
    	}
    }
	    // add newly estimated refdoc_i to count variables
    infns[docID][refDoc] += 1;
    infnsSum[docID] += 1;
    infnd[refDoc][topic] += 1;
    infndSum[refDoc] += 1;
    return refDoc;
}

*/

//TODO: faster implementation needed
/*
void Model::copyCounts(VI& countVector, uint32_t docID){

	VI& infNeighbors = infLinkGraph->getGraph()[docID];
	VI& neighbors = linkGraph->getGraph()[docID];
	VI commonIDs;
	assert(countVector.size()==infNeighbors.size());
	set_intersection(	infNeighbors.begin(),
						infNeighbors.end(),
						neighbors.begin(),
						neighbors.end(),
						insert_iterator<SI>(commonIDs, commonIDs.begin()));

	for(uint32_t i = 0; i<infNeighbors.size(); ++i){
			VI::iterator found = find(neighbors.begin(), neighbors.end(), infNeighbors[i]);
			countVector[i] = found!=neighbors.end() ? (*found) : 0;
	}

}
*/

//old function signature
//uint32_t Model::gibbsSampling(Corpus* corpus, uint32_t docID, uint32_t wordPos){

uint32_t Model::gibbsSampling(Corpus* corpus, uint32_t wordID, uint32_t topic, uint32_t refDoc){
	    // remove z_i from the count variables

		//these function calls moved to the estimate() function.

		//uint32_t topic = countTables->topicLabels()[docID][wordPos];
	    //uint32_t refDoc = countTables->refDocLabels()[docID][wordPos];
	    //uint32_t wordID = corpus->getDocumentByID(docID).getWord(wordPos);
	    //TODO: change function call to nw()
	    countTables->change_nw(wordID, topic, -1);
	    countTables->change_nd(refDoc, topic, -1);

	    VVI& nw = countTables->nw();
	    VI& nwSum = countTables->nwSum();
	    MIVI& nd = countTables->nd();
	    MII& ndSum = countTables->ndSum();

	    //decrease the counts of the respective cells. This solution is not good, because it can
	    //happen that the respective key does not exist. See change_nw, change_nd implementation.
	    //nw[wordID][topic]--;
	    //nwSum[wordID]--;
	    //nd[refDoc][topic]--;
	    //ndSum[refDoc]--;

	    //change these to member variables.

	    //double Vbeta = vocabSize * beta;
	    //double Kalpha = topicSize * alpha;
	    // do multinomial sampling via cumulative method
	    //TODO: draga???
	    //TODO: nd[refDoc], ndSum[refDoc] referenciak
	    VI& nw_wordID = nw[wordID];
	    VI& nd_refDoc = nd[refDoc];

		tmpTopic.resize(topicSize);
	    for (uint32_t k = 0; k < topicSize; k++) {
	    	tmpTopic[k] = (nw_wordID[k] + beta) / (nwSum[k] + Vbeta) * (nd_refDoc[k] + alpha) / (ndSum[k] + Kalpha);
	    }
	    // cumulate multinomial parameters
	    for (uint32_t k = 1; k < topicSize; k++) {
	    	tmpTopic[k] += tmpTopic[k - 1];
	    }
	    // scaled sample because of unnormalized p[]

	    double u = mtRand.randExc() * tmpTopic[topicSize - 1];
	    //only testing the built-in rnd generator: according to test results
	    // mersene twister is twice faster than built-in c++ rand generator.
	    //double u = static_cast<double>(rand())/static_cast<double>( RAND_MAX )*tmpTopic[topicSize - 1];
	    //TODO:handle the case, when topicSize==1
	    //log-kereses a topicra.
	    /*
	    for (topic = 0; topic < topicSize; topic++) {
	    	if (tmpTopic[topic] > u) {
	    		break;
	    	}
	    }
	    */
	    //changed to stl::upper_bound, which uses bin-search.
	    topic = upper_bound(tmpTopic.begin(), tmpTopic.end(), u)-tmpTopic.begin();
	    // add newly estimated z_i to count variables
	    countTables->change_nw(wordID, topic, 1);
	    countTables->change_nd(refDoc, topic, 1);

	    //see above, why it is not good.
	    //nw[wordID][topic]++;
		//nwSum[wordID]++;
	    //nd[refDoc][topic]++;
	    //ndSum[refDoc]++;
	    return topic;
}
//old function signature
//uint32_t Model::gibbsSamplingRefDoc(uint32_t docID, uint32_t wordPos){

uint32_t Model::gibbsSamplingRefDoc(uint32_t docID, uint32_t wordID, uint32_t topic, uint32_t refDoc){

	//TODO: improvement should be done as in gibbsSampling.

	//cerr << "docID: " << docID << "wordPos: " << wordPos << endl;
	// remove refdoc_i from the count variables
    //uint32_t refDoc = refDocLabels[docID][wordPos];
    //uint32_t topic = topicLabels[docID][wordPos];

	//uint32_t topic = countTables->topicLabels()[docID][wordPos];
	//uint32_t refDoc = countTables->refDocLabels()[docID][wordPos];

    countTables->change_ns(docID, refDoc, -1);
    countTables->change_nd(refDoc, topic, -1);

    MIVI& nd = countTables->nd();
    MII& ndSum = countTables->ndSum();
    MIMII& ns = countTables->ns();
    MII& nsSum = countTables->nsSum();

    //nd[refDoc][topic]--;
    //ndSum[refDoc]--;
    //ns[docID][refDoc]--;
    //nsSum[docID]--;

    //test purpose only
	//cerr << "docID: " << docID << "\tn: " << n << endl;
    //uint32_t refDoc = s.at(docID)[n];
    //uint32_t topic = z.at(docID)[n];
    //cerr << "refDoc: " << refDoc <<"\ttopic: " << topic << endl;
    //ns.at(docID).at(refDoc) -= 1;
    //nsSum.at(docID) -= 1;
    //nd.at(refDoc).at(topic) -= 1;
    //ndSum.at(refDoc) -= 1;

    VI& refDocSet = linkGraph->getLinks(docID);
    uint32_t inDegree = refDocSet.size();
    double Dgamma =  inDegree * gamma;
    //double Kalpha = topicSize * alpha;
    // do multinomial sampling via cumulative method

    //VD p(inDegree);
    tmpRefDoc.clear();
    tmpRefDoc.resize(inDegree);
    uint32_t d = 0;
    for(VI::iterator iter=refDocSet.begin(); iter!=refDocSet.end(); ++iter, ++d){
    	tmpRefDoc[d] = (ns[docID][d] + gamma) / (nsSum[docID] + Dgamma)*
    	(nd[*iter][topic] + alpha) / (ndSum[*iter] + Kalpha);
    }
    // cumulate multinomial parameters
    for (uint32_t d = 1; d < inDegree; d++) {
    	tmpRefDoc[d] += tmpRefDoc[d - 1];
    }
    // scaled sample because of unnormalized p[]
    double u = mtRand.randExc() * tmpRefDoc[inDegree - 1];
    uint32_t refDocIdx = upper_bound(tmpRefDoc.begin(), tmpRefDoc.end(), u)-tmpRefDoc.begin();
    /*
    for (refDocIdx = 0; refDocIdx < inDegree; ++refDocIdx) {
    	if (p[refDocIdx] > u) {
    		break;
    	}
    }
    */

    //msgerr(inDegree);
    //msgerr(refDocIdx);
    refDoc = refDocSet[refDocIdx];
    // add newly estimated refdoc_i to count variables
    countTables->change_ns(docID, refDoc, 1);
    countTables->change_nd(refDoc, topic, 1);

    //nd[refDoc][topic]++;
    //ndSum[refDoc]++;
    //ns[docID][refDoc]++;
    //nsSum[docID]++;

    return refDoc;

}

/*

void Model::computeSmoothedMatrix(VVD& matrix, const VVI& empCounts,
								  const VI& empCountsSums, double smoothingFactor){

	//test matrix sizes
	assert(matrix.size()==empCounts.size());
	uint32_t checkIdx = mtRand.randInt(matrix.size()-1);
	assert(matrix[checkIdx].size()==empCounts[checkIdx].size());

	for (uint32_t m = 0; m < matrix.size(); m++) {
		uint32_t colLength = matrix[m].size();
		for (uint32_t k = 0; k < colLength; k++) {
		    matrix[m][k] = (empCounts[m][k] + smoothingFactor) / (empCountsSums[m] + colLength * smoothingFactor);
		}
	}
}

*/

void Model::computeAndSaveTheta(string baseName){

	string thetaFileName = baseName + modelProperties->getParam("thetaSuffix");
	cerr << "theta matrix saved to: " << thetaFileName<< endl;
	/*
	string countThetaFileName = modelProperties->getParam("dir")+
							    modelProperties->getParam("modelName")+".nd";

	string countSumThetaFileName = modelProperties->getParam("dir")+
							    modelProperties->getParam("modelName")+".ndSum";

	*/
	ofstream thetaFile(thetaFileName.c_str());
	//ofstream countThetaFile(countThetaFileName.c_str());
	//ofstream countSumThetaFile(countSumThetaFileName.c_str());
	MIVI& nd = countTables->nd();
	MII& ndSum = countTables->ndSum();
	MII::iterator docSumIter = ndSum.begin();
	for(MIVI::iterator docIter = nd.begin(); docIter != nd.end();
		++docIter,++docSumIter) {
		thetaFile << docIter->first << " ";
		for(VI::iterator iter = docIter->second.begin(); iter!=docIter->second.end(); ++iter) {
				thetaFile << (*iter + alpha)/(docSumIter->second + topicSize*alpha) << " ";
			}
		thetaFile << endl;
	}
	//countTables->write(nd, countThetaFile);
	//countTables->write(ndSum, countSumThetaFile);
}

void Model::computeAndSavePhi(string baseName){

	string phiFileName = baseName + modelProperties->getParam("phiSuffix");
	cerr << "phi matrix saved to: " << phiFileName<< endl;
	/*
	string countPhiFileName = modelProperties->getParam("dir")+
							  modelProperties->getParam("modelName")+".nw";

	string countSumPhiFileName = modelProperties->getParam("dir")+
								 modelProperties->getParam("modelName")+".nwSum";

	*/

	ofstream phiFile(phiFileName.c_str());
	//ofstream countPhiFile(countPhiFileName.c_str());
	//ofstream countSumPhiFile(countSumPhiFileName.c_str());
	VVI& nw = countTables->nw();
	VI& nwSum = countTables->nwSum();
	assert(nw.size() == vocabSize);
	assert(nwSum.size() == topicSize);
	for (uint32_t k = 0; k < topicSize; k++) {
		for (uint32_t w = 0; w < vocabSize; w++) {
			//TODO:change to []
			//phiFile << (nw[w][k] + beta) / (nwSum[k] + vocabSize * beta) << " ";
			phiFile << (nw.at(w).at(k) + beta)/(nwSum.at(k) + vocabSize * beta) << " ";
		}
		phiFile << endl;
    }
	//countTables->write(nw, countPhiFile);
	//countTables->write(nwSum, countSumPhiFile);
}
void Model::computeAndSaveChi(string baseName){

	string chiFileName = baseName + modelProperties->getParam("chiSuffix");
	cerr << "chi matrix saved to: " << chiFileName<< endl;
	/*
	string countChiFileName = modelProperties->getParam("dir")+
							  modelProperties->getParam("modelName")+".ns";

	string countSumChiFileName = modelProperties->getParam("dir")+
							  modelProperties->getParam("modelName")+".nsSum";
	*/
	ofstream chiFile(chiFileName.c_str());
	//ofstream countChiFile(countChiFileName.c_str());
	//ofstream countSumChiFile(countSumChiFileName.c_str());
	MIMII& ns = countTables->ns();
	MII& nsSum = countTables->nsSum();
	MII::iterator nsSumIter = nsSum.begin();
	for(MIMII::iterator nsIter = ns.begin(); nsIter!=ns.end();
		++nsIter, ++nsSumIter) {
		chiFile << nsIter->first << " ";
		for(MII::iterator inlinkIter = nsIter->second.begin();
			inlinkIter!=nsIter->second.end(); ++inlinkIter) {
			uint32_t inDegree = nsIter->second.size();
			chiFile << (inlinkIter->second+gamma)/(nsSumIter->second+inDegree*gamma) << " ";
		}
		chiFile << endl;
	}
	//countTables->write(ns, countChiFile);
	//countTables->write(nsSum, countSumChiFile);
}

/*
int Model::initInfLabels(){

	infTopicLabels.resize(corpusSize);
	infRefDocLabels.resize(corpusSize);
	return 0;
}
*/
//TODO:use this function to load the trainingCorpus without using the vocabulary

int Model::saveModel(uint32_t iterNumber){

	saveLabels();
	string modelStatus = modelProperties->getParam("modelStatus");
	string baseName;
	string iterNumberString = int2String(iterNumber);
	if(modelStatus=="est" || modelStatus=="estc"){
		baseName = modelProperties->getParam("dir")+
				   modelProperties->getParam("modelName") + "-" + iterNumberString;
		//saveDoubleMatrix(dir + modelName + phiSuffix, phi);
		//saveDoubleMatrix(dir + modelName + thetaSuffix, theta);
		//saveDoubleMatrix(dir + modelName + chiSuffix, chi);

	}else if(modelStatus=="inf"){
		baseName = modelProperties->getParam("dir")+
				   modelProperties->getParam("modelName")+
				   modelProperties->getParam("infSuffix");
		//saveDoubleMatrix(dir + modelName + infSuffix + phiSuffix, infPhi);
		//saveDoubleMatrix(dir + modelName + infSuffix + thetaSuffix, infTheta);
		//saveDoubleMatrix(dir + modelName + infSuffix + chiSuffix, infChi);
	}else {
		cerr << "modelStatus unknown.\n";
		exit(1);
	}
	computeAndSavePhi(baseName);
	computeAndSaveTheta(baseName);
	computeAndSaveChi(baseName);
	if (topWords > 0) {
		saveTopWords(baseName);
	}
	return 0;
}

int Model::saveLabels() {

	string topicFile;
	string refDocFile;

	string modelStatus = modelProperties->getParam("modelStatus");

	if(modelStatus == "est" || modelStatus == "estc"){
		topicFile = modelProperties->getParam("dir")+
					modelProperties->getParam("modelName")+
					modelProperties->getParam("topicAssignSuffix");

		refDocFile = modelProperties->getParam("dir")+
					 modelProperties->getParam("modelName")+
					 modelProperties->getParam("refDocAssignSuffix");
	}
	else if(modelStatus == "inf"){
		topicFile = modelProperties->getParam("dir")+
					modelProperties->getParam("modelName")+
					modelProperties->getParam("infSuffix")+
					modelProperties->getParam("topicAssignSuffix");

	    refDocFile = modelProperties->getParam("dir")+
					 modelProperties->getParam("modelName")+
					 modelProperties->getParam("infSuffix")+
					 modelProperties->getParam("refDocAssignSuffix");

	}
	else {
		cerr << "modelStatus unknown.\n";
	    exit(1);
    }
	ofstream topicStream(topicFile.c_str());
    assert(topicStream);
    ofstream refDocStream(refDocFile.c_str());
    assert(refDocStream);
    countTables->write(countTables->topicLabels(), topicStream);
    countTables->write(countTables->refDocLabels(), refDocStream);
    return 0;
}

/*
int Model::saveDoubleMatrix(VVD& matrix, string fileName){

	ofstream file(fileName.c_str());
	assert(file);
	for (uint32_t i = 0; i < matrix.size(); ++i) {
		for (uint32_t j = 0; j < matrix[i].size(); ++j) {
			file << matrix[i][j] << " ";
		}
		file << endl;
	}
	return 0;
}
*/

int Model::saveTopWords(string baseName) {

	string topWordFileName = baseName + modelProperties->getParam("topWordsSuffix");

	ofstream topWordFile(topWordFileName.c_str());
	assert(topWordFile);

    if (topWords > vocabSize) {
    	topWords = vocabSize;
    }

    VVI nw = countTables->nw();
    VI nwSum = countTables->nwSum();
   	PIDcompare comp;
    for (uint32_t k = 0; k < topicSize; k++) {
    	VPID wordsProbs(vocabSize);
    	PID wordProb;
    	for (uint32_t w = 0; w < vocabSize; w++) {
    		wordProb.first = w;
    		//TODO:change to []
    		wordProb.second = (nw.at(w).at(k) + beta)/(nwSum.at(k) + vocabSize * beta);
    		wordsProbs[w] = wordProb;
    	}

    	sort(wordsProbs.begin(), wordsProbs.end(), comp);

    	topWordFile << "Topic " << k << "th:\n";
    	for (uint32_t i = 0; i < topWords; i++) {
    		string word = vocabulary->getWord(wordsProbs[i].first);
    		if (word != "__UNKNOWN__") {
				topWordFile << "\t" << word << "\t" << wordsProbs[i].second << endl;
    		}
    	}
    }
    return 0;
}

void Model::run(){

	string modelStatus = modelProperties->getParam("modelStatus");
	if (modelStatus=="est"){
		estimate(trainingCorpus);
	}
	else if(modelStatus=="estc"){
		estimateContinue();
	}
	else if(modelStatus=="inf"){
		infer(testCorpus);
	}
	else {
		cerr << "Please set the right task in configFile (modelStatus=est/estc/inf)\n";
	}
}

/*

void Model::computeInfTheta(){
	for (uint32_t m = 0; m < infCorpusSize; m++) {
		for (uint32_t k = 0; k < topicSize; k++) {
		    infTheta[m][k] = (infnd[m][k] + alpha) / (infndSum[m] + topicSize * alpha);
		}
	}
}

void Model::computeInfPhi(){
	for (uint32_t k = 0; k < topicSize; k++) {
		for (uint32_t w = 0; w < vocabSize; w++) {
		    infPhi[k][w] = (infnw[w][k] + beta) / (infnwSum[k] + vocabSize * beta);
		}
	}
}

void Model::computeInfChi(){
	for (uint32_t m = 0; m < infChi.size(); ++m) {
		uint32_t inDegree = infChi[m].size();
		for (uint32_t r = 0; r < inDegree; ++r) {
			infChi[m][r] = (infns[m][r]+gamma)/(infnsSum[m]+inDegree*gamma);
		}
	}
}


int Model::saveInfModel(){
	saveModel();
	return 0;
}


bool Model::checkDisjunctness(){

	VI commonIDs;
	VI trainIDs;
	VI testIDs;
	for(Dataset::iterator iter=trainingCorpus->begin();
		iter!=trainingCorpus->end();++iter){
		trainIDs.push_back(iter->first);
	}
	for(Dataset::iterator iter=testCorpus->begin();
		iter!=testCorpus->end();++iter){
		testIDs.push_back(iter->first);
		}
	set_intersection(	trainIDs.begin(),
						trainIDs.end(),
						testIDs.begin(),
						testIDs.end(),
						insert_iterator<VI>(commonIDs, commonIDs.begin()));
	return commonIDs.empty();
}

*/
