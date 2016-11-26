/*
 * ModelProperties.cpp
 *
 *  Created on: 2008.09.28.
 *      Author: ibiro
 */

#include "ModelProperties.h"
#include "ModelProperties-inl.h"

ModelProperties::ModelProperties(string configFileName) {
	configFile_ = new ConfigFile(configFileName);
	parseConfigFile();
}

ModelProperties::~ModelProperties() {
	// TODO Auto-generated destructor stub
}

int ModelProperties::parseConfigFile() {

	//TODO:write out all of the parameters read.
	string propertyName;

	//read common names, file extensions
	readParam(propertyName, "modelStatus", "");	// model status:
										// "": unknown status
										// "est": estimating from scratch
										// "estc": continue to estimate the model from a previous one
										// "inf": do inference
	readParam(propertyName, "dir", "./");  		// model directory
	readParam(propertyName, "trainingCorpusFileName", "trainingCorpus.dat");
	readParam(propertyName, "testCorpusFileName", "testCorpus.dat"); //if inference is selected, this corpus is needed
	readParam(propertyName, "linkGraphFileName", "linkGraph.dat"); //linkGraph of the trainingCorpus
	readParam(propertyName, "fullLinkGraphFileName", "fullLinkGraph.dat");
	readParam(propertyName, "vocabFileName", "vocab.txt"); // file that contains the vocabulary
	readParam(propertyName, "modelName", "model-final"); // model name, meg nem tom mire fog kelleni


	//read file extensions
	readParam(propertyName, "thetaSuffix", ".theta");
	readParam(propertyName, "phiSuffix", ".phi");
	readParam(propertyName, "chiSuffix", ".chi");
	readParam(propertyName, "topicAssignSuffix", ".tassign");
	readParam(propertyName, "refDocAssignSuffix", ".refdoc");
	readParam(propertyName, "topWordsSuffix", ".twords");
	readParam(propertyName, "infSuffix", ".inf");


	//TODO: default values for alpha, beta, gamma should be read as double.
	//read model parameters
	double paramValue = 0.0;
	readParam(paramValue, "topicSize", 100.0);
	readParam(paramValue, "alpha", 0.5);
	readParam(paramValue, "beta", 0.01);
	readParam(paramValue, "gamma", 0.1);
	readParam(paramValue, "iterNumber", 2000.0); //number of Gibbs sampling iterations
	readParam(paramValue, "saveSteps", 200.0); //after saveSteps iteration the full model will be saved
	readParam(paramValue, "topWords", 0.0); //print out top words per each topic
	readParam(paramValue, "infIter", 0.0);
	return 0;
}

int ModelProperties::add(string name, string value) {
	PropertyMap::iterator found = properties_.find(name);
	if(found == properties_.end()){
		pair<PropertyMap::iterator, bool> result = properties_.insert(make_pair(name, value));
		assert(result.second);
	}
	else {
		cerr << "Parameter already added to propertyMap!\n";
	}
	return 0;
}

int ModelProperties::add(string name, double value) {
	ParameterMap::iterator found = parameters_.find(name);
	if(found == parameters_.end()){
		pair<ParameterMap::iterator, bool> result = parameters_.insert(make_pair(name, value));
		assert(result.second);
	}
	else {
		cerr << "Parameter already added to parameterMap!\n";
	}
	return 0;
}


void ModelProperties::writeParams(ostream& out) {
	for(PropertyMap::iterator iter=properties_.begin(); iter!=properties_.end(); ++iter) {
		out << iter->first << " = " << iter->second << endl;
	}
}




