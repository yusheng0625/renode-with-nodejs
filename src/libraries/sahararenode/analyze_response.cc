#include "analyze_response.h"
#include <string.h>
#include "utils.hh"

const char* _commands[]={
	"alias",             
	"allowPrivates",
	"analyzers",
	"commandFromHistory",
	"createPlatform",
	"currentTime",
	"displayImage",
	"execute",
	"help",
	"history",
	"include",
	"lastLog",
	"log",
	"logFile",
	"logLevel",
	"mach",
	"macro",
	"numbersMode",
	"path",
	"pause",
	"peripherals",
	"python",
	"quit",
	"require",
	"runMacro",
	"set",
	"showAnalyzer",
	"start",
	"string",
	"using",
	"verboseMode",
	"version",
	"watch",
};

AnalyzerResponse::AnalyzerResponse()
{
}

AnalyzerResponse::~AnalyzerResponse()
{
}

bool AnalyzerResponse::_isCommandLine(std::string strLine)
{
	for(auto c : _commands){
		//if(strLine.find(c) == 0)
		if(strLine == c)
		{
			size_t cmdLen = strlen(c);
			if(strLine.length() == cmdLen || 
				(strLine.length() > cmdLen && strLine.at(cmdLen) == ' '))
				return true;
		}
	}
	return false;
}

void AnalyzerResponse::classifyCommandResult(std::vector<std::string>& responses, CommandResultList& outs, std::string& additionalCommand)
{
	outs.clear();
	CommandResult* curCmd = nullptr;
	for(auto line: responses){
		if(_isCommandLine(line) || 
			(additionalCommand.length() >0 && line == additionalCommand)){
			if(curCmd){
				outs._list.push_back(curCmd);
			}
			curCmd = new CommandResult();
			curCmd->_command = line;
		}else if(curCmd != nullptr){
			curCmd->addResponse(line);
		}
	}	

	if(curCmd){
		outs._list.push_back(curCmd);
	}
}
