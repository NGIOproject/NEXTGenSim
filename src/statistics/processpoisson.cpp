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
#include <statistics/processpoisson.h>
#include <math.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessPoisson
*******************************************************************/
//@mfunc Creates a new Poission distribution.
//@parm double | lambda0 | Frequency beetwen arrivals.
//@parm int | stream | Random nuber stream.
ProcessPoisson::ProcessPoisson(double lambda0,  int stream)
{
  lambda = lambda0;
  // per a evitar zeros en el log de DeltaProx()
  SetLastNumber(zrng[stream]);
}

ProcessPoisson::ProcessPoisson(double lambda0)
{
  lambda = lambda0;
  // per a evitar zeros en el log de DeltaProx()
  SetLastNumber(zrng[lastStream]);
}

//@mfunc Returns the frequency of enities arrivals.
//@rdesc Returns the frequency of enities arrivals.
double ProcessPoisson::GetMu()
{
	return lambda;
}

//@mfunc Sets the frequency of enities arrivals.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Lambda less or equal tahan zero.
//@parm double | lambda0 | frequency of enities arrivals.
int ProcessPoisson::SetMu(double lambda0)
{
	if (lambda0 <= 0) return -1;
	else lambda=lambda0;
	return 0;
}

//@mfunc Returns a poisson random number.
//@rdesc Returns a poisson random number.
UDTIME ProcessPoisson::DeltaProx()
{
  double ra=rando();
  ra = ra < epsilon? epsilon : ra;  // Num aleat ]0,1]
  UDTIME tmp=(UDTIME)(-log(ra) / (lambda));           // Law pag. 507
  if (tmp<0) tmp=DeltaProx();
  return tmp;
}

}
