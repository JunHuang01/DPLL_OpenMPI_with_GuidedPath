#ifndef DPLL_H
#define DPLL_H


#include <vector>
#include <string>
#include <stack>

#include "Constants.h"
#include "CommonTypes.h"

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

typedef std::stack<GuidedPath*> WorkPool;

class dpll{
public:
	dpll();
	dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes, int MAX_DEPTH_ALLOWED);
	~dpll();


	void Solve();

private:
	SATSET m_SATSET;
	int m_iMaxClause;
	int m_iMaxVarTypes;
	int m_iHighestC;
	int m_iMAX_DEPTH_ALLOWED;
	int m_iConflicts;
	int m_iMAX_GPCount;
	clock_t m_startTime;

	SolSet getNewSolSet();
	bool isAllVarAssigned(SolSet currSol);
	bool runDPLL(SolSet leftSol, SolSet rightSol, SATSET currClauses,int depth);
	bool evalTruthValue(int iVar, int currAssign);

	void printSolSet(SolSet currSol);
	int pickVar(SATSET currClauses,SolSet currSol);

};



#endif