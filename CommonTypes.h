#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <memory>
#include <vector>
#include <string>


class SATVariable;

typedef std::string STRING;
typedef std::vector<int> Clause;
typedef std::vector<int> SolSet;
typedef std::vector<Clause> SATSET;

#endif