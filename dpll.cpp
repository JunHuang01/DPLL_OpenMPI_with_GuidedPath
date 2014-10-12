#include "dpll.h"

#include <vector>
#include <string>
#include <map>


#include "stdlib.h"
#include "time.h"
#include "limits.h"

#include "Constants.h"
#include "CommonTypes.h"


dpll::dpll(){
	m_iHighestC = 0;
}

dpll::dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes){
	m_SATSET = inputData;
	m_iMaxClause = iMaxClause;
	m_iMaxVarTypes = iMaxVarTypes;
	m_iHighestC = 0;
}



dpll::~dpll(){

}

void dpll::Solve()
{
	clock_t startTime;
	double timeElapsed = 0;
	startTime  = clock();
	
	SolSet leftSol = getNewSolSet();
	SolSet rightSol = getNewSolSet();

	int iVarToPick = pickVar(&m_SATSET,&leftSol);

	leftSol[iVarToPick-1] = ASSIGNTRUE;
	rightSol[iVarToPick-1] = ASSIGNFALSE;
	bool bSolved = runDPLL(leftSol,m_SATSET) || runDPLL(leftSol,m_SATSET);
	timeElapsed = double(clock() - startTime)/CLOCKS_PER_SEC;
	fprintf(stdout, "Solve=%d\tTimeSpent=%f\tHighestC=%d\n", 
		int(bSolved), timeElapsed, m_iHighestC);
}

//private

SolSet dpll::getNewSolSet()
{
	SolSet newSolSet;
	for (int i = 0 ; i < m_iMaxVarTypes; i++){
		newSolSet.push_back(UNASSIGNED);
	}

	return newSolSet;
}

void dpll::printSolSet(SolSet * currSol)
{
	int iLen = currSol->size();

	for ( int i =0 ; i < iLen ; ++i)
	{
		fprintf(stderr, "%d ",  currSol->at(i));
	}
	fprintf(stderr, "\n");
}

bool dpll::evalTruthValue(int iVar, int currAssign)
{
	bool bResult = bool(currAssign);
	return iVar < 0 ? (!bResult) : bResult;
}


bool dpll::runDPLL(SolSet currSol, SATSET currClauses)
{
	//unit propagate
	int iCurrClauseCount = currClauses.size();

	int bContradictionFound = false;
	for ( int i = 0; i < iCurrClauseCount; ++i)
	{
		SolSet currClause = currClauses[i];
		int jLen = currClause.size();
		int falseCount = 0;
		for ( int j = 0 ; j < jLen ; ++j){
			int currVar = currClause[j];
			int currAssign = currSol[abs(currVar)-1];

			//Variable not assigned continue 
			if (currAssign == UNASSIGNED)
				continue;

			//evaluate variable, if true then the clause is true, remove this from the list
			if (evalTruthValue(currVar,currAssign))
			{
				currClauses.erase(currClauses.begin()+i);
				iCurrClauseCount--;
				i--;
				break;
			}
			else
			{
				falseCount++;
			}
		}

		//check for contradiction
		if(jLen == falseCount)
		{
			bContradictionFound = true;
		}
	}

	m_iHighestC = std::max(m_iHighestC, m_iMaxClause- iCurrClauseCount);

	//if contradiction is found return false
	if (bContradictionFound) return false;

	//if all clauses are consitent return true
	if (iCurrClauseCount == 0) return true;

	//choose a variable
	int iVarToPick = pickVar(&m_SATSET,&currSol);

	SolSet leftSol = currSol;
	SolSet rightSol = currSol;

	leftSol[iVarToPick-1] = ASSIGNTRUE;
	rightSol[iVarToPick-1] = ASSIGNFALSE;
	//choose the variable with the most apperance as next variable to search


	//Record highest Clause solved
	return runDPLL(leftSol,currClauses) || runDPLL(leftSol,currClauses);
}

int dpll::pickVar(SATSET * currClauses, SolSet* currSol)
{
	std::map<int,int> varMap;
	int iLen = currClauses->size();

	for ( int i = 0; i < iLen ; ++i)
	{
		SolSet currClause = currClauses->at(i);
		int jLen = currClause.size();
		for ( int j = 0; j < jLen ; ++j){
			int currVar = abs(currClause[j]);
			if( currSol->at(currVar) == UNASSIGNED)
				varMap[currVar]++;
		}
	}

	int iVarToPick = -1;
	int iCurrMaxCount = 0;
	for(auto& varCount : varMap){
		if (varCount.second > iCurrMaxCount){
			iVarToPick = varCount.first;
			iCurrMaxCount = varCount.second;
		}
	}

	return iVarToPick;
}