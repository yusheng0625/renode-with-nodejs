/*
 * global.h
 */

#ifndef SRC_GLOBAL_H_
#define SRC_GLOBAL_H_

#include <map>
#include <math.h>
#include <string>
#include "config.h"
#include "monitor-script.h"

class Global
{
public:
    Global();
    virtual ~Global();
    bool _started;
    Config _config;
    MonitorScript* _currentScript = nullptr;
    void setCurrentScript(MonitorScript* newScript){
        if(_currentScript)
            delete _currentScript;
        _currentScript = newScript;
    }
    
protected:
    static Global *m_inst;

public:
    static Global *instance();
};

#endif /* SRC_GLOBAL_H_ */
