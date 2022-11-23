#include "monitor-script.h"
#include <string.h>
#include "utils.hh"

MonitorScript::MonitorScript()
{
	_curMachine = nullptr;
}

MonitorScript::~MonitorScript()
{
	for(auto m: _machines){
		delete m;
	}
}

bool MonitorScript::load(std::vector<std::string>& lines)
{
	for(auto line: lines){
		if(line.find(":name:")==0){
			_name = line.c_str() + 6;
			_name = Utils::trim(_name);
		}else if(line.find(":description:")==0){
			_description = line.c_str() + 13;
			_description = Utils::trim(_description);
		}
	}

	return true;
}