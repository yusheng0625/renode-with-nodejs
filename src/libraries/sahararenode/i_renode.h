#ifndef I_RENODE_H_
#define I_RENODE_H_

#include <string>
#include <vector>
#include "telnetclient.h"
#include "analyze_response.h"
#include "machine/peripheral.h"
class IRenode{
    public:
        static IRenode* m_s;
        static IRenode* instance(){
            if(m_s==nullptr){
                m_s = new IRenode();
            }
            return m_s;
        }
        IRenode();
        ~IRenode();        
    private:
        TelnetClient* _telnet;
        bool _isLoadedScript;
        bool _isStartedEmulator;
    public:
        bool start(int port);
        bool stop();
        void getResponse(std::vector<std::string>& outs);
        void getParsedResponse(CommandResultList& resp, std::string addtionalCommand = "");
        void sendCommand(std::string cmd);
        bool loadScript(std::vector<std::string>& scriptLines);
        bool isLoadedScript(){
            return _isLoadedScript;
        }
        bool setActiveMachine(std::string& machine);
        std::string getActiveMachine();

        bool startEmulator();
        bool pauseEmulator();
        bool enumPeripherals();
        Peripheral* enumInterfaceOfPeripheral(std::string& path);
        bool readProperty(std::string& propertyCommand, PeripheralProperty::Result& out);
        bool setProperty(std::string& propertyCommand, std::string& error);
        bool callMethod(std::string& methodCommand, std::vector<std::string>& outs);

        bool isStartedEmulator(){
            return _isStartedEmulator;
        }
};
#endif
