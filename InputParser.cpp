#include <mpi.h>

#include "InputParser.h"
#include <iostream>
#include <string>
#include <limits.h>
#include <iostream>
#include <stdio.h>



//public

InputParser::~InputParser(){
	/*int iLen = m_SATSET.size();
	for(int i = 0 ; i < iLen ; ++i){
		int jLen = m_SATSET[i].size();
		for (int j = 0 ; j< jLen; ++j)
		{	
			delete m_SATSET[i][j];
			m_SATSET[i][j] =NULL;
		}
	}*/
	m_SATSET.clear();
}
void InputParser::ReadAndParseData(){
	m_SATSET.clear();

	if(std::cin.eof()) return;
	//read input format
	std::cin >> m_cStatus;
	std::cin >> m_sProblemFormat;
	std::cin >> m_iMaxVarType;
	std::cin >> m_iMaxClauses;
	std::cin.ignore();
	//printf("%c %s %d %d \n", m_cStatus, m_sProblemFormat.c_str(), m_iMaxVarType,m_iMaxClauses);

	//Read all SAT clauses
	for (int i = 0; i < m_iMaxClauses ; ++i)
	{
		Clause currClause;
		int currInput;
		std::cin >> currInput;
		while(currInput)
		{
			currClause.push_back(currInput);
			std::cin >> currInput;
		}
		std::cin.ignore(INT_MAX,'\n');
		m_SATSET.push_back(currClause);
	}
	std::cin.ignore(INT_MAX);
}

void InputParser::printSet()
{
	printf("There should be %d clauses, we have %lu clauses\n",m_iMaxClauses,m_SATSET.size() );
	for ( int i = 0 ; i < m_iMaxClauses ; i++)
	{
		int jLen = m_SATSET[i].size();
		for ( int j = 0; j < jLen ; j++){
			std::cout << m_SATSET[i][j] << " ";
		}
		std::cout << std::endl;
	}
	//std::cout << std::flush;
}


InputParser & InputParser::Parser(){
	static InputParser _Parser;

	return _Parser;
}
