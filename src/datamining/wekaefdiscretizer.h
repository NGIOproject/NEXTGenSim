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
#ifndef DATAMININGWEKAEFDISCRETIZER_H
#define DATAMININGWEKAEFDISCRETIZER_H

#include <datamining/wekadiscretizer.h>

namespace Datamining {

/**	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/

/** 
EqualFrequency

- Will Discretize the attribute with the same number of instances per bin
- Number of bins : Finds automatically the number of intervals in which the attributes will be discretized if -1 is provided. Otherwise the number of bins provided is used.

Parameters: -F -B NUMBERBEANS -M -1.0 -c X1,X2 -i INPUTFILE -o OUTPUTFILE 
Parameters num bins automatic: -O -F -1.0 -M -c X1,X2 -i INPUTFILE -o OUTPUTFILE -c last  
*/
class WekaEFDiscretizer : public WekaDiscretizer {
public:
  WekaEFDiscretizer(Log* log, string OutputDir,int NumbBins);;
  ~WekaEFDiscretizer();
  
  
private: 
 int NumbBins;  

};

}

#endif
