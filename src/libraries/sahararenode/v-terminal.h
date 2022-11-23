#ifndef V_TERMINAL_H_
#define V_TERMINAL_H_

#include "libtelnet.h"
#include <string>
#include <vector>

#define VTERMINAL_BUFFER_SIZE 63999

class VTerminal
{
	public:
		VTerminal(const char* prompt);
		~VTerminal();
	private:
		std::string _prompt;
		bool        _seenPrompt = false;
		int         _seenPrompts = 0;
	public:
		void write(const char *buffer, int size, std::vector<std::string>& outs);
		void clear();
		void setPrompt(const char* prompt);
		bool getSeenPrompt(){
			return _seenPrompt;
		}
		int getSeenPrompts(){
			return _seenPrompts;
		}
	private:
		std::string _filter(std::string& s);
	    char _buffer[VTERMINAL_BUFFER_SIZE + 1];
		int  _position;	
};

#endif