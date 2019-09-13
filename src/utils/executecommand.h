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
#ifndef UTILSEXECUTECOMMAND_H
#define UTILSEXECUTECOMMAND_H

#include <vector>

#include <utils/log.h>

using namespace std;
using std::vector;

namespace Utils {

/**
  @author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class has been defined for executed shell commands, with a given set of arguments. For instance scripts, binaris and others */
class ExecuteCommand{
public:
  ExecuteCommand(string command,string params,bool outputRequired,Log* log,string tmpdir);
  ExecuteCommand();
  ~ExecuteCommand();
  virtual vector<string>* execute();
  int MySystem(const char *cmd);
   
protected: 
  string command; /**< The command line that will be executed*/
  string params; /**< The parameters to be provided to the command line*/
  bool outputRequired; /**< A boolean indicating if the output is required or not*/
  string TemporaryDir; /**< The directory where the temporary files will be stored */
  Log* log; /**< The logging engine*/

};

}

#endif
