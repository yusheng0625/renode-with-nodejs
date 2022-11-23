#ifndef TELNETCLIENT_H_
#define TELNETCLIENT_H_

#include "libtelnet.h"
#include <pthread.h>
#include <string>
#include <vector>
#include "v-terminal.h"

typedef struct _telnetclient_ctx{
	int socket;
	void* client;
}TELNETCLIENT_CTX, *PTELNETCLIENT_CTX;

class TelnetClient
{
	public:
		TelnetClient();
		~TelnetClient();
	public:
		telnet_t *_telnet;
		int _do_echo;
		TELNETCLIENT_CTX _context;
		pthread_t _thread;
		bool _started;
	private:
		void _input(char *buffer, int size);
		void _send(int sock, const char *buffer, size_t size);
		ssize_t _readSocketData(int sock, unsigned char* buffer, unsigned int read_len);		
		static void _event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data);

	    pthread_mutex_t   _lock_commands;
    	std::vector<std::string> _commands;
		bool _getCommand(char* buffer, size_t len);

	    pthread_mutex_t   _lock_responses;
    	std::vector<std::string> _responses;
		void _pushResponse(std::string input);

		VTerminal* _vTerminal;
	public:
		bool start(int port);
		bool stop();
		static void* threadProc(void* param);
	public:
		void pushCommand(std::string input);
		void getResponse(std::vector<std::string>& outs);
		void clearResponse();
		size_t  getResponseCount();

		VTerminal* getTerminal(){
			return _vTerminal;
		};
};

#endif