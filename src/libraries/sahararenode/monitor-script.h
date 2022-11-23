#ifndef MONITOR_SCRIPT_H_
#define MONITOR_SCRIPT_H_

#include <string>
#include <vector>
#include "machine/machine.h"

class MonitorScript
{
	public:
		MonitorScript();
		~MonitorScript();
	public:
		std::string _name;
		std::string _description;
		std::vector<Machine*> _machines;
		Machine*    _curMachine = nullptr;
	public:
		bool load(std::vector<std::string>& lines);
		MonitorScript* clone(){
			MonitorScript* newInst = new MonitorScript();
			newInst->_name = _name;
			newInst->_description = _description;			

			for(auto m: _machines){
				newInst->_machines.push_back(m->clone());
				if(m == _curMachine){
					newInst->_curMachine = newInst->_machines[newInst->_machines.size() - 1];
				}
			}
			return newInst;
		}
		Machine* findMachine(std::string& machine){
			for(auto m: _machines){
				if(m->_name == machine){
					return m;
				}
			}
			return nullptr;
		}
};

#endif