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
#include <utils/executecommand.h>
#include <utils/utilities.h>
#include <csignal>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>


namespace Utils {

/**
 * This is the constructor for the file 
 * @param command The command line to be executed (recommended to use the complet path)
 * @param paramas The params that will be provided to the executable
 * @param outputRequired A bool indicating if the output is required
 * @param Log The loggin engine 
 * @param TmpDir The directory where the temporary files will be stored
 */
ExecuteCommand::ExecuteCommand(string command,string params,bool outputRequired,Log* log,string TmpDir)
{
  this->command = command;
  this->params = params;
  this->outputRequired = outputRequired;
  this->log = log;
  this->TemporaryDir = TmpDir;
}


/**
 * The default constructor for the class 
 */
ExecuteCommand::ExecuteCommand()
{
  
}

/**
 * The default constructor for the class
 */
ExecuteCommand::~ExecuteCommand()
{
}

vector<string>* ExecuteCommand::execute()
{
  vector<string>* output;
  
  if(this->outputRequired)
    output = new vector<string>();

  string cmd = command+" "+params;

  /* we create the output file where te output will be dumped */
  int fileseed = getpid();  //one temporary file per process
  string outputFile = this->TemporaryDir+"/MN2SimTMPFIle"+to_string(fileseed);
  
  /* time to execute the command */
  this->log->debug(cmd+" > "+outputFile,3);
  MySystem(string(cmd+" > "+outputFile).c_str());


  if(!outputRequired)
    return NULL;

  std::ifstream execution (outputFile.c_str(),ios::in);
  string line;
  
  if (!execution.is_open())
  {
    log->error("Error while getting the output file "+outputFile + " with the commands " +command+" with params "+this->params);
    return NULL;
  }

  while (! execution.eof())
  {
      
    getline (execution,line);
      
    if(this->outputRequired && line.size() > 0)
      output->push_back(line);    
  }
  
  return output;

}

int ExecuteCommand::MySystem(const char *cmd)
{
    int stat;
    pid_t pid;
    struct sigaction sa, savintr, savequit;
    sigset_t saveblock;
    if (cmd == NULL)
        return(1);
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigemptyset(&savintr.sa_mask);
    sigemptyset(&savequit.sa_mask);
    sigaction(SIGINT, &sa, &savintr);
    sigaction(SIGQUIT, &sa, &savequit);
    sigaddset(&sa.sa_mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sa.sa_mask, &saveblock);
    if ((pid = fork()) == 0) {
        sigaction(SIGINT, &savintr, (struct sigaction *)0);
        sigaction(SIGQUIT, &savequit, (struct sigaction *)0);
        sigprocmask(SIG_SETMASK, &saveblock, (sigset_t *)0);
        execl("/bin/sh", "sh", "-c", cmd, (char *)0);
        _exit(127);
    }
    if (pid == -1) {
        stat = -1; 
    } else {
        while (waitpid(pid, &stat, 0) == -1) {
            if (errno != EINTR){
  stat = -1;
  break;
            }
        }
    }
    sigaction(SIGINT, &savintr, (struct sigaction *)0);
    sigaction(SIGQUIT, &savequit, (struct sigaction *)0);
    sigprocmask(SIG_SETMASK, &saveblock, (sigset_t *)0);
    return(stat);
}

}
