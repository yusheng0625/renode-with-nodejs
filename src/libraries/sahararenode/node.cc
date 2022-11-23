#include <napi.h>
#include <pthread.h>
#include "global.h"
#include "utils.hh"
#include <string.h>
#include <vector>
#include <unistd.h>
#include "i_renode.h"
#include "monitor-script.h"

#include <ngspice/sharedspice.h>
#include <filesystem>

#include "machine/peripheral.h"


/*--------------- *
 * Configuration  *
 * -------------- */
Napi::Boolean setConfig(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 1 || !info[0].IsObject())
  {
    throw Napi::TypeError::New(env, "config object expected");
  }
  auto config = info[0].As<Napi::Object>();

  Global* global = Global::instance();
  global->_config._verbose = config.Get("verbose").As<Napi::Boolean>().Value(); 
  global->_config._telnet_port = config.Get("port").As<Napi::Number>().Int32Value();
  return Napi::Boolean::New(env, true);
}

Napi::Object getConfig(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  Global* global = Global::instance();

  Napi::Object obj = Napi::Object::New(env);
  obj.Set("verbose", global->_config._verbose);
  obj.Set("port", global->_config._telnet_port);
  return obj;
}


Napi::Boolean start(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  //check already started
  Global* global = Global::instance();
  if (global->_started)
    return Napi::Boolean::New(env, false);
 
  bool res = IRenode::instance()->start(global->_config._telnet_port);

  //set global flag
  global->_started = res;
  return Napi::Boolean::New(env, res);
}


Napi::Boolean stop(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  if (!Global::instance()->_started)
    return Napi::Boolean::New(env, false);

  IRenode::instance()->stop();
  Global::instance()->_started = false;
  return Napi::Boolean::New(env, true);
}

Napi::Array getRenodeResponse(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  CommandResultList list;
  IRenode::instance()->getParsedResponse(list);

  //making response list
  Napi::Array res = Napi::Array::New(env, list._list.size());
  for(int i=0; i<list._list.size(); i++)
  {
    auto r = list._list[i];

    Napi::Object cmdResult = Napi::Object::New(env);
    cmdResult.Set("command", r->_command.c_str());
    Napi::Array responses = Napi::Array::New(env, r->_responses.size());
    for(int j=0; j< r->_responses.size(); j++)
    {
      responses[j] = r->_responses[i].c_str();
    }
    cmdResult.Set("response", responses);

    res[i] = cmdResult;
  }
  return res;
}


Napi::Boolean loadScript(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 1 || !info[0].IsArray())
  {
    throw Napi::TypeError::New(env, "1 params expected script lines");
  }

  //collect lines
  std::vector<std::string> lines;

  Napi::Array dat = info[0].As<Napi::Array>();
  for (size_t i = 0; i < dat.Length(); i++)
  {
    std::string line = dat.Get(i).ToString().Utf8Value();
    lines.push_back(line);
  }

  //load script on the renode.
  bool bLoaded = IRenode::instance()->loadScript(lines);
  lines.clear();

  //enumerate peripherals
//IRenode::instance()->enumPeripherals();
  return Napi::Boolean::New(env, bLoaded);
}

Napi::Boolean setActiveMachine(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 1 || !info[0].IsString())
  {
    throw Napi::TypeError::New(env, "1 params expected machine name");
  }
  std::string machine = info[0].ToString().Utf8Value();
  bool bLoaded = IRenode::instance()->setActiveMachine(machine);  
  return Napi::Boolean::New(env, bLoaded);
}

Napi::String getActiveMachine(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  std::string machine = IRenode::instance()->getActiveMachine();  
  return Napi::String::New(env, machine.c_str());
}


