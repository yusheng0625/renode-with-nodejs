// Reset handler
'use strict'

let Response = require('../utils/response')

// main route handler
function InfoHandler(options, req, res) {
  let method = req.method,
    promise

  let SaharaSpice = options.SaharaSpice

  if (method === 'OPTIONS') {
    promise = Response.OK
  } else if (method === 'GET') {
    //
    promise = new Promise(function (resolve, reject) {
      //let devsrv_status = SaharaSpice.status() // return status
      let devsrv_status = { message: 'No info available yet...' }

      if (devsrv_status) {
        resolve(new Response({ status: 200, body: devsrv_status }))
      } else {
        resolve(
          new Response({
            status: 500,
            body: { success: false, message: 'Internal error' },
          })
        )
      }
    })
  } else if (method === 'POST') {
    promise = Response.METHOD_NOT_SUPPORTED
  } else if (method === 'PUT') {
    promise = Response.METHOD_NOT_SUPPORTED
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

module.exports = InfoHandler
