// Reset handler
'use strict'

let Response = require('../utils/response')

// main route handler
function CommandHandler(options, req, res) {
  let method = req.method,
    promise

  let SaharaSpice = options.SaharaSpice

  if (method === 'OPTIONS') {
    promise = Response.OK
  } else if (method === 'GET') {
    promise = Response.METHOD_NOT_SUPPORTED
  } else if (method === 'POST') {
    promise = new Promise(function (resolve, reject) {
      // get command
      let status
      let command = req.params.command

      try {
        switch (command) {
          case 'start':
            status = SaharaSpice.start()
            resolve(new Response({ status: 200, body: { result: status } }))
            break

          case 'restart':
            status = SaharaSpice.restart()
            resolve(new Response({ status: 200, body: { result: status } }))
            break

          case 'stop':
            status = SaharaSpice.stop()
            resolve(new Response({ status: 200, body: { result: status } }))
            break

          default:
            throw 'No such command'
        }
      } catch (e) {
        resolve(
          new Response({
            status: 500,
            body: { success: false, message: e },
          })
        )
      }
    })
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

module.exports = CommandHandler