Napi::Object getPeripherals(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 1 || !info[0].IsString())
  {
    throw Napi::TypeError::New(env, "1 params expected machine name");
  }

  Napi::Object resObj = Napi::Object::New(env);
  std::string machineName = info[0].ToString().Utf8Value();
  Global* global =  Global::instance();
  if(!global->_currentScript){
    resObj.Set("status", false);
    resObj.Set("message", "any script is not loaed");
  }else{
    Machine* machine = global->_currentScript->findMachine(machineName);
    if(!machine){
      resObj.Set("status", false);
      resObj.Set("message", "can not find the machine");
    }else{
      if(machine->_peripherals){
        resObj.Set("status", true);
        resObj.Set("message", "ok");
        resObj.Set("data", machine->_peripherals->toString());        
      }else if(machine == global->_currentScript->_curMachine){
        //enumerate peripherals
        if(IRenode::instance()->enumPeripherals())
        {
          resObj.Set("status", true);
          resObj.Set("message", "ok");
          resObj.Set("data", machine->_peripherals->toString());
        }else{
          resObj.Set("status", false);
          resObj.Set("message", "enumerate periphers failed!");
        }

        if(machine->_peripherals){
          resObj.Set("status", true);
          resObj.Set("message", "ok");
          resObj.Set("data", machine->_peripherals->toString().c_str());
        }
      }else{
          resObj.Set("status", false);
          resObj.Set("message", "machine is not active and periphers is null!");
      }
    }
  }
  //load script on the renode.
  return resObj;
}


Napi::Object getPeripheral(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() != 2 || !info[0].IsString() || !info[1].IsString())
  {
    throw Napi::TypeError::New(env, "2 params expected machine name, pheriperal name");
  }

  Napi::Object resObj = Napi::Object::New(env);
  std::string machineName = info[0].ToString().Utf8Value();
  std::string periperalPathName = info[1].ToString().Utf8Value();

  Global* global =  Global::instance();
  if(!global->_currentScript){
    resObj.Set("status", false);
    resObj.Set("message", "any script is not loaed");
    return resObj;
  }

  Machine* machine = global->_currentScript->findMachine(machineName);
  if(!machine){
    resObj.Set("status", false);
    resObj.Set("message", "can not find the machine");
    return resObj;
  }

  if(!machine->_peripherals && machine != global->_currentScript->_curMachine){
    resObj.Set("status", false);
    resObj.Set("message", "machine is not active && it dosn't init peripherals");
    return resObj;
  }

  //enumerate peripherals
  if(!machine->_peripherals && !IRenode::instance()->enumPeripherals()){
    resObj.Set("status", false);
    resObj.Set("message", "enumerate periphers failed!");
    return resObj;
  }

  //find peripheral 
  Peripherals::Node* node = machine->_peripherals->_root->findNodeByPath(periperalPathName);
  if(!node){
    resObj.Set("status", false);
    resObj.Set("message", "can not find peripheral from path");
    return resObj;
  }

  //check interface
  if(!node->_interface){
    //enum interface(methods & properties)
    std::string path = node->path();
    node->_interface = IRenode::instance()->enumInterfaceOfPeripheral(path);
  }

  if(!node->_interface){
    resObj.Set("status", false);
    resObj.Set("message", "peripheral interface enumerating failed!");
    return resObj;
  }

  resObj.Set("status", true);
  resObj.Set("message", node->path());
  resObj.Set("data", node->_interface->toString());
  return resObj;
}


Peripherals::Node* checkAndInitPeripheralInterface(std::string& machineName, std::string& periperalPathName, std::string& error){
  //check script
  Global* global =  Global::instance();
  if(!global->_currentScript){
    error = "any script is not loaed";
    return nullptr;
  }

  //check script
  Machine* machine = global->_currentScript->findMachine(machineName);
  if(!machine){
    error = "can not find the machine";
    return nullptr;
  }

  if(machine != global->_currentScript->_curMachine){
    error = "machine is not active";
    return nullptr;
  }

  //enumerate peripherals
  if(!machine->_peripherals && !IRenode::instance()->enumPeripherals()){
    error = "enumerate periphers failed!";
    return nullptr;
  }

  //find peripheral 
  Peripherals::Node* node = machine->_peripherals->_root->findNodeByPath(periperalPathName);
  if(!node){
    error = "can not find peripheral from path";
    return nullptr;
  }

  //check interface
  if(!node->_interface){
    std::string path = node->path();
    node->_interface = IRenode::instance()->enumInterfaceOfPeripheral(path);
  }

  if(!node->_interface){
    error = "enumerate interface error!";
    return nullptr;
  }
  return node;
}


