/*
Author: Jun Huang
Date: 10/7/2014
SAT Solver Program
*/

#ifndef INPUTPARSER_H
#define INPUTPARSER_H


#include <memory>
#include <vector>
#include <string>


#include "CommonTypes.h"





class InputParser{
public:
	~InputParser();
	static InputParser& Parser();
protected:
	InputParser(){
		m_iMaxClauses = 0;
		m_iMaxVarType = 0;
		m_sProblemFormat = "";
		//m_SATSET.clear();
	}

private:
	SATSET m_SATSET;
	char m_cStatus;
	std::string m_sProblemFormat;
	int m_iMaxVarType;
	int m_iMaxClauses;
	//static InputParser *_Parser;
	InputParser(InputParser const&);
	void operator=(InputParser const&);
	

public:
	void ReadAndParseData();
	void printSet();
	SATSET getInputData(){return m_SATSET;}
	int getSATMaxClause(){return m_iMaxClauses;}
	int getSATMaxVarType(){return m_iMaxVarType;}
};

#endif 
