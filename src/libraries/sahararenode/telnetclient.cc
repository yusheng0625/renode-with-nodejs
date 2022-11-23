#if !defined(_BSD_SOURCE)
#	define _BSD_SOURCE
#endif


#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <sstream> 

#include "telnetclient.h"
#include "global.h"

static const telnet_telopt_t telopts[] = {
	//{ TELNET_TELOPT_ECHO,		TELNET_WONT, TELNET_DO   },
	{ TELNET_TELOPT_ECHO,		TELNET_WONT, TELNET_DONT },	
	{ TELNET_TELOPT_TTYPE,		TELNET_WILL, TELNET_DONT },
	{ TELNET_TELOPT_COMPRESS2,	TELNET_WONT, TELNET_DO   },
	{ TELNET_TELOPT_MSSP,		TELNET_WONT, TELNET_DO   },
	{ -1, 0, 0 }
};

TelnetClient::TelnetClient()
{
	_started = false;
	_lock_commands = PTHREAD_MUTEX_INITIALIZER;
	_lock_responses = PTHREAD_MUTEX_INITIALIZER;
	_vTerminal= new VTerminal("(monitor)");
}
TelnetClient::~TelnetClient()
{
	delete _vTerminal;
}

void TelnetClient::_input(char *buffer, int size) {
	static char crlf[] = { '\r', '\n' };
	int i;

	for (i = 0; i != size; ++i) {
		/* if we got a CR or LF, replace with CRLF
		 * NOTE that usually you'd get a CR in UNIX, but in raw
		 * mode we get LF instead (not sure why)
		 */
		if (buffer[i] == '\r' || buffer[i] == '\n') {
			telnet_send(_telnet, crlf, 2);
		} else {
			telnet_send(_telnet, buffer + i, 1);
		}
	}
}

void TelnetClient::_send(int sock, const char *buffer, size_t size) {
	int rs;

	/* send data */
	while (size > 0) {
		if ((rs = send(sock, buffer, size, 0)) == -1) {
			printf("send() failed: %s\n", strerror(errno));
			break;
		} else if (rs == 0) {
			printf("send() unexpectedly returned 0\n");
			break;
		}

		/* update pointer and size to see if we've got more to send */
		buffer += rs;
		size -= rs;
	}
}

void TelnetClient::_event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data) {
	PTELNETCLIENT_CTX context = (PTELNETCLIENT_CTX)user_data;
	int sock = context->socket;
	TelnetClient* client = (TelnetClient*)context->client;
	std::string response;
	Config* config = &Global::instance()->_config;
	char buffer[2048];
	std::vector<std::string> res;

	switch (ev->type) {
	/* data received */
	case TELNET_EV_DATA:
		if (ev->data.size)
		{
			if(config->_verbose)
			{
				//fwrite(ev->data.buffer, 1, ev->data.size, stdout);
				//std::cout << ev->data.buffer << std::endl;
				//printf("response from renode=%s, len=%d\n", ev->data.buffer, ev->data.size);
			}
			client->_vTerminal->write(ev->data.buffer, ev->data.size, res);
			for(auto s: res){
				client->_pushResponse(s);
			}
		}
		break;
	/* data must be sent */
	case TELNET_EV_SEND:
		client->_send(sock, ev->data.buffer, ev->data.size);
		break;
	/* request to enable remote feature (or receipt) */
	case TELNET_EV_WILL:
		/* we'll agree to turn off our echo if server wants us to stop */
		if (ev->neg.telopt == TELNET_TELOPT_ECHO)
			client->_do_echo = 0;
		break;
	/* notification of disabling remote feature (or receipt) */
	case TELNET_EV_WONT:
		if (ev->neg.telopt == TELNET_TELOPT_ECHO)
			client->_do_echo = 1;
		break;
	/* request to enable local feature (or receipt) */
	case TELNET_EV_DO:
		break;
	/* demand to disable local feature (or receipt) */
	case TELNET_EV_DONT:
		break;
	/* respond to TTYPE commands */
	case TELNET_EV_TTYPE:
		/* respond with our terminal type, if requested */
		if (ev->ttype.cmd == TELNET_TTYPE_SEND) {
			telnet_ttype_is(telnet, nullptr);
			//telnet_ttype_is(telnet, getenv("TERM"));
		}
		break;
	/* respond to particular subnegotiations */
	case TELNET_EV_SUBNEGOTIATION:
		break;
	/* error */
	case TELNET_EV_ERROR:
		printf("ERROR: %s\n", ev->error.msg);
		//exit(1);
	default:
		/* ignore */
		break;
	}
}

