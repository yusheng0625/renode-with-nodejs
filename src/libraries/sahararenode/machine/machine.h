#ifndef machine_H_
#define machine_H_

#include <string>
#include <vector>
#include "peripherals.h"

class Machine{
    public:
        Machine(){
            _peripherals = nullptr;
        };        
        Machine(std::string& name){
            _name = name;
        };        
        ~Machine(){
            if(_peripherals){
                delete _peripherals;
            }
        };
    public:
        std::string _name;
        Peripherals* _peripherals = nullptr;
        Machine* clone(){
            Machine* newInst = new Machine();
            newInst->_name = _name;
            if(_peripherals)
                newInst->_peripherals = _peripherals->clone();
            else
                newInst->_peripherals = nullptr;
            return newInst;
        }
};

#endif
