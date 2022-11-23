#ifndef peripherals_H_
#define peripherals_H_

#include <string>
#include <vector>
#include <string.h>

#include "peripheral.h"

class Peripherals{
    public:
        class Node{
            public:
                Node(std::string name){_name= name;}; 
                ~Node(){
                    for(auto n : _children){
                        delete n;
                    }
                    if(_interface)
                        delete _interface;
                };

                Node* clone(){
                    Node* newInst = new Node(_name);
                    for(auto c: _children){
                        Node* newC = c->clone();
                        newC->_parent = newInst;
                        newInst->_children.push_back(newC);
                    }
                    if(_interface)
                        newInst->_interface = _interface->clone();
                    else
                        newInst->_interface = nullptr;

                    return newInst;
                }
                

                void addChild(Node* c){
                    _children.push_back(c);
                }
            public:
                Peripheral* _interface = nullptr;
                std::string _name;
                Node*       _parent = nullptr;
                std::vector<Node*> _children;
                std::string toString(int depth){
                    std::string res = "";
                    for(int i=0; i<depth; i++)
                        res += "-";
                    res += _name + "\n";

                    for(auto c: _children){
                        res += c->toString(depth + 1);
                    }
                    return res;
                };
                std::string path(){
                    std::string res;
                    if(_parent){
                        res = _parent->path() + "." + _name;
                    }else
                        res = _name;
                    return res;
                }

                Node* findNodeByPath(std::string& p){
                    if(path() == p) 
                        return this;
                    for(auto c: _children){
                        Node* node = c->findNodeByPath(p);
                        if(node) 
                            return node;
                    }
                    return nullptr;
                }

        };

    public:
        Peripherals();
        ~Peripherals();
        Node* _root = nullptr;
        std::string toString();
        Node* getNodeByPath(std::string& path);
        Peripherals* clone(){
            Peripherals* newInst = new Peripherals();
            if(_root)
                newInst->_root = _root->clone();
            else
                newInst->_root = nullptr;
            return newInst;
        }
    public:
        static Peripherals* fromString(std::vector<std::string> lines);
};
#endif
