#include "v-terminal.h"
#include <string.h>
#include "utils.hh"

VTerminal::VTerminal(const char * prompt)
{
	_prompt = prompt;
	clear();
}

VTerminal::~VTerminal()
{

}
void VTerminal::clear(){
	_position = 0;
	_buffer[0] = 0;
	_seenPrompts = 0;
}

void VTerminal::setPrompt(const char* prompt)
{
	_prompt = prompt;
	_seenPrompt = false;
	_seenPrompts = 0;
}

std::string VTerminal::_filter(std::string& s)
{
	int index = 0;		
	while( (index = s.find("[33;1m")) >= 0)
	{		
		s.erase(s.begin() + index, s.begin() + index + 6);
	}
	while( (index = s.find("[;032m")) >= 0)
	{
		s.erase(s.begin() + index, s.begin() + index + 6);
	}
	while( (index = s.find("[31;1m")) >= 0)
	{
		s.erase(s.begin() + index, s.begin() + index + 6);
	}
	while( (index = s.find("[;031m")) >= 0)
	{
		s.erase(s.begin() + index, s.begin() + index + 6);
	}
	while( (index = s.find("[0m")) >= 0)
	{
		s.erase(s.begin() + index, s.begin() + index + 3);
	}
	return s;
}

void VTerminal::write(const char* buffer, int size, std::vector<std::string>& outs)
{
	outs.clear();
	//check buffer remove none char
	for(int i=0; i<size; i++)
	{
		u_char ch = buffer[i];
		if(ch == '\n' || !iscntrl(ch))
		{
			if(_position >= VTERMINAL_BUFFER_SIZE)
				break;
			_buffer[_position] = ch;
			_position++;
		}
	}
	_buffer[_position] = 0;

	std::vector<std::string> datas;
	//check if detected prompt
	char* pstr = nullptr;
	do{
		pstr = strstr(_buffer, _prompt.c_str());
		if(pstr){
			_seenPrompts ++;

			int count  = pstr - _buffer;
			int remain = _position - count - _prompt.length();
			*pstr = 0;

			std::string s = _buffer;
			std::string ss = Utils::trim(s);
			ss = _filter(ss);

			datas.clear();
			Utils::split(ss, '\n', datas);
			for(auto line: datas){
				if(line.length() > 0)
					outs.push_back(line);
			}
			// if(ss.length() > 0)
			// 	outs.push_back(ss);
			if(!_seenPrompt)
				_seenPrompt = true;

			if(remain > 0)
			{
				memmove(_buffer, (pstr + _prompt.length()), remain);
				_position = remain;
				_buffer[_position] = 0;
			}else{
				_position = 0;
				_buffer[_position] = 0;
			}
		}

	}while(pstr != nullptr);

	//printf("%s\n", _buffer); 
}