Napi::Object readProperty(const Napi::CallbackInfo &info){
  Napi::Env env = info.Env();
  if (info.Length() != 3 || !info[0].IsString() || !info[1].IsString() || !info[2].IsString())
  {
    throw Napi::TypeError::New(env, "2 params expected machine name, pheriperal name");
  }

  Napi::Object resObj = Napi::Object::New(env);
  std::string machineName = info[0].ToString().Utf8Value();
  std::string periperalPathName = info[1].ToString().Utf8Value();
  std::string propertyName = info[2].ToString().Utf8Value();
  std::string error;

  Peripherals::Node* node = checkAndInitPeripheralInterface(machineName, periperalPathName, error);
  if(node == nullptr){
    resObj.Set("status", false);
    resObj.Set("message", error);
    return resObj;
  }

  //find property
  PeripheralProperty* property = node->_interface->getProperty(propertyName);
  if(property == nullptr){
    resObj.Set("status", false);
    resObj.Set("message", "can not find property " + propertyName);
    return resObj;
  }

  // make command
  std::string command = periperalPathName + " " + property->_name;

  PeripheralProperty::Result propertyResult; 
  error.clear();
  if(!IRenode::instance()->readProperty(command, propertyResult)){
    resObj.Set("status", false);
    resObj.Set("message", "read property failed");
    return resObj;
  }

  resObj.Set("status", true);
  resObj.Set("message", command);
  resObj.Set("data", propertyResult.toString());
  return resObj;
}


Napi::Object setProperty(const Napi::CallbackInfo &info){
  Napi::Env env = info.Env();
  if (info.Length() != 4 || !info[0].IsString() || !info[1].IsString() || !info[2].IsString() || !info[3].IsString())
  {
    throw Napi::TypeError::New(env, "4 params expected machine name, pheriperal name, property, value");
  }

  Napi::Object resObj = Napi::Object::New(env);
  std::string machineName = info[0].ToString().Utf8Value();
  std::string periperalPathName = info[1].ToString().Utf8Value();
  std::string propertyName = info[2].ToString().Utf8Value();
  std::string value = info[3].ToString().Utf8Value();
  std::string error;

  Peripherals::Node* node = checkAndInitPeripheralInterface(machineName, periperalPathName, error);
  if(node == nullptr){
    resObj.Set("status", false);
    resObj.Set("message", error);
    return resObj;
  }

  //find property
  PeripheralProperty* property = node->_interface->getProperty(propertyName);
  if(property == nullptr){
    resObj.Set("status", false);
    resObj.Set("message", "can not find property " + propertyName);
    return resObj;
  }

  // make command
  std::string command = periperalPathName + " " + property->_name + " " + value;

  error.clear();
  if(!IRenode::instance()->setProperty(command, error)){
    resObj.Set("status", false);
    resObj.Set("message", error);
    return resObj;
  }

  resObj.Set("status", true);
  resObj.Set("message", command);
  return resObj;
}


Napi::Object callMethod(const Napi::CallbackInfo &info){
  Napi::Env env = info.Env();
  if (info.Length() != 4 || !info[0].IsString() || !info[1].IsString() || !info[2].IsString() || !info[3].IsArray()){
    throw Napi::TypeError::New(env, "4 params expected machine name, pheriperal name, method, [params]");
  }

  Napi::Object resObj = Napi::Object::New(env);
  std::string machineName = info[0].ToString().Utf8Value();
  std::string periperalPathName = info[1].ToString().Utf8Value();
  std::string methodName = info[2].ToString().Utf8Value();

  //parameters
  std::vector<std::string> params;
  Napi::Array dat = info[3].As<Napi::Array>();
  for (size_t i = 0; i < dat.Length(); i++){
    params.push_back(dat.Get(i).ToString().Utf8Value());
  }

  std::string error;
  Peripherals::Node* node = checkAndInitPeripheralInterface(machineName, periperalPathName, error);
  if(node == nullptr){
    resObj.Set("status", false);
    resObj.Set("message", error);
    return resObj;
  }

  //find method
  PeripheralMethod* method = node->_interface->getMethod(methodName);
  if(method == nullptr){
    resObj.Set("status", false);
    resObj.Set("message", "can not find method " + methodName);
    return resObj;
  }

  //check parameters count
  if(params.size() > method->getParamsCount()){
    resObj.Set("status", false);
    resObj.Set("message", "exceed parameter count");
    return resObj;
  }

  if(params.size() < method->getParamsCount() - method->getDefParamsCount()){
    resObj.Set("status", false);
    resObj.Set("message", "insufficient parameter count");
    return resObj;
  }

  // make command
  std::string command = periperalPathName + " " + method->_name;
  std::string paramsStr = Utils::join(params, ' ');
  if(!paramsStr.empty())
    command = command + " " + paramsStr;

  std::vector<std::string> results;
  bool bRet = IRenode::instance()->callMethod(command, results);

  resObj.Set("status", bRet);
  resObj.Set("message", command);
  resObj.Set("proto", method->toString());

  Napi::Array data = Napi::Array::New(env, results.size());
  for(size_t i=0; i< results.size(); i++){
    data[i] = results[i];
  }
  resObj.Set("data", data);
  return resObj;
}



