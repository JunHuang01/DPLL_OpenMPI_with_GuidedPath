#include <mpi.h>

#include "dpll.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <stack>


#include "stdlib.h"
#include <stdio.h>
#include "time.h"
#include "limits.h"
#include "math.h"

#include "Constants.h"
#include "CommonTypes.h"


dpll::dpll(){
	m_iHighestC = 0;
	m_iConflicts = 0;
	m_iMAX_GPCount = 0;
	m_iPreProcessLevel = NORMAL_PROCESSING;
}

dpll::dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes,int MAX_DEPTH_ALLOWED){
	m_SATSET = inputData;
	m_iMaxClause = iMaxClause;
	m_iMaxVarTypes = iMaxVarTypes;
	m_iHighestC = 0;
	m_iConflicts = 0;
	m_iMAX_GPCount = 0;
	m_iMAX_DEPTH_ALLOWED = MAX_DEPTH_ALLOWED;
	m_iPreProcessLevel = NORMAL_PROCESSING;
}

dpll::dpll(SATSET inputData,int iMaxClause,
 int iMaxVarTypes,int MAX_DEPTH_ALLOWED,
 int iProc, int nProc){
	m_SATSET = inputData;
	m_iMaxClause = iMaxClause;
	m_iMaxVarTypes = iMaxVarTypes;
	m_iHighestC = 0;
	m_iConflicts = 0;
	m_iMAX_GPCount = 0;
	m_iMAX_DEPTH_ALLOWED = MAX_DEPTH_ALLOWED;
	m_iPreProcessLevel = NORMAL_PROCESSING;
	m_iProc = iProc;
	m_nProc = nProc;
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
	fprintf(stderr, "Solve=%d\tTimeSpent=%f\tHighestC=%d\tConflicts=%d\tMaxGP=%d\n", 
		int(bSolved), timeElapsed, m_iHighestC, m_iConflicts, m_iMAX_GPCount);
}

void dpll::initMaster(){
	for ( int i = 0 ; i < m_nProc ; i++){
		WorkerActivityList.push_back(WORKER_INACTIVE);
	}

	MasterProduceInitialGP();
	LunchSlaves();
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

void dpll::LunchSlaves()
{

}

void dpll::SlaveInitialRecv(){

}

void dpll::MasterProduceInitialGP()
{
	m_iPreProcessLevel = MAX_PRE_PROCESS_LEVEL;
	Solve();
	m_MasterWorkPool = m_SlaveWorkPool;
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

	int iCurrClauseCount;
	m_SlaveWorkPool.push(GuidedPath(rightSol,currClauses,depth));
	m_SlaveWorkPool.push(GuidedPath(leftSol,currClauses,depth));
	
	while(!m_SlaveWorkPool.empty()){
		//fprintf(stderr, "%d is the size of work pool\n", m_SlaveWorkPool.size() );
		bool bFoundConflict = false;
		GuidedPath pCurrGuidedPath = m_SlaveWorkPool.top();
		m_SlaveWorkPool.pop();
		currSol = pCurrGuidedPath.currSol;
		currClauses =pCurrGuidedPath.currClauses;
		depth = pCurrGuidedPath.depth;
		//printSolSet(currSol);
		//delete pCurrGuidedPath;
		//pCurrGuidedPath = NULL;
		if (m_iMAX_DEPTH_ALLOWED > 0 &&depth == m_iMAX_DEPTH_ALLOWED){
			m_iMAX_GPCount++;
			//fprintf(stderr, "The currDepth is %d and MAX_DEPTH is %d\n",depth,m_iMAX_DEPTH_ALLOWED );
			continue;
		}

		if ( m_iPreProcessLevel == MAX_PRE_PROCESS_LEVEL && depth == MAX_PRE_PROCESS_LEVEL){
			m_iMAX_GPCount++;
			continue;
		}
		//fprintf(stderr, "%d depth %d is MAX_DEPTH\n", depth,m_iMAX_DEPTH_ALLOWED);
		//printSolSet(&currSol);
		//unit propagate
		double timeElapsed = double(clock() - m_startTime)/CLOCKS_PER_SEC;

		if (timeElapsed > MAX_TIME_LIMIT) return false;
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
		m_iHighestC = std::max(m_iHighestC, m_iMaxClause- iCurrClauseCount);
		if (iCurrClauseCount == 0) 
		{
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
		m_SlaveWorkPool.push( GuidedPath(rightSol,rightClauses,depth+1));
		m_SlaveWorkPool.push( GuidedPath(leftSol,leftClauses,depth+1));
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
	for(std::map<int,int>::iterator varCountIter = varMap.begin() ; varCountIter != varMap.end() ; ++varCountIter){
		//fprintf(stderr, "Var:%d Count:%d\n",varCount.first, varCount.second );
		if (varCountIter->second > iCurrMaxCount){
			iVarToPick = varCountIter->first;
			iCurrMaxCount = varCountIter->second;
		}
	}
	//fprintf(stderr, "%d is the var to pick\n", iVarToPick);
	return iVarToPick;
}