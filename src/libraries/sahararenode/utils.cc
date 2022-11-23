/*
 * utils.cc
 *
 */

#include "utils.hh"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <algorithm>
#include <sstream>

Utils::Utils() {


}

Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

std::string& Utils::ltrim(std::string &s)
{
    auto it = std::find_if(s.begin(), s.end(),
                    [](char c) {
                        return !std::isspace(c);
                    });
    s.erase(s.begin(), it);
    return s;
}
 
std::string& Utils::rtrim(std::string &s)
{
    auto it = std::find_if(s.rbegin(), s.rend(),
                    [](char c) {
                        return !std::isspace(c);
                    });
    s.erase(it.base(), s.end());
    return s;
}
 
std::string& Utils::trim(std::string &s) {
    return ltrim(rtrim(s));	
}

void Utils::split(std::string &s, char c, std::vector<std::string>& out)
{
    out.clear();

	std::stringstream ss(s);
	std::string line;
	while(std::getline(ss, line, c)){
		std::string line1 = trim(line);
		if(line1.length() > 0)
			out.push_back(line1);
	}	
}

std::string Utils::join(std::vector<std::string> arr, char c)
{
	std::string res;
	for(auto s : arr)
	{
		if(res.length() > 0)
			res += c;
		res += s;
	}
	return res;
}

std::string Utils::join(std::vector<std::string> arr, std::string cc)
{
	std::string res;
	for(auto s : arr)
	{
		if(res.length() > 0)
			res += cc;
		res += s;
	}
	return res;
}

int Utils::substr_count(std::string &s, std::string& s1)
{
    int res = 0, idx = 0;
    while((idx = s.find(s1.c_str(), idx)) >=0){
        res += 1;        
        idx += s1.length();
    }
    return res;
}


uint64_t Utils::get_timestamp_ms()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)(ts.tv_nsec / 1000000) + ((uint64_t)ts.tv_sec * 1000ull);
}

