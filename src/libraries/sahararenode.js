/*****************************************************************************
 * This module defines methods to create pertinent packets to send across network
 ******************************************************************************/
'use strict'
const _ = require('lodash')
const ds0 = require('../../build/Release/sahararenode.node')

var ds_status = [
  {
    state: 'reset',
    error: '',
    circuit: null,
    memory: {},
  }
]

function getStatus({ i, sahararenode }) {
  return ds_status[i]
}

function getInfo({ i, sahararenode }) {
  return JSON.parse(sahararenode.getInfo())
}

function getStats({ i, sahararenode }) {
  return sahararenode.getStats()
}

/*
JSON Format:
...
*/
function setConfig({ i, sahararenode }, j) {
  let res = sahararenode.setConfig(j)
  if (!res) {
    ds_status[i].state = 'error'
    ds_status[i].error = 'Error setting config'
  }
  return res
}

function getConfig({ i, sahararenode }) {
  return sahararenode.getConfig()
}

// bind to sockets and start devsrv
function start({ i, sahararenode }) {
  let res = sahararenode.start()

  if (!res) {
    ds_status[i].state = 'error'
    ds_status[i].error = 'Error starting devsrv'
  } else {
    ds_status[i].state = 'running'
  }
  return res
}

// stop() then start()
function restart({ i, sahararenode }) {
  let res = false

  res &= stop({ i, sahararenode })
  res &= start({ i, sahararenode })

  if (!res) {
    ds_status[i].state = 'error'
    ds_status[i].error = 'Error restarting devsrv'
  } else {
    ds_status[i].state = 'running'
  }
  return res
}

// stop devsrv and release socket
function stop({ i, sahararenode }) {
  let res = sahararenode.stop()

  if (!res) {
    ds_status[i].state = 'error'
    ds_status[i].error = 'Error stopping devsrv'
  } else {
    ds_status[i].state = 'stopped'
  }

  return res
}

// reset devsrv and release socket
function reset({ i, sahararenode }) {
  let res = sahararenode.stop()

  if (!res) {
    ds_status[i].state = 'error'
    ds_status[i].error = 'Error stopping devsrv'
  } else {
    ds_status[i].state = 'reset'
  }

  return res
}

// reset devsrv and release socket
function getRenodeResponse({ i, sahararenode }) {
  let res = sahararenode.getRenodeResponse()

  if (!res) {
    ds_status[i].state = 'error'
    ds_status[i].error = 'Error stopping devsrv'
  } else {
    ds_status[i].state = 'reset'
  }
  return res
}


function loadScript({ i, sahararenode }, scripts) {
  let res = sahararenode.loadScript(scripts)
  return res
}

function setActiveMachine({ i, sahararenode }, machine){
  let res = sahararenode.setActiveMachine(machine)
  return res
}
function getActiveMachine({ i, sahararenode }){
  let res = sahararenode.getActiveMachine()
  return res
}
function getPeripherals({ i, sahararenode }, machine){
  let res = sahararenode.getPeripherals(machine)
  return res
}

function getPeripheral({ i, sahararenode }, machine, path){
  let res = sahararenode.getPeripheral(machine, path)
  return res
}

function readProperty({ i, sahararenode }, machine, path, prop){
  let res = sahararenode.readProperty(machine, path, prop)
  return res
}

function setProperty({ i, sahararenode }, machine, path, prop, value){
  let res = sahararenode.setProperty(machine, path, prop, value)
  return res
}

function callMethod({ i, sahararenode }, machine, path, prop, params){
  let res = sahararenode.callMethod(machine, path, prop, params)
  return res
}

function startEmulator({ i, sahararenode }) {
  let res = sahararenode.startEmulator()
  return res
}

function pauseEmulator({ i, sahararenode }) {
  let res = sahararenode.pauseEmulator()
  return res
}

function test({ i, sahararenode }) {
  let res = sahararenode.test()
  return res
}


let exp_funcs = [
  getInfo,
  getStats,
  setConfig,
  getConfig,
  start,
  restart,
  stop,
  reset,
  getStatus,
  getRenodeResponse,  
  loadScript,
  setActiveMachine,
  getActiveMachine,
  getPeripherals,
  getPeripheral,
  readProperty,
  setProperty,
  callMethod,

  startEmulator,
  pauseEmulator,
  test,
]

module.exports = (instance_num = 0) => {
  let inst

  switch (instance_num) {
    case 0:
      inst = ds0
      break
    case 1:
      inst = ds1
      break
    case 2:
      inst = ds2
      break
    case 3:
      inst = ds3
      break
    default:
      inst = ds0
  }

  let to_exp = {}
  _.each(exp_funcs, (f) => {
    to_exp[f.name] = _.partial(f, { i: instance_num, sahararenode: inst })
  })

  to_exp.instance_num = instance_num
  return to_exp
}
