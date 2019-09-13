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
#include <statistics/processerlang.h>
#include <math.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessErlang
*******************************************************************/
//@mfunc Creates a new Erlang.
//@parm double | beta0 | Erlang's parameter.
//@parm int | m0 | Number of stages.
//@parm int | stream | Random number stream.
ProcessErlang::ProcessErlang(double beta0,int m0)
{
	beta=beta0;
	m=m0;
	SetLastNumber(zrng[lastStream]);
}

ProcessErlang::ProcessErlang(double beta0,int m0, int stream)
{
  beta=beta0;
  m=m0;
  SetLastNumber(zrng[stream]);
}

//@mfunc Returns the number of stages.
//@rdesc Return the number of stages.
unsigned int ProcessErlang::GetStage()
{
  return m;
}

//@mfunc Sets the number of stages.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Number of stages less or equal than zero.
//@parm int | m0 | Number of stages.
int ProcessErlang::SetStage(int m0)
{
  if (m0 <= 0) return -1;
  else m=m0;
  return 0;
}

//@mfunc Returns beta parameter.
//@rdesc Returns beta parameter.
double ProcessErlang::GetBeta()
{
	return beta;
}

//@mfunc Sets beta parameter.
//@rdesc Returns a flag.
//@flag 0  | Operation was succesfull.
//@flag -1 | Beta less or equal than zero.
//@parm double | beta0 | Erlang's parameter.
int ProcessErlang::SetBeta(double beta0)
{
  if (beta0 <= 0) return -1;
  else beta=beta0;
  return 0;
}

//@mfunc Returns an Erlang random number.
//@rdesc Returns an Erlang random number.
UDTIME ProcessErlang::DeltaProx()
{
  double ra=1;
  int i;
  for(i=1;i<=m;i++) ra *= rando(); // Num aleatori [0,1]

  ra = ra < epsilon? epsilon : ra;        // Num aleat ]0,1]

  UDTIME tmp=(UDTIME) (-log(ra)*beta / m);               // Law pag. 486
  
  if (tmp<0) tmp=DeltaProx();
	
	return tmp;
}


}
