/*****************************************************************************
 * This module defines objects to be used for HTTP-compliant responses
 ******************************************************************************/
'use strict'

let _ = require('lodash')

class Response {
  constructor(options) {
    this.status = options.status
    this.body = options.body
  }
}

Response.OK = new Promise(function (resolve, reject) {
  resolve(new Response({ status: 200, body: { success: true } }))
})

Response.METHOD_NOT_SUPPORTED = new Promise(function (resolve, reject) {
  resolve(
    new Response({
      status: 405,
      body: { success: false, error: 'Method not supported!' },
    })
  )
})

module.exports = Response
