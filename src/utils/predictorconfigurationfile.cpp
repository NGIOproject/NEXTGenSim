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
#include <utils/predictorconfigurationfile.h>

#include <utils/utilities.h>
#include<sstream>

namespace Utils {

/**
 * Function that initialize the variables for the predictor information 
 */
void PredictorConfigurationFile::init()
{  
  postDeadlineManager = OTHER_DM_MANAGER;
  preDeadlineManager = OTHER_DM_MANAGER;
  predictorType = OTHER_PREDICTOR;
  this->fileLoaded = false;
  this->classifierType = OTHER_CLASSIFIER;
  this->numberOfBins = -1;
  this->ResponseJobAttributes = 0;
  this->NewFilePerModel = false;
  this->PredictionVariable = "";
  this->ModelDirectoryFile = "";
  this->ModelIntervalGeneration = 0;
  this->discretizer = OTHER_DISCRETIZER;
  
}

/**
 * The constructor for the class 
 * @param ConfigFile The file path of the configuration file for the predictor 
 * @return 
 */
PredictorConfigurationFile::PredictorConfigurationFile(string ConfigFile)
{
    this->init();
    this->ConfigFile = expandFileName(ConfigFile);

    //Init XML2 Lib Parser
    xmlInitParser();
    //Parse the Xml document
    this->doc = xmlParseFile(this->ConfigFile.c_str());
    
    std::cout << "Loading the predictor configuration file "<< this->ConfigFile << endl;

    //UserRuntime
    string UserRuntime= getStringFromXPathExpression("/PredictorConfiguration/UserRuntime",this->doc,true);
    cout << "[PredictorConfiguration] UserRuntime= " << UserRuntime<< endl; 
    if(UserRuntime!= "") this->predictorType = USER_RUNTIME_ESTIMATE;
    

    //HistoricalClassifier
    xmlNodePtr HistoricalClassifier= getNodeFromXPathExpression("/PredictorConfiguration/HistoricalClassifier",this->doc,true);
    if(HistoricalClassifier != NULL)
    { 
        this->predictorType = HISTORICAL_CLASSIFIER;
        extractHistoricalClassifierFields(HistoricalClassifier);
    }

    //PreDeadlineMissManager
    string preDeadlineManager_s = getStringFromXPathExpression("/PredictorConfiguration/Common/PreDeadlineMissManager",this->doc,true);
    cout << "[PredictorConfiguration] PreDeadlineMissManager= " << preDeadlineManager_s<< endl; 
    this->preDeadlineManager = this->getDeadLineManager(preDeadlineManager_s);

    //PostDeadlineMissManager
    string postDeadlineManager_s = getStringFromXPathExpression("/PredictorConfiguration/Common/PostDeadlineMissManager",this->doc,true);
    cout << "[PredictorConfiguration] PostDeadlineMissManager= " << postDeadlineManager_s<< endl; 
    this->postDeadlineManager = this->getDeadLineManager(postDeadlineManager_s);

    //ArrivalFactor
    ModelIntervalGeneration= atof((getStringFromXPathExpression("/PredictorConfiguration/Common/ModelIntervalGeneration",this->doc)).c_str());
    if(ModelIntervalGeneration == 0) ModelIntervalGeneration= 100;
    cout << "[SimulationConfiguration] ModelIntervalGeneration= " << ftos(ModelIntervalGeneration) << endl; 
}



/**
 * The destrcutor for the class
 */
PredictorConfigurationFile::~PredictorConfigurationFile()
{
}

/**
 * Returns the classifier type contained in the string 
 * @param strClassifier The string containing the classifier type 
 * @return A classifier_t containing the classifier type indicated in the string 
 */
classifier_t PredictorConfigurationFile::getClassifierType(string strClassifier)
{
  if(strClassifier == "ID3")
    return ID3;
  else if(strClassifier == "C45")
    return C45;
  else
  {
    return OTHER_CLASSIFIER;
  }

}

/**
 * Returns the discretizer type contained in the string 
 * @param strClassifier The string containing the discretizer type 
 * @return A discretizer_t containing the discretizer type indicated in the string 
 */
discretizer_t PredictorConfigurationFile::getDiscretizerType(string strDiscretizer)
{
  if(strDiscretizer == "SAME_INTERVAL_DISCRETIZER")
    return SAME_INTERVAL_DISCRETIZER;
  else if(strDiscretizer == "SAME_INSTANCES_PER_INTERVAL_DISCRETIZER")
    return SAME_INSTANCES_PER_INTERVAL_DISCRETIZER;
  else
  {
    return OTHER_DISCRETIZER;
  }
}

/**
 * Returns the deadline manager  type contained in the string 
 * @param strClassifier The string containing the deadline manager type 
 * @return A DeadlineMissManager_t containing the deadline manager type indicated in the string 
 */
DeadlineMissManager_t PredictorConfigurationFile::getDeadLineManager(string strManager)
{
  if(strManager == "GRADUAL")
    return GRADUAL;
  else if(strManager == "FACTOR")
    return FACTOR;
  else
  {
    return OTHER_DM_MANAGER;
  }
}


/**
 * Function that given a XML node reference extracts the EASY information 
 * @param xmlNodePtr The context node   
*/
void PredictorConfigurationFile::extractHistoricalClassifierFields(xmlNodePtr classifier)
{
    xmlDocPtr classifierDoc = (xmlDocPtr) classifier;

    string classifierType_s = getStringFromXPathExpression("/classifierType",classifierDoc);
    cout << "[PredictorConfigurationFile] classifierType_s= " << classifierType_s<< endl; 
    this->classifierType = getClassifierType(classifierType_s);  

    string DiscretizerType_s = getStringFromXPathExpression("/DiscretizerType",classifierDoc);
    cout << "[PredictorConfigurationFile] DiscretizerType= " << DiscretizerType_s<< endl; 
    this->discretizer = getDiscretizerType(DiscretizerType_s);  

    //numberOfBins
    numberOfBins= atoi((getStringFromXPathExpression("/NumberOfBins",classifierDoc)).c_str());
    if(numberOfBins== 0) numberOfBins= 1;
    cout << "[PredictorConfigurationFile] NumberOfBins = " << to_string(numberOfBins) << endl; 

    //PredictionVariable
    PredictionVariable= getStringFromXPathExpression("/PredictionVariable",classifierDoc);
    cout << "[PredictorConfigurationFile] PredictionVariable= " << PredictionVariable<< endl; 

    //ModelDirectoryFile
    ModelDirectoryFile= getStringFromXPathExpression("/ModelDirectoryFile",classifierDoc);
    cout << "[PredictorConfigurationFile] ModelDirectoryFile= " << ModelDirectoryFile<< endl; 

    //Getting the  NewFilePerModel 
    string NewFilePerModel_s = getStringFromXPathExpression("/NewFilePerModel",classifierDoc);
    cout << "[PredictorConfigurationFile] NewFilePerModel= " << NewFilePerModel_s<< endl; 

    if(NewFilePerModel_s.compare("TRUE") == 0)  
        this->NewFilePerModel = true;
    else 
        this->NewFilePerModel = false;
          
    //ModelIntervalGeneration
    ModelIntervalGeneration= atoi((getStringFromXPathExpression("/ModelIntervalGeneration",classifierDoc)).c_str());
    if(numberOfBins== 0) numberOfBins= 1;
    cout << "[PredictorConfigurationFile] ModelIntervalGeneration= " << to_string(ModelIntervalGeneration) << endl; 

    xmlNodeSetPtr variables = getNodeSetFromXPathExpression("/ResponseVariables/Variable",this->doc);

    for(int i = 0; variables&& i < variables->nodeNr;i++)
    {
        xmlDocPtr variableDoc = (xmlDocPtr) variables->nodeTab[i];

        //ModelDirectoryFile
        string AttName= getStringFromXPathExpression("/Name",variableDoc);
        cout << "[PredictorConfigurationFile] Name= " << AttName<< endl; 
        ResponseJobAttributes->push_back(AttName);
    }
    xmlXPathFreeNodeSet(variables);
}
}
