#include <unistd.h>
#include <iostream>
#include <fstream>

#include "i_renode.h"
#include "global.h"
#include "monitor-script.h"
#include "utils.hh"
#include "machine/peripherals.h"

IRenode* IRenode::m_s = nullptr;

IRenode::IRenode()
{
    _isLoadedScript = false;
    _isStartedEmulator = false;
    _telnet = new TelnetClient();
}
IRenode::~IRenode()
{    
}
bool IRenode::start(int port)
{
    Config* config = &Global::instance()->_config;    
    char command[256];

    //start renode as daemon
    if (fork() == 0)
    {
        if(config->_verbose)
        {
            printf("starting renode as background mode port=%d\n", port);
        }        
        sprintf(command, "%d", port);
        execl("/usr/bin/renode", "--disable-xwt", "--hide-monitor", "--hide-analyzers", "--hide-log", "--port", command, (char *)0);
    }
    else
    {
        usleep(3000000);
        _telnet->start(port);

        usleep(2000000);
        //send "version" command
        _telnet->pushCommand("\n");
        _telnet->pushCommand("version\n");
        usleep(4000000);
    }
    return true;
}

bool IRenode::stop()
{
    return _telnet->stop();
}

void IRenode::getResponse(std::vector<std::string>& outs)
{
    _telnet->getResponse(outs);
}

void IRenode::getParsedResponse(CommandResultList& resp, std::string addtionalCommand)
{
    std::vector<std::string> outs;    
    AnalyzerResponse analyzer;
    _telnet->getResponse(outs);
    analyzer.classifyCommandResult(outs, resp, addtionalCommand);
}

void IRenode::sendCommand(std::string cmd)
{
    _telnet->pushCommand(cmd);
}

bool IRenode::loadScript(std::vector<std::string>& scriptLines)
{    
    Config* config = &Global::instance()->_config;

    //first parsing 
    MonitorScript monitorScript;
    monitorScript.load(scriptLines);
    if(config->_verbose){
        printf("name=%s\n", monitorScript._name.c_str());
        printf("description=%s\n", monitorScript._description.c_str());
    }

    char fileName[256];
    sprintf(fileName, "/tmp/sahara-renode-%ld.resc", Utils::get_timestamp_ms());

    std::ofstream file(fileName);
    if (file.is_open()){
        for(auto line : scriptLines){
            file << line.c_str() << "\n";
        }
        file.close();
    }else{
        printf("Create temp file error \n", fileName);
        return false;
    }

    if(config->_verbose){  
        printf("Create temp file =%s\n", fileName);
        printf("Loading script..\n");
    }

    //first clear vTerminal and reset prompt
    _telnet->getTerminal()->clear();
    std::string newPrompt = "(" + monitorScript._name + ")";
    _telnet->getTerminal()->setPrompt(newPrompt.c_str());

    //send command
    char command[1024];
    sprintf(command, "include @%s\n", fileName);
    _telnet->pushCommand(command);

    //waiting we get response from renoed, while checking new prompt.
    uint timeOut = 20 * 1000 * 1000; // 20 seconds
    uint checkingPeriod = 20 * 1000; //20 ms
    uint checkedCount = 0;
    uint timeOutCount =  timeOut / checkingPeriod;
    bool bSuccess = false;
    while(checkedCount < timeOutCount){
        usleep(checkingPeriod);
        if(_telnet->getTerminal()->getSeenPrompt()){
            bSuccess = true;
            break;
        }
        checkedCount ++;
    }
    
    //delete temp file
    std::remove(fileName);

    if(bSuccess){
        monitorScript._curMachine = new Machine(monitorScript._name);
        monitorScript._machines.push_back(monitorScript._curMachine);

        MonitorScript* newScript = monitorScript.clone();
        Global::instance()->setCurrentScript(newScript);
    }else{
        printf("load script error timeout!\n");
    }
    _isLoadedScript = bSuccess;

    return bSuccess;
}

