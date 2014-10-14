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
	m_bMasterProc = false;
	m_iLastProcAsked = 0;
}

dpll::dpll(SATSET inputData,int iMaxClause, int iMaxVarTypes,int MAX_DEPTH_ALLOWED){
	m_SATSET = inputData;
	m_iMaxClause = iMaxClause;
	m_iMaxVarTypes = iMaxVarTypes;
	m_iHighestC = 0;
	m_iConflicts = 0;
	m_iMAX_GPCount = 0;
	m_iMAX_DEPTH_ALLOWED = MAX_DEPTH_ALLOWED;
	m_bMasterProc = false;
	m_iLastProcAsked = 0;
}

dpll::dpll(SATSET inputData,int iMaxClause,
 int iMaxVarTypes,int MAX_DEPTH_ALLOWED,
 int iProc, int nProc, bool bMasterProc){
	m_SATSET = inputData;
	m_iMaxClause = iMaxClause;
	m_iMaxVarTypes = iMaxVarTypes;
	m_iHighestC = 0;
	m_iConflicts = 0;
	m_iMAX_GPCount = 0;
	m_iMAX_DEPTH_ALLOWED = MAX_DEPTH_ALLOWED;
	m_bMasterProc = bMasterProc;
	m_iProc = iProc;
	m_nProc = nProc;
	m_startTime = clock();
	m_iLastProcAsked = 0;
}


