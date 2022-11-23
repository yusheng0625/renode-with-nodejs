/*****************************************************************************
 * This module defines middleware to be used for any of the web service routes.
 ******************************************************************************/
'use strict'

const bodyParser = require('body-parser')
const cookieParser = require('cookie-parser')
const authCheck = require('./authorization')
const errorCatch = require('./errorCatch')
const _ = require('lodash')

const Middleware = {
  jsonBody: bodyParser.json(),
  cookies: cookieParser(),
  queryString: bodyParser.urlencoded({ extended: false }),
  authCheck,
  dummyAuth,
  errorCatch,
}

// define a dummy middleware when auth is disabled
// to apply necessary changes to req
function dummyAuth(req, res, next) {
  req.authPayload = { user: { userId: 'somerandomid' }, token: 'doesntmatter' }
  next()
}

function RegisterMiddleware(router, middleware, route, params) {
  if (Middleware[middleware]) {
    const target =
      params == null
        ? Middleware[middleware]
        : _.partial(Middleware[middleware], params)
    router.use(route, target)
  }
}

module.exports = RegisterMiddleware
