#ifndef peripheral_H_
#define peripheral_H_

#include <string>
#include <vector>
#include "../utils.hh"

class PeripheralProperty{
    public:
        class Result{
            public:
                std::string _value;
                std::vector<std::string> _possibles;
            public:
                bool _valid = false;
                bool fromString(std::vector<std::string>& lines);
            public:
                std::string toString();
        };

    public:
        PeripheralProperty(){};
        PeripheralProperty(std::string& name, std::string& type){
            _name= name;
            _type = type;
        };
        ~PeripheralProperty(){};
        std::string _type;
        std::string _name;
        bool _readOnly;
    public:
        std::string toString(){
            std::string res = _type + " " + _name;
            if(_readOnly)
                res += " ReadOnly";
            return res;
        };
        PeripheralProperty* clone(){
            PeripheralProperty* newInst = new PeripheralProperty();
            newInst->_type = _type;
            newInst->_name = _name;
            newInst->_readOnly = _readOnly;
            return newInst;
        }
};

class PeripheralMethod{
    public:
        class ParameterDef{
            public: 
                ParameterDef(){};
                ParameterDef(std::string& name, std::string& type){
                    _name = name;
                    _type = type;
                };
                ~ParameterDef(){};
                std::string _type;
                std::string _name;
                std::string _defValue;
                bool hasDefValue(){
                    return !_defValue.empty();
                }
            public:
                std::string toString(){
                    std::string res = _type + " " + _name;
                    if(!_defValue.empty())
                        res += " = " + _defValue;
                    return res;
                };
                ParameterDef* clone(){
                    ParameterDef* newInst = new ParameterDef();
                    newInst->_type = _type;
                    newInst->_name = _name;
                    newInst->_defValue = _defValue;
                    return newInst;
                }
        };

        PeripheralMethod(){};
        PeripheralMethod(std::string& name, std::string type){
            _name = name;
            _type = type;
        };
        ~PeripheralMethod(){
            for(auto a : _args){
                delete a;
            }
            _args.clear();
        };
        std::string _name;
        std::string _type;
        std::vector<ParameterDef*> _args;
        size_t getParamsCount(){
            return _args.size();
        }
        size_t getDefParamsCount(){
            size_t res = 0;
            for(auto a : _args){
                if(a->hasDefValue())
                    res++;
            }
            return res;
        }
        void getDefParameterValues(std::vector<std::string>& out, size_t limit = 0){
            out.clear();
            size_t paramsCount = getParamsCount();
            if(limit > paramsCount)
                limit = paramsCount;
            size_t iStart = paramsCount - limit;
            for(size_t i=iStart; i<paramsCount; i++)
            {
                out.push_back(_args[i]->_defValue);
            }
        }
    public:
        std::string toString(){
            std::vector<std::string> params;
            for(auto a: _args){
                params.push_back(a->toString());
            }
            std::string res = _type + " " + _name + "(" + Utils::join(params, ", ") + ");";
            return res;
        };
        PeripheralMethod* clone(){
            PeripheralMethod* newInst = new PeripheralMethod();
            newInst->_type = _type;
            newInst->_name = _name;
            for(auto a: _args){
                newInst->_args.push_back(a->clone());
            }
            return newInst;
        }
};


class Peripheral{
    public:
        Peripheral(){};
        ~Peripheral(){
            //remove properties
            for (auto p: _properties){
                delete p;
            }
            _properties.clear();

            //remove methodes
            for (auto m: _methodes){
                delete m;
            }
            _methodes.clear();
        };

        std::string _name;
        std::vector<PeripheralProperty*> _properties;
        std::vector<PeripheralMethod*> _methodes;

    public:
        PeripheralProperty* getProperty(std::string name);
        PeripheralMethod* getMethod(std::string name);
    public:
        std::string toString();
        Peripheral* clone();

    public:
        static Peripheral* fromString(std::vector<std::string> lines); 
};


#endif
