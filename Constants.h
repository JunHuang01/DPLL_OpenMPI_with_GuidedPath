/*
Author: Jun Huang
Date: 10/7/2014
SAT Solver Program
*/


#ifndef CONSTANTS_H
#define CONSTANTS_H

#define GetParser InputParser::Parser()
#define MAX_TIME_LIMIT 180.0 //max time can be used is 3 minutes to solve the SAT Consttraint
#define MAX_TIME_LIMIT_HILL 0.5//60.0 //max time one instance of hill climb can run


#define UNASSIGNED -1
#define ASSIGNTRUE 1
#define ASSIGNFALSE 0



#define MASTERPROC 0
#define MAX_PRE_PROCESS_LEVEL 10


#define WORKER_INACTIVE 0
#define WORKER_ACTIVE 1

#define InitialSendRecvTag 55
#define MasterAskSlaveTag 56
#define SlaveAskMasterTag 57
#define MasterSendToSlaveTag 58
#define SlaveSendToMasteTag 59
#endif
