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
#include <datamining/wekadiscretizer.h>
#include <stdlib.h>
#include <utils/utilities.h>
#include <assert.h>
#include <time.h>

namespace Datamining {

/**
 * @param Log A reference to the logging engine 
 * @param DirOutputFiles Indicates the dir we the temporary files will be output 
 */
WekaDiscretizer::WekaDiscretizer(Log* log,string DirOutputFiles): Discretizer(log), WekaObject(log)
{
  this->DirOutputFiles = DirOutputFiles;
}


/**
 * The destructor for the class 
 */

WekaDiscretizer::~WekaDiscretizer()
{
}


/**
* This is the minimum operation that a given discretizer must implement. That is, given a set of values of a given attribute the discretizer should return a set of nominal values containing the discretization values for the 
* @param input The vector with the real values to discretize 
* @return A vector containing the nominal values with the discretization
*/
vector<string>* WekaDiscretizer::discretize(vector<string>* input)
{
  //first we create the arff file that will be provided to the weka 
  //the instances 
  InstancesSet instances;  
  instances.push_back(input);
  
  //the attribute definition 
  vector<string> attribute;
  attribute.push_back("ToDiscretize");
  
  vector<native_t> attType;
  attType.push_back(DOUBLE);
  
  //generate the random temporary file name 
	srand((unsigned int)time(NULL));
  string random = to_string(rand());
  string inputFileName = this->DirOutputFiles+"Continue"+random+".arff";
  string outputFileName = this->DirOutputFiles+"Discretized"+random+".arff";
  
  //we generate the input file path 
  this->generateArffFile(inputFileName,&instances,&attribute,&attType,input->size());	
  
  //we discretize the attributes of the file 
  this->discretize(inputFileName,outputFileName,&attribute);
	
  //we load the weka generated file and return it 
  InstancesSet* discretized = this->loadInstancesArffFile(outputFileName,1);
  
  vector<string>* result = discretized->operator[](0);
  
  //we free the instance set and return the result 
  delete discretized;
  
  return result;
  
}

/**
 * This function discretize the specified attributes file in the specified file and save the discretized arff file in the output file 
 * @param IntPutArffFile The input arff file 
 * @param OutPutArffFile The output arff file 
 * @param attributes The attributes to discretize 
 */
void WekaDiscretizer::discretize(string IntPutArffFile,string OutPutArffFile,vector<string>* attributes)
{  
  string strAttributes = "";

  for(vector<string>::iterator it = attributes->begin(); it != attributes->end(); ++it)
  {
    if(strAttributes != "")
      strAttributes+=",";
      
    strAttributes+=*it;
  }

  string params = this->WekaDiscretizationParameters+" -R "+strAttributes+" -i "+IntPutArffFile+" -o "+OutPutArffFile;    
  InvoqueWekaFunctionality(this->WekaDiscretizationClass,params,false);
}

}
