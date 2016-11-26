/*
 * Model.h
 *
 *  Created on: 2008.07.25.
 *      Author: ibiro
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "CountTables.h"
#include "CountTables-inl.h"
#include "ModelProperties.h"
#include "ModelProperties-inl.h"
#include "MultinomialDistribution.h"


//TODO:change to hash_map if the algorithm is slow.

class Model {

public:
	Model(string configFileName)
		: trainingCorpusSize(0),
		  vocabSize(0),
		  topicSize(0) {

		modelProperties = new ModelProperties(configFileName);
		modelProperties->writeParams(cerr);
		init();
	}
	~Model(){}
	int init();
	//int initCounters();
	//int initInfCounters();
	//int initLabel(VVI& labelMatrix, uint32_t size);
	int initEstimate();
	int initEstimateContinue();
	int initInfer();
	//int initSmoothedMatrices();
	//int initInfSmoothedMatrices();
	//int updateCounters();
	int estimate(Corpus* corpus);
	int estimateContinue();
	int infer(Corpus* corpus);
	//uint32_t gibbsSampling(Corpus* corpus, uint32_t docID, uint32_t wordPos);
	uint32_t gibbsSampling(Corpus* corpus, uint32_t wordID, uint32_t topic, uint32_t refDoc);
	//uint32_t gibbsSamplingRefDoc(uint32_t docID, uint32_t wordPos);
	uint32_t gibbsSamplingRefDoc(uint32_t docID, uint32_t wordID, uint32_t topic, uint32_t refDoc);
	//valoszinuleg meg lehet oldani, hogy csak ugyanaz legyen a gibbs sampling, mint a model-inference-ben
	//a new_nw bevezetese (Phan kodja) felesleges hiszen lehet a countokat egy nw-ben tarolni.

	//uint32_t infGibbsSampling(uint32_t, uint32_t);
	//uint32_t infGibbsSamplingRefDoc(uint32_t, uint32_t);
	//void copyCounts(VI& cVector, uint32_t docID);
	//void computeSmoothedMatrix(	VVD& matrix,
	//							const VVI& empCounts,
	//							const VI& empCountsSums,
	//							double smoothingFactor);
	//TODO:better implementation of compute and save methods
	void computeAndSaveTheta(string baseName);
	void computeAndSavePhi(string baseName);
	void computeAndSaveChi(string baseName);
	//void computeInfTheta();
	//void computeInfPhi();
	//void computeInfChi();
	int saveModel(uint32_t saveStep);
	//int saveInfModel();
	int saveLabels();
	int loadLabelFile(string labelFileName, VVI& labelMatrix);
	//int saveTopicAssign(string fileName);
	int saveDoubleMatrix(VVD& matrix, string fileName);
	int saveTopWords(string baseName);
	//bool checkDisjunctness();
	//void checkMatrices();
	void run();

private:

	ModelProperties* modelProperties;
	Vocabulary* vocabulary;
	MTRand mtRand;

	//data-structures for model-inference (training phase)
	Corpus* trainingCorpus;
	LinkGraph* linkGraph;
	CountTables* countTables;

	//egyelore ugy nez ki hogy erre nem lesz szukseg
	//data-structures for unseen-inference (test phase)
	Corpus* testCorpus;
 	//LinkGraph* infLinkGraph;
	//CountTables* testCountTables;

	//model parameters and variables

	uint32_t trainingCorpusSize; //M: number of docs in the trainingCorpus
	uint32_t testCorpusSize; //M_t: number of docs in the testCorpus
	uint32_t vocabSize; //V: size of the vocabulary
	uint32_t topicSize; //K: number of topics
	double alpha, beta, gamma; //Dirichlet hyperparameters
	uint32_t topWords; 	// number of words with highest probability in a
						// given topic (used only in saveTopWords())
	VD tmpTopic;
	VD tmpRefDoc;

	double Vbeta;
	double Kalpha;

};

#endif /* MODEL_H_ */
