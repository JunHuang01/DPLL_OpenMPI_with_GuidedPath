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

};

typedef std::stack<GuidedPath> WorkPool;

class dpll{
public:
	dpll();
	dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes, int MAX_DEPTH_ALLOWED);
	dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes, 
		int MAX_DEPTH_ALLOWED,int iProc,int nProc,bool bMasterProc);
	~dpll();


	void Solve();

	
	void initMaster();

	void SlaveInitialRecv();

	void SlaveAskForMoreWork();

	void MasterAskForMoreWork();

	bool IsThereActiveSlave();

	void packGPToSend(WorkPool &currGPToSend, int iGPToSend = 10);
private:
	SATSET m_SATSET;
	int m_iMaxClause;
	int m_iMaxVarTypes;
	int m_iHighestC;
	int m_iMAX_DEPTH_ALLOWED;
	int m_iConflicts;
	int m_iMAX_GPCount;
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
	bool runDPLL();
	bool evalTruthValue(int iVar, int currAssign);

	void printSolSet(SolSet currSol);
	int pickVar(SATSET currClauses,SolSet currSol);
	void MasterProduceInitialGP();
	void LunchSlaves();

};



#endif