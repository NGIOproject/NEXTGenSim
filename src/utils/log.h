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
#ifndef SIMULATORLOG_H
#define SIMULATORLOG_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace std;


namespace Utils {

    /**
     * @author Francesc Guim,C6-E201,93 401 16 50,
     */

    /** Class for storage of errors and debug messages*/
    class Log {
    public:
        //Constructors
        Log(string ErrorPath, string DebugPath, int level, int debug_from, bool debugEnabled, uint64_t* globalTime, bool buffer);
        ~Log();

        //Auxiliar methods
        void error(string error);
        void debugConfig(string log);
        void debugDefault(string log);
        void debug(string log, int levelmsg = 1);

        //set and gets methods
        void setLevel(int& theValue);
        int getLevel();
        void setVerboseMode(bool theValue);
        bool getverboseMode() const;
        void setdebugEnabled(bool theValue);
        bool getdebugEnabled() const;
        void setglobalTimeref(uint64_t *);
        
        void beginFile(string ErrorPath, string DebugPath);
        


    private:
        //Variables that indicate the paths stuff
        string ErrorPath; /**<indicates the path of the file where the errors will be stored */
        string DebugPath; /**<indicates the path of the file where the logs will be stored */
        int level; /**<Indicates the debug level , the errors are always stored. Logs are displayed if the level is greater than 1 */

        boost::filesystem::ofstream ErrorFile; /**< Stream for the error file */
        boost::filesystem::ofstream DebugFile; /**< Stream for the log file  */

        bool verboseMode = false; /**< all the stuff is shown in the STDIO*/
        bool showErrors; /**< all the error stuff is shown in the STDIO*/
        bool debugEnabled; /**< debug is enabled */
        int line; /**< The current simulation line in the debug file */
        uint64_t* gt; // Pointer to globaltime.
        bool bufferToMemory = true; // Do we want to buffer to memory rather than a file.
        
        std::ostringstream errorBuf;
        std::ostringstream debugBuf;
    };

}

#endif
