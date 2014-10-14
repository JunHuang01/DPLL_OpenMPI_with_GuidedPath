#ifndef DPLL_H
#define DPLL_H

#include <mpi.h>
#include <vector>
#include <string>
#include <stack>

#include "Constants.h"
#include "CommonTypes.h"

struct GuidedPath{
	SATSET currClauses;
	SolSet currSol;
	int depth;
	GuidedPath(SolSet currSol, SATSET currClauses, int depth){
		this->currSol = currSol;
		this->currClauses = currClauses;
		this->depth = depth;
	}

	GuidedPath(){

	}

};



typedef std::stack<GuidedPath> WorkPool;

struct PackedData{
	std::vector<GuidedPath> GPPacked;
	int iSize;
	PackedData(WorkPool currGP){
		int iLen = currGP.size();
		this->iSize = iLen;
		for (int i = 0; i < iLen && !currGP.empty(); i++)
		{
			GPPacked.push_back(currGP.top());
			currGP.pop();
		}
	}
	PackedData(int iNumGP){
		this->iSize = iNumGP;
		for ( int i = 0; i < iNumGP; i++){
			GPPacked.push_back(GuidedPath());
		}
	}

	void GetWorkPool(WorkPool &currGP){
		for (int i = 0 ; this->iSize; i++){
			currGP.push(GPPacked.at(i));
		}
	}		
};

class dpll{
public:
	dpll();
	dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes, int MAX_DEPTH_ALLOWED);
	dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes, 
		int MAX_DEPTH_ALLOWED,int iProc,int nProc,bool bMasterProc);
	~dpll();

	
	void initMaster();

	void SlaveInitialRecv();

	void SlaveAskForMoreWork();

	bool IsThereActiveSlave();

	bool MasterListener();

	void printResult(int bSolve);

	void MasterGenerateWork(int destPE);

	void packGPToSend(WorkPool &currGPToSend, int iGPToSend = 10);

	void SlaveSplitWork();

	bool runDPLL();
private:
	SATSET m_SATSET;
	int m_iMaxClause;
	int m_iMaxVarTypes;
	int m_iHighestC;
	int m_iMAX_DEPTH_ALLOWED;
	int m_iConflicts;
	int m_iMAX_GPCount;

	int m_iLastProcAsked;
	clock_t m_startTime;


	WorkPool m_SlaveWorkPool;
	WorkPool m_MasterWorkPool;
	bool m_bMasterProc;
	int iNextProcToAsk;
	int m_iProc;
	int m_nProc;

	SolSet WorkerActivityList;

	SolSet getNewSolSet();
	bool isAllVarAssigned(SolSet currSol);
	
	bool evalTruthValue(int iVar, int currAssign);

	void printSolSet(SolSet currSol);
	int pickVar(SATSET currClauses,SolSet currSol);
	void MasterProduceInitialGP();
	void LunchSlaves();

	void MasterAskForMoreWork();

	void GetNextActiveProc();
};



#endif