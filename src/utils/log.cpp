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
#include <utils/log.h>
#include <assert.h>
#include <time.h>
#include <utils/utilities.h>
#include <string>
#include <cerrno>


namespace Utils {

    /***************************
      Constructor and destructors
     ***************************/

    /**
     * The constructor for the class 
     * @param ErrorPath indicates the path of the file where the errors will be stored
     * @param DebugPath indicates the path of the file where the debug messages will be stores
     * @param level Indicates the debug level 
     */
    Log::Log(string _ErrorPath, string _DebugPath, int level, int debug_from, bool debugEnabled, uint64_t* globalTime, bool buffer) {

        this->level = level;
        this->line = 0;
        this->gt = globalTime;
        this->verboseMode = false; //default to false
        this->showErrors = false;
        this->debugEnabled = debugEnabled;
        this->bufferToMemory = buffer;

        if (this->bufferToMemory == false) {
            beginFile(_ErrorPath, _DebugPath);
        }

    }

    /**
     * The default destructor for the class 
     */
    Log::~Log() {
        this->ErrorFile.close();
        this->DebugFile.close();
    }
    /***************************
      Auxiliar functions
     ***************************/

        /**
     * Stores configuration sim in out 
     * @param string The config entry
     */
    void Log::debugConfig(string msg) {
        msg = "[SimulationConfiguration] " + msg;
        debug(msg, 0);
    }
    
    void Log::debugDefault(string msg){
        msg = "[SimulatorDefault] " + msg;
        debug(msg, 0);        
    }

    /**
     * Stores the error 
     * @param error The error to be stored 
     */
    void Log::error(string error) {
        time_t rawtime;

        time(&rawtime);
        char * time = ctime(&rawtime);
        time[24] = ' '; //chomp te \n character  

        this->ErrorFile << "[" << time << "] " << error << endl;

        if (this->showErrors || this->verboseMode)
            std::cout << "[ERROR - " << time << "] " << error << endl;

        this->line++;
    }

    /**
     * Debugs a message with a given level. If the levelmsg is lower than the level set the message will be stored 
     * @param log The debug message 
     * @param levelmsg The level of the message 
     */
    void Log::debug(string log, int levelmsg) {



        if (this->debugEnabled && this->level >= levelmsg) {
            string time = to_string(*gt);

            if (this->verboseMode) {
                //Ok, if we are in verbose mode we don't dump the stuff to the LogFile
                std::cout << "[DEBUG -" << time << " " << this->line << "] " << log << endl;
            }

            if (this->bufferToMemory) {
                this->debugBuf << "[" << time << " (L" + to_string(levelmsg) + ")] " << log << endl;

            } else {
                this->DebugFile << "[" << time << " (L" + to_string(levelmsg) + ")] " << log << endl;
            }

            this->line++;

        }
    }


    /***************************
       Sets and gets functions  
     ***************************/

    /**
     * Returns the current debug level 
     * @return The debug level 
     */
    int Log::getLevel() {
        return level;
    }

    /**
     * Sets the debug level 
     * @param theValue The debug level 
     */
    void Log::setLevel(int& theValue) {
        assert(theValue >= 0);
        level = theValue;
    }

    /**
     * Sets the verbose mode , if true the messages will be shown in the Standard outpu 
     * @return A bool indicating if the verboseMode is on 
     */
    bool Log::getverboseMode() const {
        return verboseMode;
    }

    /**)
     * Sets the debug enabled 
     * @param theValue Debug enabled or not 
     */
    void Log::setdebugEnabled(bool debugEnabled) {
        this->debugEnabled = debugEnabled;
    }

    /**
     * Sets if debug is enabled or not 
     * @return A bool if debug is enabled or not 
     */
    bool Log::getdebugEnabled() const {
        return debugEnabled;
    }

    /**
     * Sets the verbose mode 
     * @param theValue The verbose mode
     */
    void Log::setVerboseMode(bool theValue) {
        verboseMode = theValue;
    }

    void Log::setglobalTimeref(uint64_t *newGT) {
        this->gt = newGT;
    }

    void Log::beginFile(string _EPath, string _DPath) {
        // Do stuff here.

        if (_EPath != "" && _DPath != "") {
            this->ErrorPath = expandFileName(_EPath);
            this->DebugPath = expandFileName(_DPath);
        } else {
            // Do something defaulty.
        }

        cout << "Debug Logfile:\t\t" << this->DebugPath << endl;
        cout << "Error Logfile:\t\t" << this->ErrorPath << endl;

        //opening the log and error files
        //We just replace any env variable by the content it was -- like $EXPERIMENT_ROOT/Configuration.xml by the real value

        boost::filesystem::path dbpath(this->DebugPath.c_str());
        boost::filesystem::path epath(this->ErrorPath.c_str());


        /*
         * Ensure that the paths are correct and exist before trying to open the files below.
         */
        if (!boost::filesystem::exists(dbpath.parent_path())) {
            boost::filesystem::create_directory(dbpath.parent_path());
        }

        if (!boost::filesystem::exists(epath.parent_path())) {
            boost::filesystem::create_directory(epath.parent_path());
        }


        if (this->ErrorPath != "") {
            this->ErrorFile.open(this->ErrorPath.c_str(), ios::trunc);

            if (!this->ErrorFile.is_open()) {
                cout << "Error: The Error file " << this->ErrorPath.c_str() << " has not been openend. Some error has occurred: " << strerror(errno) << endl; //to be modified
                cout << "	    Please check that the file path is correct and all the directories exist" << endl;
                exit(-1);
            }
        }

        if (this->DebugPath != "") {

            this->DebugFile.open(this->DebugPath.c_str(), ios::trunc);

            if (!this->DebugFile.is_open()) {
                cout << "Error: The Log file " << this->DebugPath.c_str() << " has not been openend. Some error has occurred:" << strerror(errno) << endl; //to be modified
                cout << "	    Please check that the file path is correct and all the directories exist" << endl;
                exit(-1);

            }
        }

        this->DebugFile << debugBuf.str();
        this->ErrorFile << errorBuf.str();
        this->bufferToMemory = false;

    }

}