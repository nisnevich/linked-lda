/*
 * Distribution.h
 *
 *  Created on: 2008.10.02.
 *      Author: ibiro
 */

#ifndef DISTRIBUTION_H_
#define DISTRIBUTION_H_

#include "Utils.h"
#include "MersenneTwister.h"

class MultinomialDistribution {

public:
	MultinomialDistribution(VI& counts);
	virtual ~MultinomialDistribution();
	uint32_t rand();

private:
	VI data_;
	uint32_t size_;
	MTRand mtRand_;
};

#endif /* DISTRIBUTION_H_ */
