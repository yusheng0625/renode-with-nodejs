#ifndef ANALYZERRESPONSE_H_
#define ANALYZERRESPONSE_H_

#include <string>
#include <vector>

class CommandResult{
	public:
		CommandResult(){};
		~CommandResult(){};
	public:
		std::string _command;
		std::vector<std::string> _responses;
	public:
		void addResponse(std::string strline){
			_responses.push_back(strline);
		}
};

class CommandResultList{
	public:
		CommandResultList(){};
		~CommandResultList(){
			clear();
		};
	public:
		std::vector<CommandResult*> _list;
	public:
		void clear(){
			for(auto a : _list){
				delete a;
			}
			_list.clear();
		}
		CommandResult* get(const char* cmd){
			for(auto a : _list){
				if(a->_command.find(cmd) == 0)
				{
					return a;
				}
			}
			return nullptr;
		}
};


class AnalyzerResponse
{
	public:
		AnalyzerResponse();
		~AnalyzerResponse();
	public:
		void classifyCommandResult(std::vector<std::string>& responses, CommandResultList& outs, std::string& additionalCommand);
	private:
		bool _isCommandLine(std::string strLine);
};

#endif