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
#include <statistics/process.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>


namespace Statistics {


/*******************************************************************
	Metodes Process    
*******************************************************************/

 long Process::zrng[] = {         0,
 1973272912,  281629770,   20006270, 1280689831, 2096730329, 1933576050,
  913566091,  246780520, 1363774876,  604901985, 1511192140, 1259851944,
  824064364,  150493284,  242708531,   75253171, 1964472944, 1202299975,
  233217322, 1911216000,  726370533,  403498145,  993232223, 1103205531,
  762430696, 1922803170, 1385516923,   76271663,  413682397,  726466604,
  336157058, 1432650381, 1120463904,  595778810,  877722890, 1046574445,
   68911991, 2088367019,  748545416,  622401386, 2122378830,  640690903,
 1774806513, 2132545692, 2079249579,   78130110,  852776735, 1187867272,
 1351423507, 1645973084, 1997049139,  922510944, 2045512870,  898585771,
  243649545, 1004818771,  773686062,  403188473,  372279877, 1901633463,
  498067494, 2087759558,  493157915,  597104727, 1530940798, 1814496276,
  536444882, 1663153658,  855503735,   67784357, 1432404475,  619691088,
  119025595,  880802310,  176192644, 1116780070,  277854671, 1366580350,
 1142483975, 2026948561, 1053920743,  786262391, 1792203830, 1494667770,
 1923011392, 1433700034, 1244184613, 1147297105,  539712780, 1545929719,
  190641742, 1645390429,  264907697,  620389253, 1502074852,  927711160,
  364849192, 2049576050,  638580085,  547070247
  }; 

 int Process::lastStream=0;

Process::Process()
{
	epsilon = 1e-6;	
	lastStream%=100;
	lastStream++;
}

//@mfunc Class destroyer.
Process::~Process()
{
}

//@mfunc Returns a random number.
//@rdesc Returns a random number.
UDTIME Process::DeltaProx()
{
 return 0;
}


//@mfunc Returns a random number generated by Law & Kelton method.
//@rdesc Returns a random number.
//@parm long | sream | Stream for the random numbers sequence.
double Process::rando2(long stream)
{
 long zi, lowprd, hi31;

 zi = stream;
 lowprd = (zi & 65535) * MULT1;
 hi31 = (zi >> 16)*MULT1 + (lowprd >>16);
 zi = ((lowprd & 65535) - MODLUS) +
      ((hi31 & 32767) << 16) + (hi31 >>15);
 if (zi < 0) zi += MODLUS;
 lowprd = (zi & 65535) * MULT2;
 hi31 = (zi >> 16)*MULT2 + (lowprd >>16);
 zi = ((lowprd & 65535) - MODLUS) +
      ((hi31 & 32767) << 16) + (hi31 >>15);
 if (zi < 0) zi += MODLUS;
 SetLastNumber(zi);
 return (double)(((zi >> 7 | 1)+1)/16777216.0);
}


//@mfunc Returns a random number generated by Law & Kelton method.
//@rdesc Returns a random number.
//@parm long | sream | Stream for the random numbers sequence.
//@flag 0 | The last random number gives the stream.
double Process::rando(long stream)
{
  if (!stream) return rando2(GetLastNumber());
  else return rando2(stream);
}

//@mfunc Returns the last random number generated.
//@rdesc Returns the last random number generated.
long Process::GetLastNumber()
{
	return lastNumber;
}

//@mfunc Sets the last random number generated.
//@rdesc Always returns a 0. 
int Process::SetLastNumber(long ultimNumero)
{
	lastNumber = ultimNumero;
	return 0;
}


}
