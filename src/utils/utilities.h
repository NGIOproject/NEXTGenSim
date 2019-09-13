/*
* Copyright 2007 Francesc Guim Bernat & Barcelona Supercomputing Centre (fguim@pcmas.ac.upc.edu)
* Copyright 2019 Daniel Rivas & Barcelona Supercomputing Centre (daniel.rivas@bsc.es)
* Copyright 2015-2019 NEXTGenIO Project [EC H2020 Project ID: 671951] (www.nextgenio.eu)
*
* This file is part of NEXTGenSim.
*
* NEXTGenSim is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEXTGenSim is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with NEXTGenSim.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef UTILITIES_H
#define UTILITIES_H

#include<iostream>
#include<string>
#include<sstream>
#include <vector>
#include <deque>
#include <limits>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>

/* libxml2 stuff */
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

using namespace std;
using std::vector;
using std::deque;


string itos(int i);
void SplitLine(const std::string& str, const std::string& delim, std::deque<std::string>& output);
string ftos(double theValue);
string btos(bool theValue);
string vtos(vector<int> theValue);
string expandFileName(string path);
bool fileExists(const std::string& name);

//libXML2 functions
string getStringFromXPathExpression(string const& xpathExpr,xmlDocPtr doc,bool exit_if_null = false);
xmlNode* getNodeFromXPathExpression(string const& xpathExpr,xmlDocPtr doc,bool exit_if_null = false);
xmlNodeSet* getNodeSetFromXPathExpression(string const& xpathExpr,xmlDocPtr doc,bool exit_if_null = false);

#endif
