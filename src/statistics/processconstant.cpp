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
#include <statistics/processconstant.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessConstant
*******************************************************************/
//@mfunc Creates a new constant distribution.
//@parm double | k0 | Constant value.
ProcessConstant::ProcessConstant(double k0)
{
  k = k0;
}

//@mfunc Returns the constant value.
//@rdesc Returns the constant value.
double ProcessConstant::GetConstant()
{
  return k;
}

//@mfunc Sets the constant value.
//@rdesc Always return zero.
//@parm double | k0 | Constant value.
int ProcessConstant::SetConstant(double k0)
{
	k=k0;
 return 0;
}

//@mfunc Returns a constant random number.
//@rdesc Returns a constant random number.
UDTIME ProcessConstant::DeltaProx()
{
  UDTIME tmp=(UDTIME)k;

  if (tmp<0) tmp=k;//TODO Ara for�o que retorni el valor k, no t� sentit si �s una constant que faci un deltaprox d'ell mateix o potser si ???  DeltaProx();

	return tmp;
}

}
