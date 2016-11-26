/*
 * ModelProperties-inl.h
 *
 *  Created on: 2008.09.29.
 *      Author: ibiro
 */

#ifndef MODELPROPERTIESINL_H_
#define MODELPROPERTIESINL_H_

#include "ModelProperties.h"

inline string ModelProperties::getParam(string name) {
	PropertyMap::iterator found = properties_.find(name);
	if(found != properties_.end()){
		return found->second;
	}
	else {
		return "";
	}
}

inline double ModelProperties::getValue(string name) {
	ParameterMap::iterator found = parameters_.find(name);
	if(found != parameters_.end()){
			return found->second;
		}
		else {
			return -DBL_MAX;
		}
}

inline void ModelProperties::readParam(string var, string name, string defaultValue){
	configFile_->readInto(var, name, defaultValue);
	add(name, var);
}

inline void ModelProperties::readParam(double var, string name, double defaultValue) {
	configFile_->readInto(var, name, defaultValue);
	add(name, var);
}

#endif /* MODELPROPERTIESINL_H_ */
