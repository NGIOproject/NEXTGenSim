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
#ifndef STATISTICSPROCESS_H
#define STATISTICSPROCESS_H


#define MODLUS 2147483647
#define MULT1 24112
#define MULT2 26143

#ifndef UDTIME
#define UDTIME double
#endif

namespace Statistics {

/**
	@author Francesc Guim,C6-E201,93 401 16 50, <fguim@pcmas.ac.upc.edu>
*/
class Process{
// A partir d'aquesta classe, hom pot derivar qualsevol tasca o objecte que necessiti d'una determinada distribuci�. Tan sols
// haur� de programar la constructora de la seva classe i la funci� DeltaProx().
  //lastnumber permet independitzar l'array de nombres aleatoris de cada un dels processos de generaci� de n�meros aleatoris.
  //@cmember Keeps the last random number generated. 
  long lastNumber;
  //@cmember Generates a random number by Law & Kelton method.
  double rando2(long stream);
protected: 
  //@cmember Array of seeds for generate random numbers.
	static int lastStream;
	static long zrng[101];

	double epsilon;
public:

	 enum distribution_type_t {
			NORMAL_DISTRIBUTION=0,
			LOGNORMAL_DISTRIBUTION,
			EXPONENTIAL_DISTRIBUTION,
			POISSON_DISTRIBUTION,
    };

  //@cmember Returns the last random number generated.
  long GetLastNumber();
  //@cmember Keeps the last random number generated.
  int SetLastNumber(long lastNumber);
  //@cmember  Generates a random number by Law & Kelton method.
  double rando(long stream=0);
  //@cmember Returns a random number. This function will be overriden by derived classes.
  //ConvertToTime Indica si cal convertir el valor a format temporal
  virtual UDTIME DeltaProx();

  Process();
  //@cmember Class destructor
  virtual ~Process();
};

}

#endif
