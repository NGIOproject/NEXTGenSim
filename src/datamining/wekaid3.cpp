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
#include <datamining/wekaid3.h>
#include <assert.h>

namespace Datamining {

/**
 * The constructor for the class 
 * @param attributes A vector of string containing the names for the attributes that will be used for create the model 
 * @param attributesTypes A vector of native_t indicating the type of each attribute used for constructing the tree. 
 * @param ModelFilePath A string indicating the directory path where store the weka models 
 * @param NewFilePerModel A bool indicating if the different models have to be stored in different files 
 * @param type A classifier_t indicating the type of classifier implemented
 * @param Log A reference to the logging engine 
 */
WekaID3::WekaID3(vector<string>* attributes, vector<native_t>* attributesTypes,string ModelFilePath,bool NewFilePerModel, Log* log,discretizer_t discretizer,int numberOfBins) : WekaClassifier(attributes,attributesTypes,ModelFilePath,NewFilePerModel,ID3,log,discretizer,numberOfBins)
{
  this->WekaClassificationParameters =  " -p 0 ";
  this->WekaClassificationClass = "weka.jar weka.classifiers.trees.id3" ;
  
  this->WekaModelGenerationParameters = "-C 0.25 -M 2" ;
  this->WekaModelGenerationClass = "weka.jar weka.classifiers.trees.id3";
}



/**
 * The default constructor for the class 
 */
WekaID3::~WekaID3()
{
}

}
