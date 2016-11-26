/*
 * Distribution.cpp
 *
 *  Created on: 2008.10.02.
 *      Author: ibiro
 */

#include "MultinomialDistribution.h"

MultinomialDistribution::MultinomialDistribution(VI& counts) {

	size_ = counts.size();
	data_.resize(size_);
	uint32_t tmp = 0;
	for(uint32_t k = 1; k<counts.size(); ++k) {
			tmp += counts[k-1];
			data_[k] = tmp;
	}
}

MultinomialDistribution::~MultinomialDistribution() {
	// TODO Auto-generated destructor stub
}

uint32_t MultinomialDistribution::rand(){

	double u = mtRand_.randExc()*data_[size_-1];
	uint32_t result;
	for(result = 0; result<size_; ++result) {
		if(data_[result] > u){
			break;
		}
	}
	return result;
}


