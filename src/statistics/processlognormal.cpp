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
#include <statistics/processlognormal.h>
#include <math.h>

namespace Statistics {


/*******************************************************************
	Metodes ProcessLogNormal
*******************************************************************/
//@mfunc Creates a new Lognormal instance.
//@parm double | mu0 | Average of lognormal distribution.
//@parm double | sigma0 | Stdev of lognormal distribution.
//@parm int | stream | Llavor del n�mero aleatori.
ProcessLogNormal::ProcessLogNormal(double mu0,double sigma0,  int stream) :
	   ProcessNormal(mu0, sigma0)
{
  //La mu i la sigma son heredades de la classe pare.
  SetLastNumber(zrng[stream]);
}

ProcessLogNormal::ProcessLogNormal(double mu0,double sigma0) :
	   ProcessNormal(mu0, sigma0)
{
  //La mu i la sigma son heredades de la classe pare.
  SetLastNumber(zrng[lastStream]);
}

//@mfunc Returns a lognormal random number.
//@rdesc Returns a lognormal random number.
UDTIME ProcessLogNormal::DeltaProx()
{
    UDTIME tmp=(UDTIME) exp(ProcessNormal::DeltaProx()); // Law pag. 492
	if (tmp<0) tmp=DeltaProx();
	return tmp;
}


}
