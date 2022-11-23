/*
 * HexDump.h
 */

#ifndef MISC_HEXDUMP_H_
#define MISC_HEXDUMP_H_

#include "log.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <stdio.h>

void dump(unsigned char* data, int len);

class HexDump {
public:
	HexDump();
	virtual ~HexDump();
	static void dump(unsigned char* data, int len)
	{
		std::string strLine = "";
		int nLines = len / 16;
		if(len % 16)
			nLines++;
		for(int iLine=0; iLine<nLines; iLine++)
		{
			int nCount = std::min(len - iLine * 16, 16);
			strLine = "";
		    std::stringstream ss;
		    ss << std::hex;
		    //address
		    ss << std::setw(4) << std::setfill('0') << (iLine * 16) << "   ";

		    for (int i=0; i<16; i++) {

		    	if(i < nCount)
		    	{
			    	unsigned char b = data[iLine* 16 + i];
			        ss << std::setw(2) << std::setfill('0') << (int)b << " ";

			        if(isgraph(b))
			        	strLine = strLine + (char)b;
			        else
			        	strLine = strLine + '.';
		    	}
		    	else
		    	{
		    		ss << "   ";
		    	}
		    }
		    Log::log_bare(Log::Levels::log_info, "%s\t%s", ss.str().c_str(), strLine.c_str());
		}
	};

	static std::string dump_to_string(unsigned char* b, unsigned int len) {
	    std::stringstream ss;
	    ss << std::hex;
	    for (unsigned int i=0; i<len; i++) {
	        ss << std::setw(2) << std::setfill('0') << (int)b[i];
	    }
	    return ss.str();
	}
};


#endif /* MISC_HEXDUMP_H_ */
