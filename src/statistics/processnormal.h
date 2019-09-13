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
#ifndef STATISTICSPROCESSNORMAL_H
#define STATISTICSPROCESSNORMAL_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a Normal distribution.
//@base public | Process.
class ProcessNormal : public Process
{
  //@cmember Distribution's Average.
  double mu;
  //@cmember Distribution's Stdev.
  double sigma;
public:
  //@cmember Creates a new Normal distribution.
	ProcessNormal(){ 
		mu=123;   
	}
  //@cmember Creates a new Normal distribution.
  ProcessNormal(double mu0,double sigma0, int stream);
  ProcessNormal(double mu0,double sigma0);
  //@cmember Returns distribution's average.
  double GetMu();
  //@cmember Returns distribution's stdev.
  double GetSigma();
  //@cmember Sets distribution's average.
  int SetMu(double mu0);
  //@cmember Sets distribution's stdev.
  int SetSigma(double sigma0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