bool IRenode::setActiveMachine(std::string& machine){
    Global* global =  Global::instance();

    if(global->_currentScript==nullptr)
    {
        printf("global->_currentScript\n");
        return false;
    }

    if(global->_currentScript->_curMachine == nullptr){
        printf("global->_currentScript->_curMachine\n");
        return false;
    }
    if(global->_currentScript->_curMachine->_name == machine){
        //already setteled
        return true;
    }
    if(!global->_currentScript->findMachine(machine)){
        //can't find machine
        printf("can't find machine\n");
        return false;
    }

    //clear terminal 
    _telnet->getTerminal()->clear();
    std::string newPrompt = "(" + machine + ")";
    _telnet->getTerminal()->setPrompt(newPrompt.c_str());

    //send command
    std::string command = "mach set " + machine;
    _telnet->pushCommand(command);

    //waiting we get response from renoed, while checking new prompt.
    uint timeOut = 20 * 1000 * 1000; // 20 seconds
    uint checkingPeriod = 20 * 1000; //20 ms
    uint checkedCount = 0;
    uint timeOutCount =  timeOut / checkingPeriod;
    bool bSuccess = false;
    while(checkedCount < timeOutCount){
        usleep(checkingPeriod);
        if(_telnet->getTerminal()->getSeenPrompt()){
            bSuccess = true;
            break;
        }
        checkedCount ++;
    }

    return bSuccess;
}

std::string IRenode::getActiveMachine()
{
    std::string res = "";
    Global* global = Global::instance();
    if(global->_currentScript && global->_currentScript->_curMachine){
        res = global->_currentScript->_curMachine->_name;
    }
    return res;
}

bool IRenode::startEmulator()
{
    Config* config = &Global::instance()->_config;

    //clear response queue
    _telnet->getTerminal()->clear();
    _telnet->clearResponse();    

    //start 
    _telnet->pushCommand("start\n");

    //wait response  max 5 seconds
    uint checkingCount = 0;
    while(checkingCount < 5000){
        usleep(10 * 1000);
        if(_telnet->getResponseCount() >= 3 ){
            break;
        }
        checkingCount++;
    }

    //check reponse
    CommandResultList list;
    getParsedResponse(list);

    _isStartedEmulator = false;
    CommandResult* resp = list.get("start");
    if(resp && resp->_responses.size() > 0 && resp->_responses[0].find("Starting emulation...")==0){
        _isStartedEmulator = true;
    }else if(config->_verbose){
        if(resp){
            printf("commmand = %s\n", resp->_command.c_str());
            for(auto s: resp->_responses){
                printf("    res= %s\n", s.c_str());
            }
        }else{
            printf("ersponse = null\n");
        }
    }
    return  _isStartedEmulator;
}

bool IRenode::pauseEmulator()
{
    Config* config = &Global::instance()->_config;

    //clear response queue
    _telnet->getTerminal()->clear();
    _telnet->clearResponse();    


    _telnet->pushCommand("pause\n");

    //wait response  max 5 seconds
    uint checkingCount = 0;
    while(checkingCount < 5000){
        usleep(10 * 1000);
        if(_telnet->getResponseCount() >= 3 ){
            break;
        }
        checkingCount++;
    }

    //check reponse
    CommandResultList list;
    getParsedResponse(list);

    CommandResult* resp = list.get("pause");
    if(resp && resp->_responses.size() > 0 && resp->_responses[0].find("Pausing emulation...")==0){
        _isStartedEmulator = false;
    }else if(config->_verbose){
        if(resp){
            printf("commmand = %s\n", resp->_command.c_str());
            for(auto s: resp->_responses){
                printf("    res= %s\n", s.c_str());
            }
        }else{
            printf("ersponse = null\n");
        }
    }
    return  _isStartedEmulator != true;


}

