/*
 * global.h
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <map>
#include <math.h>

class Config
{
public:
    Config();
    virtual ~Config();
    bool _verbose = false;
    int  _telnet_port = 1234;
};

#endif /* CONFIG_H_ */
