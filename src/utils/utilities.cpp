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
#include <utils/utilities.h>
#include <wordexp.h>
#include <stdio.h>
#include <string.h>


/**
 * Function evaluates a xpath expression and returns string if any, otherwise NULL. Assume file already open.
 * @param the xpath expression 
 */
string getStringFromXPathExpression(string const& xpathExpr,xmlDocPtr doc,bool exit_if_null) {
	string result;
	xmlNode* foundNode = NULL;
	foundNode = getNodeFromXPathExpression(xpathExpr,doc,exit_if_null);
	if (foundNode != NULL)
	{
		if(xmlNodeIsText(foundNode->children))
		{
			unsigned char *myCharString = xmlNodeGetContent(foundNode->children);
			result = string((const char *)myCharString);
			xmlFree(myCharString);
		}
		xmlFreeNode(foundNode);
	}
	return result;
}

/**
 * Function evaluates a xpath expression and returns NodeSet if any, otherwise NULL. Assume file already open.
 * @param the xpath expression 
 */
xmlNodeSet* getNodeSetFromXPathExpression(string const& xpathExpr,xmlDocPtr doc,bool exit_if_null) {
	xmlNodeSet* result = NULL;
	xmlXPathContext* myXPathContext = NULL;
	xmlXPathObject* myXPathObject = NULL;
	myXPathContext = xmlXPathNewContext(doc);					//Create context (Must free)
	if (myXPathContext)
	{
		myXPathObject = xmlXPathEvalExpression((xmlChar*) xpathExpr.c_str(),myXPathContext);	//Create object (Must be free by caller)
		if (myXPathObject &&										//Check if found
				!xmlXPathNodeSetIsEmpty(myXPathObject->nodesetval))		//And it's not empty
		{
			//Copy nodeSet to result and free object
			result = myXPathObject->nodesetval;		//Copy to result
			myXPathObject->nodesetval = NULL;		//because after the myXPathObject is freed and want to maintain the nodeSet for the result
			xmlXPathFreeObject(myXPathObject);		//Free
		}
		xmlXPathFreeContext(myXPathContext);		//Free context
	}
	assert(!(result == NULL && exit_if_null));
	return result;
}

/**
 * Function evaluates a xpath expression and returns Node if any, otherwise NULL. Assume file already open.
 * The returned object must be free after using it
 * @param the xpath expression 
 */
xmlNode* getNodeFromXPathExpression(string const& xpathExpr,xmlDocPtr doc,bool exit_if_null) {
	xmlNode* result = NULL;
	xmlNodeSet* myNodeSetFound = NULL;
	myNodeSetFound = getNodeSetFromXPathExpression(xpathExpr,doc,exit_if_null);
	if (myNodeSetFound &&
			myNodeSetFound->nodeNr == 1 &&
			myNodeSetFound->nodeTab[0]->type == XML_ELEMENT_NODE)
	{
		result = xmlCopyNode(myNodeSetFound->nodeTab[0],1);
		xmlXPathFreeNodeSet(myNodeSetFound);
	}
	assert(!(result == NULL && exit_if_null));
	return result;
}



void SplitLine(const std::string& str, const std::string& delim, std::deque<std::string>& output)
{
	unsigned int offset = 0;
	size_t delimIndex = 0;

	delimIndex = str.find(delim, offset);

	while (delimIndex != string::npos)
	{
		string piece = str.substr(offset, delimIndex - offset);

		if(piece.compare("") != 0 && piece.compare(" ") != 0 && piece.compare("\t") != 0)
			output.push_back(piece);

		offset += delimIndex - offset + delim.length();
		delimIndex = str.find(delim, offset);
	}

	output.push_back(str.substr(offset));
}

string itos(int i)// convert int to string
{
	stringstream s;
	s << i;
	return s.str();
}

string vtos(vector<int> i){
    stringstream s;
    for(vector<int>::iterator it = i.begin(); it != i.end(); it++){
        s << to_string(*it) << " ";
    }
    return s.str();
}

/*
 * This function converts double to string
 * As double integer part get's length 10 as maximum
 * a charResult[11] would be ok but
 * if it has fractional part then add a "." and 3 decimals
 * so charResult gets a max size of 11+1+3=15
 * @param theValue the boolean value
 * @return resultString the result string
 */
string ftos(double theValue)// convert double to string
{
	//string resultString;
	double *integerPartPtr = new double();
	double fracPart = 0;
	fracPart = modf(theValue,integerPartPtr);
	delete integerPartPtr;
	char charResult[32];
	if(fracPart == 0.0)
		sprintf(charResult,"%.0lf",theValue);
	else
		sprintf(charResult,"%.3lf",theValue);
	//resultString = string(charResult);
	string resultString(charResult);
	return resultString;
}
/*
 * This function converts boolean to string
 * Where true->"1" and false->"0"
 * @param theValue the boolean value
 * @return resultString the result string
 */
string btos(bool theValue)// convert double to string
{
	string resultString;
	resultString = theValue?"1":"0";
	return resultString;
}

/**
 * Translate a path containing potentially variables in the path (e.g. $ALVIO_ROOT) to expanded name
 * @param path containing the path to be expanded
 * @return The expanded path (resolving variables names)
 */
string expandFileName(string myPath)
{
	/* First version using bash

   FILE *fp1;
   char command[4096];

   sprintf(command, "echo \"%s\" 2>/dev/null", path.c_str());
   if ((fp1 = popen(command, "r")) == NULL || //Echo cmd
       fgets(command, 4096, fp1) == NULL) //Get echo results
   {                        // open/get pipe failed
     pclose(fp1);           // close pipe                   
     cout << "Error during path expansion " << endl;
     exit(1);     
   }
   pclose(fp1);             // close pipe
   command[strlen(command)-1] = '\0';// remove newline

   return string(command);

	 */

	/* Second version using wordexp */
	wordexp_t p;
	char **w;
	wordexp(myPath.c_str(), &p, 0);
	w = p.we_wordv;
	string resultString;
	if (p.we_wordc == 1)
	{
		resultString = string(w[0]);
		wordfree(&p);
	}
	else
	{
		cout << "Error during path expansion" << endl;
		exit(1);
	}
	return resultString;
}

bool fileExists(const std::string& name)
{
	if (FILE *file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	} else
		return false;
}

