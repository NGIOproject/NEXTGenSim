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
#include <statistics/processexponential.h>
#include <math.h>

namespace Statistics {


/////////////////////////////////////////////////////
//Metodes ProcessExponential

//@mfunc   //@cmember Creates a new exponential distribution.
//@parm double | mu0    | Time beetwen arrivals.
//@parm int | stream | Random number stream.
ProcessExponential::ProcessExponential(double mu0,  int stream)
{
  mu = mu0;
  // per a evitar zeros en el long de DeltaProx()
  SetLastNumber(zrng[stream]);
}

ProcessExponential::ProcessExponential(double mu0)
{
  mu = mu0;
  // per a evitar zeros en el long de DeltaProx()
  SetLastNumber(zrng[lastStream]);
}

//@mfunc Returns the time beetwen arrivals.
//@rdesc Returns the time beetwen arrivals.
double ProcessExponential::GetMu()
{
  return mu;
}

//@mfunc Sets the time beetwen arrivals.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Mu less or equal tahan zero.
//@parm double | mu0 | Time beetwen arrivals.
int ProcessExponential::SetMu(double mu0)
{
 if (mu0 <= 0) return -1;
 else mu=mu0;
 return 0;
}

//@mfunc Returns an exponential random number.
//@rdesc Returns an exponential random number.
UDTIME ProcessExponential::DeltaProx()
{
  double ra=rando();
  
  ra = ra < epsilon? epsilon: ra;  // Num aleat ]0,1]

 // ra = ra < epsilonExpon ? epsilonExpon : ra;	// Num aleatori ]0,1]
  UDTIME tmp=(UDTIME)(-log(ra) * (mu));      	// Law pag. 486
  if (tmp<0) tmp=DeltaProx();

	return tmp;
}


}