ssize_t TelnetClient::_readSocketData(int sock, unsigned char* buffer, unsigned int read_len)
{
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(sock, &rset);
    struct timeval read_timeout = {0, 1}; //5 microsecond timeout

    int nready = select(sock+1, &rset, NULL, NULL, &read_timeout);
    if (nready < 0) {
        //throw SocketException("UDP Socket select error!");
		return 0;
    }

    if(!nready)
        return 0;

    if (FD_ISSET(sock, &rset)) {
        ssize_t count = recv(sock, buffer, read_len, 0);
        if (count == -1) {
            //throw SocketException("UDP Socket read error!");
			return 0;
        }
        return count;
    }
    return 0;
}

void* TelnetClient::threadProc(void* param)
{
	TelnetClient* pThis = (TelnetClient*) param;
	int rs;
	char buffer[2048];
	char bufferRecv[2048];
	int sock = pThis->_context.socket;
	Config* config = &Global::instance()->_config;
	if(config->_verbose){
		printf("starting TelnetClient::threadProc\n");
	}

	/* loop while both connections are open */
	while(pThis->_started == true)
	{
		//usleep(100000);		
		//try get commands.		
		if(pThis->_getCommand(buffer, 2048)){
			if(config->_verbose){
				printf("send command to renode %s\n", buffer);
			}
			pThis->_input(buffer, strlen(buffer));


			if(strstr(buffer, "quit") == buffer){
				usleep(300000);
				break;
			}
		}

		//try get data from server
		if((rs = pThis->_readSocketData(sock, (unsigned char*)bufferRecv, 2048)))
		{
			telnet_recv(pThis->_telnet, bufferRecv, rs);
		}
	}

	//close socket
	shutdown(sock, SHUT_RD);
	pThis->_context.socket = -1;
	return param;
}

bool TelnetClient::start(int port)
{		
	int sock;
	int rs;
	struct sockaddr_in addr;
	struct addrinfo *ai;
	struct addrinfo hints;
	Config* config = &Global::instance()->_config;

	/* process arguments */
	char servname[16];
	const char* hostname = "127.0.0.1";
	sprintf(servname, "%d", port);

	/* look up server host */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if ((rs = getaddrinfo(hostname, servname, &hints, &ai)) != 0) {
		if(config->_verbose){
			printf("getaddrinfo() failed for %s: %s\n", hostname, gai_strerror(rs));
		}
		return false;
	}
	
	/* create server socket */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		if(config->_verbose){
			printf("socket() failed: %s\n", strerror(errno));
		}
		return false;
	}

	/* bind server socket */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		if(config->_verbose){
			printf("bind() failed: %s\n", strerror(errno));
		}
		close(sock);
		return false;
	}

	/* connect */
	if (connect(sock, ai->ai_addr, ai->ai_addrlen) == -1) {
		if(config->_verbose){
			printf("connect() failed: %s\n", strerror(errno));
		}
		close(sock);
		return false;
	}

	/* free address lookup info */
	freeaddrinfo(ai);

	/* set input echoing on by default */
	_do_echo = 0;

	/* initialize telnet box */
	_context.socket = sock;
	_context.client = this;
	_telnet = telnet_init(telopts, _event_handler, 0, &_context);

	_started = true;
	pthread_create(&_thread, NULL, threadProc, this);
	return true;
}

bool TelnetClient::stop()
{
	if(!_started)
		return false;
	pushCommand("quit\n");	
	usleep(200000);
	_started = false;

	telnet_free(_telnet);
	_telnet = nullptr;
	
	return true;
}

bool TelnetClient::_getCommand(char* buffer, size_t len)
{
	bool bRes = false;
	buffer[0] = 0;
	pthread_mutex_lock(&_lock_commands);
	if(_commands.size() > 0)
	{
		bRes = true;
		strncpy(buffer, _commands[0].c_str(), len);
		_commands.erase(_commands.begin());
	}
	pthread_mutex_unlock(&_lock_commands);	
	return bRes;
}

void TelnetClient::pushCommand(std::string input)
{
	pthread_mutex_lock(&_lock_commands);
	_commands.push_back(input);
	pthread_mutex_unlock(&_lock_commands);
}

void TelnetClient::clearResponse(){
	pthread_mutex_lock(&_lock_responses);
	_responses.clear();
	pthread_mutex_unlock(&_lock_responses);	
}
size_t TelnetClient::getResponseCount()
{
	size_t count = 0;
	pthread_mutex_lock(&_lock_responses);
	count = _responses.size();
	pthread_mutex_unlock(&_lock_responses);	
	return count;
}

void TelnetClient::getResponse(std::vector<std::string>& outs)
{
	outs.clear();
	pthread_mutex_lock(&_lock_responses);
	for(auto s : _responses){
		outs.push_back(s);
	}
	_responses.clear();
	pthread_mutex_unlock(&_lock_responses);
}
void TelnetClient::_pushResponse(std::string input)
{
	pthread_mutex_lock(&_lock_responses);
	_responses.push_back(input);
	pthread_mutex_unlock(&_lock_responses);
}

