/*
 * Utils.h
 *
 *  Created on: 2008.07.25.
 *      Author: ibiro
 */

#ifndef UTILS_H_
#define UTILS_H_

#define msg(a) cout << #a": " << a << endl
#define msgerr(a) cerr << #a": " << a << endl

//option for using hash_map instead of map
//#define __HASH__

//#define NDEBUG
//TODO:switch on NDEBUG

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

//for c++ random number generation
#include <cstdlib>
#include <ctime>

using namespace std;

typedef vector<string> VS;
typedef vector<double> VD;
typedef vector<uint32_t> VI;
typedef vector<VI> VVI;
typedef vector<VD> VVD;

#ifdef __HASH__

#include <ext/hash_map>
#include <ext/hash_set>

using __gnu_cxx::hash_map;
using __gnu_cxx::hashtable;
using __gnu_cxx::hash;
using __gnu_cxx::hash_set;

typedef hash_map<uint32_t, uint32_t> MII;
typedef hash_map<uint32_t, VI> MIVI;
typedef hash_map<uint32_t, MII> MIMII;
typedef hash_set<uint32_t> SI;

#endif

#ifndef __HASH__

typedef map<uint32_t, uint32_t> MII;
typedef map<uint32_t, VI> MIVI;
typedef map<uint32_t, MII> MIMII;
typedef set<uint32_t> SI;

#endif

/*
enum Status {	MODEL_STATUS_UNKNOWN,
				MODEL_STATUS_EST,
				MODEL_STATUS_ESTC,
				MODEL_STATUS_INF
			};
*/

static VS split(const string& s, const string& delim=" "){
	VS res;
	string t;
	for ( uint32_t i = 0 ; i != s.size() ; i++ ) {
		if ( delim.find( s[i] ) != string::npos ) {
			if ( !t.empty() ) {
				res.push_back( t );
	            t = "";
	        }
	    }
		else {
			t += s[i];
	    }
	}
	if ( !t.empty() ) {
		res.push_back(t);
	}
	return res;
}

static string int2String(uint32_t x){
	ostringstream ss;
	ss <<  x;
	return ss.str();
}


typedef pair<uint32_t, double> PID;
typedef vector<PID> VPID;

class PIDcompare {
public:
	bool operator()(const PID& x, const PID& y) const{
		return x.second > y.second;
	}
};

#endif /* UTILS_H_ */
