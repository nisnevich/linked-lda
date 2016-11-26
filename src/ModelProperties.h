/*
 * ModelProperties.h
 *
 *  Created on: 2008.09.28.
 *      Author: ibiro
 */

#ifndef MODELPROPERTIES_H_
#define MODELPROPERTIES_H_

//#include <map>
//#include <string>
//#include <iostream>
//#include <fstream>

#include <cfloat>
#include "Utils.h"
#include "ConfigFile.h"

typedef map<string, string> PropertyMap;
typedef map<string, double> ParameterMap;

class ModelProperties {
public:
	explicit ModelProperties(string configFileName);
	virtual ~ModelProperties();
	int parseConfigFile();
	void readParam(string var, string name, string defaultValue);
	void readParam(double var, string name, double defaultValue);
	int add(string name, string value);
	int add(string name, double value);
	string getParam(string name);
	double getValue(string name);
	void writeParams(ostream& out);

private:
	ConfigFile* configFile_;
	PropertyMap properties_;
	ParameterMap parameters_;
};

#endif /* MODELPROPERTIES_H_ */
