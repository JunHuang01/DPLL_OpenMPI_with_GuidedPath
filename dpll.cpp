#include "dpll.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <stack>


#include "stdlib.h"
#include "time.h"
#include "limits.h"
#include "math.h"

#include "Constants.h"
#include "CommonTypes.h"


dpll::dpll(){
	m_iHighestC = 0;
	m_iConflicts = 0;
	m_iMAX_GPCount = 0;
}

dpll::dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes,int MAX_DEPTH_ALLOWED){
	m_SATSET = inputData;
	m_iMaxClause = iMaxClause;
	m_iMaxVarTypes = iMaxVarTypes;
	m_iHighestC = 0;
	m_iConflicts = 0;
	m_iMAX_GPCount = 0;
	m_iMAX_DEPTH_ALLOWED = MAX_DEPTH_ALLOWED;
}



dpll::~dpll(){

}

void dpll::Solve()
{
	clock_t startTime;
	double timeElapsed = 0;
	startTime  = clock();
	m_startTime = clock();
	std::vector<int> temp;

	for (int i = 0 ; i < m_iMaxVarTypes; i++){
		temp.push_back(UNASSIGNED);
	}

	SolSet leftSol(temp);
	SolSet rightSol(temp);

	//fprintf(stderr, "%d is size\n", leftSol.size() );

	int iVarToPick = pickVar(m_SATSET,temp);

	leftSol.at(iVarToPick) = 1;
	rightSol.at(iVarToPick) = 0;

	//printSolSet(leftSol);
	//printSolSet(rightSol);


	bool bSolved = runDPLL(leftSol,rightSol,m_SATSET,0);//|| runDPLL(rightSol,m_SATSET,0);
	timeElapsed = double(clock() - startTime)/CLOCKS_PER_SEC;
	fprintf(stdout, "Solve=%d\tTimeSpent=%f\tHighestC=%d\tConflicts=%d\tMaxGP=%d\n", 
		int(bSolved), timeElapsed, m_iHighestC, m_iConflicts, m_iMAX_GPCount);
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

void dpll::printSolSet(SolSet currSol)
{
	int iLen = currSol.size();

	for ( int i =0 ; i < iLen ; ++i)
	{
		fprintf(stderr, "%d ",  currSol.at(i));
	}
	fprintf(stderr, "\n");
}

bool dpll::evalTruthValue(int iVar, int currAssign)
{
	bool bResult = bool(currAssign);
	return iVar < 0 ? (!bResult) : bResult;
}


bool dpll::runDPLL(SolSet leftSol,SolSet rightSol, SATSET currClauses,int depth)
{
	SolSet currSol;
	WorkPool currWorkPool;

	int iCurrClauseCount;
	currWorkPool.push(new GuidedPath(rightSol,currClauses,depth));
	currWorkPool.push(new GuidedPath(leftSol,currClauses,depth));
	
	while(!currWorkPool.empty()){
		//fprintf(stderr, "%d is the size of work pool\n", currWorkPool.size() );
		bool bFoundConflict = false;
		GuidedPath* pCurrGuidedPath = currWorkPool.top();
		currWorkPool.pop();
		currSol = pCurrGuidedPath->currSol;
		currClauses =pCurrGuidedPath->currClauses;
		depth = pCurrGuidedPath->depth;
		//printSolSet(currSol);
		delete pCurrGuidedPath;
		pCurrGuidedPath = NULL;
		if (m_iMAX_DEPTH_ALLOWED > 0 &&depth == 11 /*m_iMAX_DEPTH_ALLOWED*/){
			m_iMAX_GPCount++;
			//fprintf(stderr, "The currDepth is %d and MAX_DEPTH is %d\n",depth,m_iMAX_DEPTH_ALLOWED );
			continue;
		}
		//fprintf(stderr, "%d depth %d is MAX_DEPTH\n", depth,m_iMAX_DEPTH_ALLOWED);
		//printSolSet(&currSol);
		//unit propagate
		//double timeElapsed = double(clock() - m_startTime)/CLOCKS_PER_SEC;

		//if (timeElapsed > MAX_TIME_LIMIT) return false;
		iCurrClauseCount = currClauses.size();
		//fprintf(stderr, "clauses count %d\n",iCurrClauseCount );
		for ( int i = 0; i < iCurrClauseCount; ++i)
		{
			SolSet currClause = currClauses[i];
			int jLen = currClause.size();
			
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
					currClause.erase(currClause.begin()+j);
					jLen--;
					j--;
				}
			}

			//check for contradiction
			if(jLen == 0)
			{
				m_iHighestC = std::max(m_iHighestC, m_iMaxClause- iCurrClauseCount);
				m_iConflicts++;
				bFoundConflict = true;
				break;
			}
		}

		if(bFoundConflict) continue;

		//if all clauses are consitent return true
		if (iCurrClauseCount == 0) 
		{
			m_iHighestC = std::max(m_iHighestC, m_iMaxClause- iCurrClauseCount);
			return true;
		}



		int iVarToPick = pickVar(currClauses,currSol);

		SolSet leftSol = currSol;
		SolSet rightSol = currSol;
		SATSET leftClauses = currClauses;
		SATSET rightClauses = currClauses;

		leftSol[iVarToPick] = 1;
		rightSol[iVarToPick] = 0;
		//choose the variable with the most apperance as next variable to search


		//Record highest Clause solved
		//return runDPLL(leftSol,currClauses,depth+1) || runDPLL(rightSol,currClauses,depth+1);
		currWorkPool.push(new GuidedPath(rightSol,rightClauses,depth+1));
		currWorkPool.push(new GuidedPath(leftSol,leftClauses,depth+1));
	}

	return false;
}

bool dpll::isAllVarAssigned(SolSet currSol){
	int iLen = currSol.size();
	for ( int i = 0 ; i < iLen ; i++){
		if (currSol.at(i) == UNASSIGNED) return false;
	}
	return true;
}

int dpll::pickVar(SATSET currClauses, SolSet currSol)
{
	std::map<int,int> varMap;
	int iLen = currClauses.size();

	//printf("\n\n\n\n");
	//printSolSet(currSol);

	//fprintf(stderr, "%d is the currSize\n",iLen );

	for ( int i = 0; i < iLen ; ++i)
	{
		SolSet currClause = currClauses.at(i);
		int jLen = currClause.size();
		for ( int j = 0; j < jLen ; ++j){
			int currVar = abs(currClause[j]) -1 ;
			if( currSol.at(currVar) == UNASSIGNED)
				varMap[currVar]++; //+= pow(2.0,float(jLen)) + 1;
		}
	}
	//fprintf(stderr, "%d is the size of the map\n", varMap.size());

	int iVarToPick = 0;
	int iCurrMaxCount = 0;
	for(auto& varCount : varMap){
		//fprintf(stderr, "Var:%d Count:%d\n",varCount.first, varCount.second );
		if (varCount.second > iCurrMaxCount){
			iVarToPick = varCount.first;
			iCurrMaxCount = varCount.second;
		}
	}
	//fprintf(stderr, "%d is the var to pick\n", iVarToPick);
	return iVarToPick;
}