dpll::~dpll(){

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


//shared routinue by master / slave, since this is data parallism.  Inital run happens by
//leveraging the iProcessLevel.
bool dpll::runDPLL()
{
	SolSet currSol;
	SATSET currClauses;
	int depth;
	int iCurrClauseCount;
	
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

		if ( m_bMasterProc && depth == MAX_PRE_PROCESS_LEVEL){
			m_iMAX_GPCount++;
			m_MasterWorkPool.push(GuidedPath(currSol,currClauses,depth));
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
		//fprintf(stderr, "%d is the checked in at depth %d\n",m_iProc,depth);
		SlaveSplitWork();
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

//Parallel public
void dpll::initMaster(){
	if (m_bMasterProc == false) return;
	for ( int i = 0 ; i < m_nProc ; i++){
		WorkerActivityList.push_back(WORKER_INACTIVE);
	}

	MasterProduceInitialGP();
	LunchSlaves();
}



//in paralllel, this is initial generate GP routine, should only be used by master once
void dpll::MasterProduceInitialGP()
{
	if (m_bMasterProc == false) return;

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
	int depth = 0;
	int iVarToPick = pickVar(m_SATSET,temp);

	leftSol.at(iVarToPick) = 1;
	rightSol.at(iVarToPick) = 0;

	//printSolSet(leftSol);
	//printSolSet(rightSol);
	m_SlaveWorkPool.push(GuidedPath(rightSol,m_SATSET,depth));
	m_SlaveWorkPool.push(GuidedPath(leftSol,m_SATSET,depth));

	bool bSolved = runDPLL();//|| runDPLL(rightSol,m_SATSET,0);
	timeElapsed = double(clock() - startTime)/CLOCKS_PER_SEC;
	fprintf(stderr, "Solve=%d\tTimeSpent=%f\tHighestC=%d\tConflicts=%d\tMaxGP=%d\n", 
		int(bSolved), timeElapsed, m_iHighestC, m_iConflicts, m_iMAX_GPCount);
}

void dpll::printResult(int bSolved){
	double timeElapsed = double(clock() - m_startTime)/CLOCKS_PER_SEC;
	int iGlobalConflicts = 0;
	int iGlobalHighestC = 0;
	MPI_Allreduce(&m_iConflicts,&iGlobalConflicts,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
	MPI_Allreduce(&m_iHighestC,&iGlobalHighestC,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
	fprintf(stdout, "Solve=%d\tTimeSpent=%f\tHighestC=%d\tConflicts=%d\tMaxGP=%d\n", 
		bSolved,timeElapsed, m_iHighestC, m_iConflicts, m_iMAX_GPCount);
}
void dpll::SlaveAskForMoreWork(){
	if (m_bMasterProc == true) return;

	MPI_Send(&m_iProc,1,MPI_INT,MASTERPROC,SlaveAskMasterTag,MPI_COMM_WORLD);

	MPI_Status status;
	int iTotalByteSizeOfGP = 0;
	
	WorkPool* tempRecvWorkPool;
	MPI_Recv(&iTotalByteSizeOfGP, 1, MPI_INT,MASTERPROC,MasterSendToSlaveTag,
		MPI_COMM_WORLD,&status);

	if (iTotalByteSizeOfGP > 0 ){
		tempRecvWorkPool = (WorkPool*)malloc(iTotalByteSizeOfGP);
	}

	MPI_Recv(tempRecvWorkPool,iTotalByteSizeOfGP,MPI_BYTE,MASTERPROC,MasterSendToSlaveTag,
		MPI_COMM_WORLD,&status);

	while(!tempRecvWorkPool->empty()){
		m_SlaveWorkPool.push(tempRecvWorkPool->top());
		tempRecvWorkPool->pop();
	}
}


bool dpll::MasterListener()
{
	MPI_Request request;
	int sourcePE = -1;
	MPI_Irecv(&sourcePE,1,MPI_INT,MPI_ANY_SOURCE,SlaveAskMasterTag,MPI_COMM_WORLD,&request);

	if(sourcePE>=0){
		WorkerActivityList.at(sourcePE) = WORKER_INACTIVE;
		if(!IsThereActiveSlave()){
			int bSolved = 0; //Set is Unsatisfiable
			printResult(bSolved);
			MPI_Finalize();
			return(0);
		}
		else{
			MasterGenerateWork(sourcePE);
		}
	}
}

void dpll::GetNextActiveProc(){
	int iTempProc = m_iLastProcAsked;
	m_iLastProcAsked = m_iLastProcAsked >= m_nProc-1 ? 0 : m_iLastProcAsked++;
	while(WorkerActivityList.at(m_iLastProcAsked) == WORKER_INACTIVE){
		m_iLastProcAsked = m_iLastProcAsked >= m_nProc-1 ? 0 : m_iLastProcAsked++;
		if (iTempProc == m_iLastProcAsked){
			fprintf(stderr, "No active proc, should terminate\n");
			printResult(0);
			MPI_Finalize();
		}
	}
}
void dpll::MasterAskForMoreWork()
{
	if (m_bMasterProc == false) return;	
	GetNextActiveProc();
	int iAsk = MasterAskSlaveTag;

	MPI_Status status;
	MPI_Request request;
	MPI_Isend(&iAsk,1,MPI_INT,m_iLastProcAsked,MasterAskSlaveTag,MPI_COMM_WORLD,&request);

	int iTotalByteSizeOfGP = 0;
	
	WorkPool* tempRecvWorkPool;
	MPI_Recv(&iTotalByteSizeOfGP, 1, MPI_INT,m_iLastProcAsked,SlaveSendToMasteTag,
		MPI_COMM_WORLD,&status);

	if (iTotalByteSizeOfGP > 0 ){
		tempRecvWorkPool = (WorkPool*)malloc(iTotalByteSizeOfGP);
	}

	MPI_Recv(tempRecvWorkPool,iTotalByteSizeOfGP,MPI_BYTE,m_iLastProcAsked,SlaveSendToMasteTag,
		MPI_COMM_WORLD,&status);

	while(!tempRecvWorkPool->empty()){
		m_MasterWorkPool.push(tempRecvWorkPool->top());
		tempRecvWorkPool->pop();
	}


}

bool dpll::IsThereActiveSlave(){
	int iWorkerSize =  WorkerActivityList.size();
	for ( int i = 0; i < iWorkerSize ; i++)
	{
		if (WorkerActivityList.at(i)) return true;
	}

	return false;
}

//Parallel private

void dpll::SlaveSplitWork(){
	if(m_bMasterProc == true) return; 
	int iRecv = -1;
	MPI_Request request;
	MPI_Irecv(&iRecv, 1, MPI_INT, MASTERPROC,MasterAskSlaveTag,
		MPI_COMM_WORLD,&request);

	if (iRecv == MasterAskSlaveTag){
		WorkPool currGPToSend;
		int iCurrGPCount = m_SlaveWorkPool.size();
		int iSendGPCount = iCurrGPCount/2;
		for ( int i = 0 ; i < iSendGPCount && !m_SlaveWorkPool.empty() ; i++)
		{
			currGPToSend.push(m_SlaveWorkPool.top());
			m_SlaveWorkPool.pop();
		}

		int totalGPByteSize = int(sizeof(currGPToSend));

		MPI_Isend(&totalGPByteSize,1,MPI_INT,MASTERPROC,SlaveSendToMasteTag,
			MPI_COMM_WORLD,&request);
		MPI_Isend((void*)&currGPToSend,totalGPByteSize,MPI_BYTE,MASTERPROC,SlaveSendToMasteTag,
			MPI_COMM_WORLD,&request);
	}
}
void dpll::packGPToSend(WorkPool &currGPToSend, int iGPToSend){
	int iWorkPoolSize = m_MasterWorkPool.size();
	if (iWorkPoolSize < 40 )  iGPToSend = 1;
	for(int i = 0; i < iGPToSend && !m_MasterWorkPool.empty(); i++){
		currGPToSend.push(m_MasterWorkPool.top());
		m_MasterWorkPool.pop();
	}
}
void dpll::LunchSlaves()
{
	if (m_bMasterProc == false) return;
	int iWorkPoolSize = m_MasterWorkPool.size();
	fprintf(stderr, "The size of the master work pool is %d\n", iWorkPoolSize);

	int iNumGPToSend = ceil((iWorkPoolSize/2)/double(m_nProc));

	for ( int destPE = 0; destPE < m_nProc  ; ++destPE)
	{
		WorkPool GPToSend;
		packGPToSend(GPToSend,iNumGPToSend );

		fprintf(stderr, "Packed %d GP sent to %d proc \n",GPToSend.size(), destPE);
		int sourcePE = MASTERPROC;
		int Tag = InitialSendRecvTag;
		MPI_Status status;

		PackedData * myPackedData = new PackedData(GPToSend);
		int totalGPByteSize = int(sizeof(myPackedData));

		MPI_Send(&totalGPByteSize,1,MPI_INT,destPE,InitialSendRecvTag,
			MPI_COMM_WORLD);

		fprintf(stderr, "We started lunching slave %d\n",destPE );
		MPI_Send((void*)myPackedData,totalGPByteSize,MPI_BYTE,destPE,InitialSendRecvTag,
			MPI_COMM_WORLD);
		WorkerActivityList.at(destPE) = WORKER_ACTIVE;
	}

}


void dpll::SlaveInitialRecv(){
	if (m_bMasterProc == true) return;
	MPI_Status status;
	int iTotalByteSizeOfGP = 0;
	
	WorkPool* tempRecvWorkPool;
	MPI_Recv(&iTotalByteSizeOfGP, 1, MPI_INT,MASTERPROC,InitialSendRecvTag,
		MPI_COMM_WORLD,&status);

	PackedData * myPackedData;
	if (iTotalByteSizeOfGP > 0 ){
		fprintf(stderr, "We started at slave %d have recved %d byte\n",m_iProc ,iTotalByteSizeOfGP);
		myPackedData = (PackedData*)malloc(iTotalByteSizeOfGP);

		MPI_Recv(myPackedData,iTotalByteSizeOfGP,MPI_BYTE,MASTERPROC,InitialSendRecvTag,
		MPI_COMM_WORLD,&status);
	}


	if(status.MPI_SOURCE == MASTERPROC)
		fprintf(stderr, "We started at slave %d have recved %d GPs\n",m_iProc ,myPackedData->currGP.size());
		{while(!myPackedData->currGP.empty()){
			m_SlaveWorkPool.push(myPackedData->currGP.top());
			myPackedData->currGP.pop();
		}
		
		//fprintf(stderr, "%d Proc recved %d count of GP \n",m_iProc,tempRecvWorkPool->size() );
	}
}
void dpll::MasterGenerateWork(int destPE)
{
	WorkPool GPToSend;
	int iCurrGPCount = m_MasterWorkPool.size();
	if(iCurrGPCount <= 0){	
		MasterAskForMoreWork();
	}
	
	MPI_Request request;	
	packGPToSend(GPToSend);
	int totalGPByteSize = int(sizeof(GPToSend));
	MPI_Isend(&totalGPByteSize,1,MPI_INT,destPE,MasterSendToSlaveTag,
		MPI_COMM_WORLD,&request);
	MPI_Isend((void*)&GPToSend,totalGPByteSize,MPI_BYTE,destPE,MasterSendToSlaveTag,
		MPI_COMM_WORLD,&request);
	
	WorkerActivityList.at(destPE) = WORKER_ACTIVE;
}