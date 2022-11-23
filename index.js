/*****************************************************************************
 * Project: Sahara DevSrv Library v1.0.0
 * Description: This is the library that enables devsrv access through NodeJS
 ******************************************************************************/

'use strict'

const InitializeRouter = require('./src/routes/router')
const SaharaRenodeFactory = require('./src/libraries/sahararenode')

module.exports = { SaharaRenodeFactory, InitializeRouter }
