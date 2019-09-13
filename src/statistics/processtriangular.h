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
#ifndef STATISTICSPROCESSTRIANGULAR_H
#define STATISTICSPROCESSTRIANGULAR_H

#include <statistics/process.h>

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
//@class Generates a Triangular distribution.
//@base public | Process.
class ProcessTriangular : public Process
{
  //@cmember Left triangle edge.
  double a;
  //@cmember Right triangle edge.
  double b;
  //@cmember Height triangle edge.
  double c;
public:
  //@cmember Creates a new triangular distribution.
  ProcessTriangular(){}
  //@cmember Creates a new triangular distribution.
  ProcessTriangular(double a0, double b0, double c0, int stream);
  ProcessTriangular(double a0, double b0, double c0);

  //@cmember Returns left triangle edge.
  double GetLowerLimit();
  //@cmember Returns right triangle edge.
  double GetUpperLimit();
  //@cmember Returns height triangle edge.
  double GetHigherLimit();
  
  //@cmember Sets The limits
  int SetLimits (double lower, double upper, double higher);
  //@cmember Sets left triangle edge.
  int SetLowerLimit (double a0);
  //@cmember Sets right triangle edge.
  int SetUpperLimit (double b0);
  //@cmember Sets height triangle edge.
  int SetHigherLimit (double c0);
  //@cmember Returns a random number.
  virtual UDTIME DeltaProx();
};

}

#endif
