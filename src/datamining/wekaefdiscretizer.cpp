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
#include <datamining/wekaefdiscretizer.h>
#include <utils/utilities.h>
#include <assert.h>

namespace Datamining {

/**
 * @param Log A reference to the logging engine 
 * @param DirOutputFiles Indicates the dir we the temporary files will be output 
 * @param NumbBins Indicates the number of beens to create, if -1 is provided the nubmer of bins for the discretization has to be automatically found 
 */
WekaEFDiscretizer::WekaEFDiscretizer(Log* log,string DirOutputFiles,int NumbBins) : WekaDiscretizer(log,DirOutputFiles)
{
  this->NumbBins = NumbBins;

  this->WekaDiscretizationClass = "weka.filters.unsupervised.attribute.Discretize"; 
  
  if(NumbBins != -1)
    this->WekaDiscretizationParameters = "-F -M -B "+to_string(NumbBins)+" -1.0";
  else
    this->WekaDiscretizationParameters = "-O -F -1.0 -M";

}

/**
 * The destructor for the class 
 */
WekaEFDiscretizer::~WekaEFDiscretizer()
{
}


}
