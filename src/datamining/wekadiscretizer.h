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
#ifndef DATAMININGWEKADISCRETIZER_H
#define DATAMININGWEKADISCRETIZER_H

#include <datamining/discretizer.h>
#include <datamining/wekaobject.h>

namespace Datamining {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** This class implements a datamining weka discretizer based on the interfaces defined in the discretizer type */
class WekaDiscretizer : public Discretizer, WekaObject {
public:
  WekaDiscretizer(Log* log,string DirOutputFiles);
  ~WekaDiscretizer();

  virtual vector<string>* discretize(vector<string>* input);
  void discretize(string IntPutArffFile,string OutPutArffFile,vector<string>* attributes);
  
protected:
  string WekaDiscretizationClass ; /** Indicates the java weka class that implements the discretization method - to be set by the subclasses */
  string WekaDiscretizationParameters; /** Indicates the parametrs that have to be provided to the discretizer - to be set by the subclasses */

private: 
  string DirOutputFiles ; /** Indicates the directory where the temporary weka arff files will be stored  */
  
};

}

#endif
