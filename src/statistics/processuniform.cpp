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
#include <statistics/processuniform.h>

namespace Statistics {



/////////////////////////////////////////////////////
//Metodes ProcessUniform

//@mfunc Creates a new uniform distribution.
//@parm double | a0 | Lower edge.
//@parm double | b0 | Upper edge.
//@parm int | stream | Stream of random numbers.
ProcessUniform::ProcessUniform(double a0, double b0)
{
  a = a0;
  b = b0;
  SetLastNumber(zrng[Process::lastStream]);
}
 

//@mfunc Creates a new uniform distribution.
//@parm double | a0 | Lower edge.
//@parm double | b0 | Upper edge.
//@parm int | stream | Stream of random numbers.
ProcessUniform::ProcessUniform(double a0, double b0,  int stream)
{
  a = a0;
  b = b0;
  SetLastNumber(zrng[stream]);
}

//@mfunc Returns the lower edge.
//@rdesc Returns the lower edge.
double ProcessUniform::GetLowerLimit()
{
	return a;
}

//@mfunc Returns the upper edge.
//@rdesc Returns the upper edge.
double ProcessUniform::GetUpperLimit()
{
	return b;
}

//@mfunc Sets the lower edge.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Lower edge great or equal than upper edge.
//@parm double | a0 | Lower edge.

//Els limits s'han d'establir de cop, sino no es pot comparar el
//limit inferior amb el superior pq aquest ENCARA NO ESTA DEFINIT.
//Les operacions SetUpperLimit i SetLowerLimit NO s'haurien d'usar.
int ProcessUniform::SetLimits(double inf, double sup)
{
	if (inf>=sup)
		return -1;
	else
	{
		a=inf;
		b=sup;
		
		return 0;
	}
}

int ProcessUniform::SetLowerLimit(double a0)
{
	if (a0 >= b)
		return -1;
	else
		a=a0;

	return 0;
}

//@mfunc Sets the upper edge.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Upper edge less or equal than lower edge.
//@parm double | a0 | Upper edge.
int ProcessUniform::SetUpperLimit(double b0)
{
	if (b0 <= a)  return -1;
	else b=b0;
	return 0;
}

//@mfunc Returns an uniform random number.
//@rdesc Returns an uniform random number.
UDTIME ProcessUniform::DeltaProx()
{
  UDTIME tmp=(UDTIME)( a + (rando())*(b - a) );
  if (tmp<0) tmp=DeltaProx();

  return tmp;
}


}
