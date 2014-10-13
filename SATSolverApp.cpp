
#include <iostream>
#include <stdio.h>
#include <memory>
#include "time.h"
#include <string>

#include "InputParser.h"
#include "Constants.h"
#include "dpll.h"

#include <mpi.h>


typedef enum eAlgo{eGA=0,eDPLL} eAlgo;
int main(int argc, char ** argv){
	
	int nProc, iProc, namelen,count;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&nProc);
	MPI_Comm_rank(MPI_COMM_WORLD,&iProc);


	int iLen;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	//gethostname(hostname, 1024);
	MPI_Get_processor_name(hostname,&iLen);

	printf("SIZE = %d RANK = %d HostName = %s\n", nProc,iProc,hostname);
	if (iProc == MASTERPROC){
		eAlgo eAlgoSelected = eDPLL;
		int MAX_DEPTH_ALLOWED = -1; //Temporary number that will be a max that cause no trouble for all current cases
		if (argc == 1)
		{
			fprintf(stderr,"No algo selected, defaulting to hillClimb\n");
		}
		if (argc >= 2)
		{	
			std::string algoSelection =  argv[1];
			if (algoSelection == "Genetics")
			{
				eAlgoSelected = eGA;
			}
			else if (algoSelection == "dpll")
			{
				eAlgoSelected = eDPLL;
			}
			else{
				fprintf(stderr, "Invalid Algorithm selection, please choose from 'hillClimb'|'dpll'|'Genetics' \n" );
				return 0;
			}
		}
		if (argc >= 3)
		{
			MAX_DEPTH_ALLOWED = atoi(argv[2]);

		}
		if (argc >= 4)
		{
			fprintf(stderr,"Wrong command format\n");
			fprintf(stderr,"Usage: %s AglorithmMode{'hillClimb'|'dpll'|'Genetics' < 'inputFilePath'}\n",argv[0]);
			return 0;
		}

		switch(eAlgoSelected){
		case eGA:{
			//fprintf(stderr, "GA in process\n" );

			break;
		}
		case eDPLL:{
			//fprintf(stderr, "dpll in process\n" );
			dpll * pSolver = new dpll(GetParser.getInputData(),GetParser.getSATMaxClause(),
				GetParser.getSATMaxVarType(),MAX_DEPTH_ALLOWED,iProc,nProc);

			pSolver->initMaster();

			delete pSolver;
			pSolver = NULL;
			break;
		}
		default:{
			break;
		}
		}
	}
	MPI_Finalize();
	return(0);
}