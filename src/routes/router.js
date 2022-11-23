/************************************************************************************
 * This module creates and initializes the Express router to be used by the web server.
 *************************************************************************************/
'use strict'

const _ = require('lodash'),
  Express = require('express'),
  RegisterMiddleware = require('../middleware/middleware')

let InfoHandler = require('./info'),
  ConfigHandler = require('./config'),
  CommandHandler = require('./command')

let routes = [
  {
    name: 'Info',
    path: '/$',
    middleware: ['jsonBody'],
    handler: InfoHandler,
  },
  {
    name: 'Config',
    path: '/config',
    middleware: ['jsonBody'],
    handler: ConfigHandler,
  },
  {
    name: 'Command',
    path: '/command/:command',
    middleware: ['jsonBody'],
    handler: CommandHandler,
  },
]

function InitializeRouter(options) {
  const router = Express.Router()

  _.each(routes, (r) => {
    _.each(r.middleware, (m) => {
      RegisterMiddleware(router, m, r.path)
    })

    RegisterMiddleware(router, 'errorCatch', r.path)

    router.use(
      r.path,
      _.partial(r.handler, {
        SaharaSpice: options.SaharaSpice,
        config: options.config.handlers[r.name],
      })
    )
  })

  return router
}

module.exports = InitializeRouter
