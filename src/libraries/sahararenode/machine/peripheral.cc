#include <unistd.h>
#include <iostream>
#include <fstream>

#include "peripheral.h"
#include "../global.h"



std::string Peripheral::toString(){
    std::vector<std::string> lines;
    lines.push_back(_name);
    lines.push_back("Methodes:");

    for(auto m: _methodes){
        lines.push_back(" -" + m->toString());
    }

    lines.push_back("Properties:");
    for(auto p: _properties){
        lines.push_back(" -" + p->toString());
    }
    return Utils::join(lines, '\n');
}

PeripheralProperty* Peripheral::getProperty(std::string name)
{
    for(auto p : _properties){
        if(p->_name == name)
            return p;
    }
    return nullptr;
}
PeripheralMethod* Peripheral::getMethod(std::string name){
    for(auto m : _methodes){
        if(m->_name == name)
            return m;
    }
    return nullptr;
}

Peripheral* Peripheral::clone(){
    Peripheral* newInst = new Peripheral();

    newInst->_name = _name;
    for(auto p: _properties){
        newInst->_properties.push_back(p->clone());
    }

    for(auto m: _methodes){
        newInst->_methodes.push_back(m->clone());
    }
    return newInst;
}

Peripheral* Peripheral::fromString(std::vector<std::string> lines){    
    std::vector<std::string> tempList;
    std::vector<std::string> tempList1;
    PeripheralProperty* lastPeroperty = nullptr;
    Peripheral newInst;

    bool bStartedMethodes = false;
    bool bStartedProperties = false;
    for(auto line: lines){        
        if(line.find("The following methods are available:", 0) ==0){
            printf("start method\n");
            bStartedMethodes = true;
            continue;
        }else if(line.find("The following properties are available:", 0) ==0){
            bStartedProperties = true;
            bStartedMethodes = false;
            printf("start property\n");
            continue;
        }else if(line.find("Usage:") ==0){
            if(bStartedMethodes)
                bStartedMethodes = false;
            else{
                bStartedProperties = false;
                break;
            }
        }

        if(line.find("- ") != 0 &&  (bStartedProperties ==true && line.find("available for ") != 0)) 
            continue;

        //printf("line=%s\n", line.c_str());
        //parse property
        if(bStartedProperties){
            if(line.find("- ") == 0){
                Utils::split(line, ' ', tempList);
                if(tempList.size() >=3)
                {
                    //printf("add property name=%s, type=%s\n", tempList[2].c_str(), tempList[1].c_str());
                    lastPeroperty = new PeripheralProperty(tempList[2], tempList[1]);
                    newInst._properties.push_back(lastPeroperty);
                }
            }else if(lastPeroperty){
                //second line
                if(line.find("'get' and 'set'") >=0)
                    lastPeroperty->_readOnly = false;
                else
                    lastPeroperty->_readOnly = true; 

                lastPeroperty = nullptr;
            }
        }else if(bStartedMethodes){ //parse method
            //"Void AddSymbol (Range address, String name, Boolean isThumb = False)"
            int paramStartIdx = line.find('(');
            int paramEndIdx = line.find(')', paramStartIdx+1);

            if(paramStartIdx >=0 && paramEndIdx >=0){
                std::string strDeclare = line.substr(0, paramStartIdx);
                std::string strParams = line.substr(paramStartIdx+1, paramEndIdx-paramStartIdx-1);
                //printf("param_str=%s\n", strParams.c_str());

                Utils::split(strDeclare, ' ', tempList);

                if(tempList.size() >=3){
                    PeripheralMethod* method = new PeripheralMethod(tempList[2], tempList[1]);

                    //parse params
                    Utils::split(strParams, ',', tempList);
                    for(auto paramStr : tempList){
                        Utils::split(paramStr, ' ', tempList1);
                        if(tempList1.size() >=2){
                            PeripheralMethod::ParameterDef* nweParam = new PeripheralMethod::ParameterDef(tempList1[1], tempList1[0]);
                            if(tempList1.size() >=4 && tempList1[2] == "="){
                                nweParam->_defValue = tempList1[3];
                            }
                            method->_args.push_back(nweParam);
                        }
                    }
                    newInst._methodes.push_back(method);
                }

            }
        }
    }

    if(newInst._methodes.size() >0 || newInst._properties.size() > 0)
        return newInst.clone();

    return nullptr;
}

