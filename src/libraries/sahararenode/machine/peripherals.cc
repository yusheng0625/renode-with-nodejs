#include <unistd.h>
#include <algorithm>

#include "peripherals.h"
#include "../global.h"
#include "../utils.hh"


bool PeripheralProperty::Result::fromString(std::vector<std::string>& lines)
{
    _valid = false;
    _possibles.clear();

    if(lines.size() > 0){
        _value = lines[0];
        _valid = true;
    }

    bool startedPossibles = false;
    for(size_t i=1; i<lines.size(); i++){
        if(lines[i].find("Possible values are:") == 0){
            startedPossibles = true;
            continue;
        }
        if(startedPossibles && lines[i].length() >0){
            _possibles.push_back(lines[i]);
        }
    }
    return _valid;
}

std::string PeripheralProperty::Result::toString(){
    std::string res = _value;
    if(_possibles.size() > 0){
        res += "\nPossible values are:\n";
        for(auto p: _possibles){
            res += "  " + p + "\n";
        }
    }
    return res;
}


Peripherals::Peripherals()
{
    _root = nullptr;
}
Peripherals::~Peripherals()
{

}

Peripherals::Node* Peripherals::getNodeByPath(std::string& path){
    return _root->findNodeByPath(path);
}

Peripherals* Peripherals::fromString(std::vector<std::string> lines){   

    std::vector<std::string> tempList;
    Peripherals* newInst = new Peripherals();

    //parse noeds
    std::string  mark = "├── ";
    std::string  mark1 = "└── ";
    Node* curNode = nullptr;
    int   curTreeLevel = 0;

    for(auto line: lines){
        if(line.find("sysbus")==0 && newInst->_root == nullptr)        
        {            
            Utils::split(line, ' ', tempList);            
            newInst->_root = new Node(tempList[0]);
            curNode = newInst->_root;
            curTreeLevel = 0;
            continue;
        }

        int idx = line.find(mark.c_str());
        if(idx < 0)
            idx = line.find(mark1.c_str());
        if(idx < 0)
            continue;

        //calc tree level
        std::string prefix = line;
        std::string treeLine = "│   ";
        prefix.erase(idx);
        
        int newTreeLevel = Utils::substr_count(prefix, treeLine);
        newTreeLevel ++;

        Node* parent = nullptr;
        if(newTreeLevel == curTreeLevel)
            parent = curNode->_parent;
        else if(newTreeLevel == curTreeLevel+1)
            parent = curNode;
        else if(newTreeLevel < curTreeLevel){
            for(int i=0; i< curTreeLevel - newTreeLevel; i++){
                curNode = curNode->_parent;
                parent = curNode->_parent;
            }
        }else{
            printf("parsing peripheral tree error \n");
            return nullptr;
        }
        
        std::string newLine = line.data() + idx + mark.length();//line.erase(line.begin(), line.begin() + idx + 4);
        Utils::split(newLine, ' ', tempList);
        Node* newNode = new Node(tempList[0]);
        newNode->_parent = parent;
        parent->addChild(newNode);

        curNode = newNode;
        curTreeLevel = newTreeLevel;
    }
    return newInst;
}

std::string Peripherals::toString()
{
    return _root->toString(0);
}