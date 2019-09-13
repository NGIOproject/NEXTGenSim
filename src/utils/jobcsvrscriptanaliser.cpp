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
#include <utils/jobcsvrscriptanaliser.h>

#include <utils/utilities.h>

namespace Utils {

/**
 * The constructor for the class 
 * @param CSVToAnalyse The CSV input file to be analyzed
 * @param ScriptPath  The path of the script file that wants to be analyzed 
 * @param OutPutDir The output dir where the files of the analysis will be dump
 * @param arch The architecture definition, used for correlate some variables 
 * @param log The logging engine
 * @param ScriptsDir The directory where the temporary scripts will be created 
 * @param RBinaryPath The file path for the binary path 
 * @param ScriptsBaseDir The directory where scripts are located 
 */
JobCSVRScriptAnaliser::JobCSVRScriptAnaliser(string CSVToAnalyse,string ScriptPath, string OutPutDir,ArchitectureConfiguration* arch ,Log* log,string ScriptsDir,string RBinaryPath,string ScriptsBaseDir)
{
  this->CSVToAnalyse = CSVToAnalyse;
  this->ScriptsDir = ScriptsDir;
  
  this->scriptType = JOB_CSV_R_ANALYZER_BASIC;
    
  
  //first we create the file name where the proxy r file will be created (the once that will invoque the others)
  int seedFileName = rand();

  //expanding the variables 
  string filepath = expandFileName(ScriptsDir+"/"+to_string(seedFileName)+".r");  

  std::ofstream fout(filepath.c_str(), ios::trunc);
  
  //now its time to output the needed params 
  //setting up the global variables for the workload if not set
  fout << "MachineProcessors<-"+to_string(arch->getNumberCPUs()) << endl << endl;

  //FileNameWhere2StoreTheGraphs
  fout << "thePDFFile<-\""+OutPutDir+"/SimulationGraphicalAnalysis.pdf\"" << endl << endl;  
  
  //the csv input file 
  fout << "CSVFilePath<-\""+CSVToAnalyse+"\""<< endl << endl;
  
  //
  fout << "ScriptsBaseDir<-\""+ScriptsBaseDir+"\"" << endl << endl;
  
  //time to inline the call to the source file 
  fout << "source(\""+ScriptPath+"\")" << endl << endl;
  
  fout.close();
  
  //now its time to create the params and invoque the upper class constructors 
  string command = RBinaryPath;
  string params = " --no-save < "+filepath;
  
  //no header params are needed 
  this->command = command;
  this->params = params;
  this->outputRequired = false;
  this->log = log;
  this->headerFile = NULL;
  this->TemporaryDir = ScriptsDir;
}


/**
 * The default descrutor for the class 
 */
JobCSVRScriptAnaliser::~JobCSVRScriptAnaliser()
{  
}

/**
 * Function that starts the analysis of the CSV file 
 */
void JobCSVRScriptAnaliser::analyze()
{
  //basically execute the execute class for the upper class 
  this->execute(); 
}

}
