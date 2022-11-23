/* eslint-disable no-undef */

// read .env
require('dotenv').config()

let fs = require('fs'),
  path = require('path')

// pull in config.json

var fname = path.join(__dirname, 'config.json')
var config = JSON.parse(fs.readFileSync(fname))

/* Pull important variables */

// Node port
config.NODE_PORT = process.env.NODE_PORT ? process.env.NODE_PORT : 80

// Access secret (can be null)
config.APP_SECRET = process.env.APP_SECRET

// export
module.exports = config
