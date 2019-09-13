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
#include <utils/rscript.h>

namespace Utils {

/**
 * 
 * @param command The command line to be executed (recommended to use the complet path)
 * @param paramas The params that will be provided to the executable
 * @param outputRequired A bool indicating if the output is required
 * @param Log The loggin engine 
 * @param headerFile A set of vector fields indicating the lines that have to be put inside the header R file, this in case required.. usually this vector is used for setup some R variables that are used for the simulator. This mainly depends on the R script used
 * @param tempDir The directory where the temporary files will be dump
 */
RScript::RScript(string command,string params,bool outputRequired,Log* log, vector<string>* headerFile,string tempDir) : ExecuteCommand(command,params,outputRequired,log,tempDir)
{
  this->headerFile = headerFile;
}

/**
 * The default constructor
 * @return 
 */
RScript::RScript()
{
}

/**
 * The default constructor for the class.
 */
RScript::~RScript()
{
}

/**
 * Returns the r script analyzer type used in the current implementation
 * @return A r_scripts_type with the analyzer type used in the simulator
 */
r_scripts_type RScript::getscriptType() const
{
  return scriptType;
}


/**
 * Sets the analyzer type used in the implementation
 * @param theValue The analyzer type used
 */
void RScript::setScriptType(const r_scripts_type& theValue)
{
  scriptType = theValue;
}

}

