/*********************************************************
* This module defines a generic middleware error function.
**********************************************************/
'use strict';

//check for an error and if present, respond immediately
function MiddlewareErrorCheck (err, req, res, next) {
    if (err) {
        //check for JSON body parser error
        if (err instanceof SyntaxError && err.status === 400 && 'body' in err) {
            return res.status(400).json({message: "An error occurred while trying to parse the request's JSON body!"});
        }
        else {
            return res.status(500).json({message: err});
        }
    }
    else {
        next();
    }
}

module.exports = MiddlewareErrorCheck;