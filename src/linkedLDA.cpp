/*
 * linkedLDA.cpp
 *
 *  Created on: 2008.07.25.
 *      Author: ibiro
 */

#include "Model.h"

int main(int argc, char** argv){

	if (argc != 2) {
		fprintf(stderr,"usage: %s configFile\n", argv[0]);
		return 1;
	}
	string configFileName(argv[1]);
	//cerr << configFileName << endl;
	// calls ldaModel.init(), which calls ldaModel.initEst() or
	// ldaModel.initEstc() or ldaModel.initInf() based on the
	// value of modelStatus (from configFile)
	Model ldaModel(configFileName);

	// calls ldaModel.estimate() or ldaModel.estimateContinue()
	//or ldaModel.inference() based on the value of modelStatus
	ldaModel.run();
	cerr << "run done." << endl;
	return 0;
}
