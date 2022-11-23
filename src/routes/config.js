// Reset handler
'use strict'

let Response = require('../utils/response')

// main route handler
function ConfigHandler(options, req, res) {
  let method = req.method,
    promise

  let SaharaSpice = options.SaharaSpice

  if (method === 'OPTIONS') {
    promise = Response.OK
  } else if (method === 'GET') {
    //
    promise = new Promise(function (resolve, reject) {
      // get all of the config params
      try {
        let dev_config = SaharaSpice.getConfig()
        let dev_devices = SaharaSpice.getDevices()
        let dev_channels = SaharaSpice.getChannels()

        let dev_cfg = {
          config: dev_config,
          devices: dev_devices,
          channels: dev_channels,
        }

        resolve(new Response({ status: 200, body: dev_cfg }))
      } catch (e) {
        resolve(
          new Response({
            status: 500,
            body: { success: false, message: `Internal error: ${e}` },
          })
        )
      }
    })
  } else if (method === 'POST') {
    promise = Response.METHOD_NOT_SUPPORTED
  } else if (method === 'PUT') {
    //
    promise = new Promise(function (resolve, reject) {
      // set each param that we actually have
      let updated = {
        config: false,
        devices: false,
        channels: false,
      }

      try {
        if (req.body.config) {
          let status = SaharaSpice.setConfig(req.body.config)

          if (!status) {
            throw 'Error setting config'
          }

          updated.config = true
        }

        if (req.body.devices) {
          let status = SaharaSpice.setDevices(req.body.devices)

          if (!status) {
            throw 'Error setting devices'
          }

          updated.devices = true
        }

        if (req.body.channels) {
          let status = SaharaSpice.setChannels(req.body.channels)

          if (!status) {
            throw 'Error setting channels'
          }

          updated.channels = true
        }

        resolve(
          new Response({
            status: 200,
            body: { success: true, updated: updated },
          })
        )
      } catch (e) {
        resolve(
          new Response({
            status: 500,
            body: { success: false, message: `Internal error: ${e}` },
          })
        )
      }
    })
  } else if (method === 'DELETE') {
    promise = Response.METHOD_NOT_SUPPORTED
  } else {
    promise = Response.METHOD_NOT_SUPPORTED
  }

  // global catch handler and response sender
  global_response(promise, req, res)
}

// global response
function global_response(promise, req, res) {
  promise
    .then(function (response) {
      res.status(response.status).json(response.body)
    })
    .catch(function (err) {
      if (err instanceof Response) {
        res.status(err.status).json(err.body)
      } else {
        res.status(500).json(err)
      }
    })
}

module.exports = ConfigHandler
