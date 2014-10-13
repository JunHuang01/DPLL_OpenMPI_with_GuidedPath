#include <iostream>
#include <stdio.h>
#include <memory>
#include "time.h"
#include <string>

#include "InputParser.h"
#include "Constants.h"
#include "dpll.h"


typedef enum eAlgo{eGA=0,eDPLL} eAlgo;
int main(int argc, char ** argv){
	srand(time(NULL));
	eAlgo eAlgoSelected = eDPLL;
	int MAX_DEPTH_ALLOWED = 200; //Temporary number that will be a max that cause no trouble for all current cases
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
			GetParser.getSATMaxVarType(),MAX_DEPTH_ALLOWED);

		pSolver->Solve();

		delete pSolver;
		pSolver = NULL;
		break;
	}
	default:{
		break;
	}
	}
	return 0;
}