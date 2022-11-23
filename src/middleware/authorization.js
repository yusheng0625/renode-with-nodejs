/*********************************************************************************************
 * This module defines utility functions to check the Authorization header for valid JWTs.
 **********************************************************************************************/
'use strict';

const _ = require('lodash');
const base64 = require('base-64');
const crypto = require('crypto');

function MiddlewareAuthCheck(appSecret, req, res, next) {
  if (req.method === 'OPTIONS') {
    next();
  } else {
    AuthorizeRequest(req, appSecret)
      .then((payload) => {
        payload.token = quickTokenGet(req); // graft token on for pass-through
        req.authPayload = payload;
        next();
      })
      .catch((err) => {
        return res.status(403).json({
          message: err
        });
      });
  }
}

function AuthorizeRequest(req, appSecret) {
  const promise = GetToken(req)
    .then(function(token) {
      return UnpackToken(token);
    })
    .then(function(token) {
      return CheckAuthToken(token, req, appSecret);
    });

  return promise;
}

//helper methods
function GetToken(req) {
  if ('headers' in req && 'authorization' in req.headers) {
    return new Promise(function(resolve, reject) {
      resolve(req.headers.authorization);
    });
  } else {
    return new Promise(function(resolve, reject) {
      reject("The request is missing an Authorization header!");
    });
  }
}

function UnpackToken(token) {
  const initialSplit = token.split(" ");
  if (initialSplit.length !== 2 || initialSplit[0] !== "Bearer") {
    return new Promise(function(resolve, reject) {
      reject("Invalid bearer token!");
    });
  }

  const token_parts = initialSplit[1].split(".");

  if (token_parts.length !== 3) {
    return new Promise(function(resolve, reject) {
      reject("Invalid JWT - must have exactly 3 parts!");
    });
  } else {
    return new Promise(function(resolve, reject) {
      resolve({
        header: token_parts[0],
        payload: token_parts[1],
        signature: token_parts[2]
      });
    });
  }
}

function CheckAuthToken(token, req, appSecret) {
  try {
    const toHash = (token.header + '.' + token.payload);
    const signature = crypto.createHmac('sha256', appSecret).update(toHash).digest('hex');
    const output = base64.encode(signature);
  } catch (err) {
    return new Promise(function(resolve, reject) {
      reject("Invalid JWT - signature validation failed!");
    });
  }

  if (output != token.signature) {
    return new Promise(function(resolve, reject) {
      reject("Invalid JWT - signature validation failed!");
    });
  } else {
    const payload = JSON.parse(base64.decode(token.payload));
    const exp_date = new Date(payload.exp);
    const current = new Date(Date.now());

    if (current > exp_date) {
      return new Promise(function(resolve, reject) {
        reject("Invalid JWT - token expired!");
      });
    } else {
      return new Promise(function(resolve, reject) {
        resolve(payload);
      });
    }
  }
}

// helper method to quickly extract a token that we know is already valid and present
function quickTokenGet(req) {
  return req.headers.authorization.split(" ")[1];
}

module.exports = MiddlewareAuthCheck;
