#ifndef DPLL_H
#define DPLL_H


#include <vector>
#include <string>

#include "Constants.h"
#include "CommonTypes.h"


class dpll{
public:
	dpll();
	dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes);
	~dpll();


	void Solve();

private:
	SATSET m_SATSET;
	int m_iMaxClause;
	int m_iMaxVarTypes;
	int m_iHighestC;

	SolSet getNewSolSet();
	bool runDPLL(SolSet currSol, SATSET currClauses);
	bool evalTruthValue(int iVar, int currAssign);

	void printSolSet(SolSet *currSol);
	int pickVar(SATSET * currClauses,SolSet* currSol);
};



#endif