#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <memory>
#include <vector>
#include <string>
#include <stack>


struct GuidedPath{
	SolSet currSol;
	SATSET currClauses;
	int depth;
	GuidedPath(SolSet currSol, SATSET currClauses, int depth){
		this->currSol = currSol;
		this->currClauses = currClauses;
		this->depth = depth;
	}
};

class SATVariable;

typedef std::string STRING;
typedef std::vector<int> Clause;
typedef std::vector<int> SolSet;
typedef std::vector<Clause> SATSET;

typedef std::stack<GuidedPath> WorkPool;
#endif