bool IRenode::enumPeripherals(){
   
    Config* config = &Global::instance()->_config;

    //clear response queue
    VTerminal* terminal = _telnet->getTerminal();
    terminal->clear();
    _telnet->clearResponse();

    //start 
    _telnet->pushCommand("peripherals\n");

    //wait response  max 5 seconds
    uint checkingCount = 0;
    while(checkingCount < 5000){
        usleep(10 * 1000);
        if(terminal->getSeenPrompts() >= 2 ){
            break;
        }
        checkingCount++;
    }

    //check reponse
    CommandResultList list;
    getParsedResponse(list);

    CommandResult* resp = list.get("peripherals");
    if(resp){
        Peripherals* peripherals = Peripherals::fromString(resp->_responses);
        Global::instance()->_currentScript->_curMachine->_peripherals = peripherals;
    }
    return  true;
}



Peripheral* IRenode::enumInterfaceOfPeripheral(std::string& path){
     Config* config = &Global::instance()->_config;

    //clear response queue
    VTerminal* terminal = _telnet->getTerminal();
    terminal->clear();
    _telnet->clearResponse();

    //start 
    _telnet->pushCommand(path + "\n");

    //wait response  max 5 seconds
    uint checkingCount = 0;
    while(checkingCount < 5000){
        usleep(10 * 1000);
        if(terminal->getSeenPrompts() >= 2 ){
            break;
        }
        checkingCount++;
    }

    //check reponse
    CommandResultList list;
    getParsedResponse(list, path);

    CommandResult* resp = list.get(path.c_str());
    if(resp){
        Peripheral* peripheral = Peripheral::fromString(resp->_responses);
        return peripheral;
    }
    return nullptr;   
}


bool IRenode::readProperty(std::string& propertyCommand, PeripheralProperty::Result& out){
    out._valid = false;

    Config* config = &Global::instance()->_config;
    //clear response queue
    VTerminal* terminal = _telnet->getTerminal();
    terminal->clear();
    _telnet->clearResponse();

    //start 
    _telnet->pushCommand(propertyCommand + "\n");

    //wait response  max 5 seconds
    uint checkingCount = 0;
    while(checkingCount < 5000){
        usleep(10 * 1000);
        if(terminal->getSeenPrompts() >= 2 ){
            break;
        }
        checkingCount++;
    }

    //check reponse
    CommandResultList list;
    getParsedResponse(list, propertyCommand);

    CommandResult* resp = list.get(propertyCommand.c_str());
    if(resp){
        out.fromString(resp->_responses);
    }
    
    return out._valid;
}

bool IRenode::setProperty(std::string& propertyCommand, std::string& error){

    Config* config = &Global::instance()->_config;
    //clear response queue
    VTerminal* terminal = _telnet->getTerminal();
    terminal->clear();
    _telnet->clearResponse();

    //start 
    _telnet->pushCommand(propertyCommand + "\n");

    //wait response  max 5 seconds
    uint checkingCount = 0;
    while(checkingCount < 5000){
        usleep(10 * 1000);
        if(terminal->getSeenPrompts() >= 2 ){
            break;
        }
        checkingCount++;
    }


    //check reponse
    CommandResultList list;
    getParsedResponse(list, propertyCommand);

    CommandResult* resp = list.get(propertyCommand.c_str());
    if(resp){
        if(resp->_responses.size() == 0)
            return true;
        else{
            error = Utils::join(resp->_responses, '\n');
        }
    }    
    return false;
}

bool IRenode::callMethod(std::string& methodCommand, std::vector<std::string>& outs){

    Config* config = &Global::instance()->_config;
    //clear response queue
    VTerminal* terminal = _telnet->getTerminal();
    terminal->clear();
    _telnet->clearResponse();

    //start
    _telnet->pushCommand(methodCommand + "\n");

    //wait response  max 5 seconds
    uint checkingCount = 0;
    while(checkingCount < 5000){
        usleep(10 * 1000);
        if(terminal->getSeenPrompts() >= 2 ){
            break;
        }
        checkingCount++;
    }
    

    //check reponse
    CommandResultList list;
    getParsedResponse(list, methodCommand);

    CommandResult* resp = list.get(methodCommand.c_str());
    if(resp){
        outs.clear();
        for(auto l: resp->_responses){            
            outs.push_back(l);
        }
        return true;
    }    
    return false;
}