Napi::Boolean startEmulator(const Napi::CallbackInfo &info){
  Napi::Env env = info.Env();

  IRenode* pIRenode = IRenode::instance();
  if(!pIRenode->isLoadedScript()){
    return Napi::Boolean::New(env, false);
  }

  bool bRet = pIRenode->startEmulator();
  return Napi::Boolean::New(env, bRet);
}


Napi::Boolean pauseEmulator(const Napi::CallbackInfo &info){
  Napi::Env env = info.Env();

  IRenode* pIRenode = IRenode::instance();
  if(!pIRenode->isLoadedScript()){
    return Napi::Boolean::New(env, false);
  }
  if(!pIRenode->isStartedEmulator()){
    return Napi::Boolean::New(env, false);
  }

  bool bRet = pIRenode->pauseEmulator();
  return Napi::Boolean::New(env, bRet);
}


Napi::Boolean test(const Napi::CallbackInfo &info){
  Napi::Env env = info.Env();

  std::vector<std::string> lines;
  lines.push_back("The following methods are available:");  
  lines.push_back("- Void AddSymbol (Range address, String name, Boolean isThumb = False)");
  lines.push_back("- Void AddWatchpointHook (UInt64 address, SysbusAccessWidth width, Access access, String pythonScript)");
  lines.push_back("The following properties are available:");
  lines.push_back("- Endianess Endianess");
  lines.push_back("available for 'get' and 'set'");   

  Peripheral* newInst = Peripheral::fromString(lines);
  if(newInst)
    printf("peripheral=%s\n", newInst->toString().c_str());
  else
    printf("peripheral=NULL\n");
    
  return Napi::Boolean::New(env, true);
}



// export all functions
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "setConfig"),
              Napi::Function::New(env, setConfig));
  exports.Set(Napi::String::New(env, "getConfig"),
              Napi::Function::New(env, getConfig));


  exports.Set(Napi::String::New(env, "getRenodeResponse"),
              Napi::Function::New(env, getRenodeResponse));
  exports.Set(Napi::String::New(env, "start"),
              Napi::Function::New(env, start));
  exports.Set(Napi::String::New(env, "stop"),
              Napi::Function::New(env, stop));

  exports.Set(Napi::String::New(env, "loadScript"),
              Napi::Function::New(env, loadScript));
  exports.Set(Napi::String::New(env, "setActiveMachine"),
              Napi::Function::New(env, setActiveMachine));

  exports.Set(Napi::String::New(env, "getActiveMachine"),
              Napi::Function::New(env, getActiveMachine));
  exports.Set(Napi::String::New(env, "getPeripherals"),
              Napi::Function::New(env, getPeripherals));
  exports.Set(Napi::String::New(env, "getPeripheral"),
              Napi::Function::New(env, getPeripheral));
  exports.Set(Napi::String::New(env, "readProperty"),
              Napi::Function::New(env, readProperty));              
  exports.Set(Napi::String::New(env, "setProperty"),
              Napi::Function::New(env, setProperty));
  exports.Set(Napi::String::New(env, "callMethod"),
              Napi::Function::New(env, callMethod));



  exports.Set(Napi::String::New(env, "startEmulator"),
              Napi::Function::New(env, startEmulator));
  exports.Set(Napi::String::New(env, "pauseEmulator"),
              Napi::Function::New(env, pauseEmulator));

  exports.Set(Napi::String::New(env, "test"),
              Napi::Function::New(env, test));



  // exports.Set(Napi::String::New(env, "setCircuitParameters"),
  //             Napi::Function::New(env, setCircuitParameters));
  // exports.Set(Napi::String::New(env, "runTransient"),
  //             Napi::Function::New(env, runTransient));

  return exports;
}

NODE_API_MODULE(SaharaSpice, Init)
