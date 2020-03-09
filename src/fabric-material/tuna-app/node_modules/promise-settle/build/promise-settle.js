(function(f){if(typeof exports==="object"&&typeof module!=="undefined"){module.exports=f()}else if(typeof define==="function"&&define.amd){define([],f)}else{var g;if(typeof window!=="undefined"){g=window}else if(typeof global!=="undefined"){g=global}else if(typeof self!=="undefined"){g=self}else{g=this}(g.Promise || (g.Promise = {})).settle = f()}})(function(){var define,module,exports;return (function e(t,n,r){function s(o,u){if(!n[o]){if(!t[o]){var a=typeof require=="function"&&require;if(!u&&a)return a(o,!0);if(i)return i(o,!0);var f=new Error("Cannot find module '"+o+"'");throw f.code="MODULE_NOT_FOUND",f}var l=n[o]={exports:{}};t[o][0].call(l.exports,function(e){var n=t[o][1][e];return s(n?n:e)},l,l.exports,e,t,n,r)}return n[o].exports}var i=typeof require=="function"&&require;for(var o=0;o<r.length;o++)s(r[o]);return s})({1:[function(require,module,exports){
'use strict';

module.exports = require('./lib/promise-settle');

},{"./lib/promise-settle":2}],2:[function(require,module,exports){
'use strict';

module.exports = settle;

function settle(promises) {
  return Promise.resolve()
    .then(function () {
      return promises;
    })
    .then(function (promises) {
      if (!Array.isArray(promises)) throw new TypeError('Expected an array of Promises');

      var promiseResults = promises.map(function (promise) {
        return Promise.resolve()
          .then(function () {
            return promise;
          })
          .then(function (result) {
            return promiseResult(result);
          })
          .catch(function (err) {
            return promiseResult(null, err);
          });
      });

      return Promise.all(promiseResults);
    });
}

function promiseResult(result, err) {
  var isFulfilled = (typeof err === 'undefined');
  var value = isFulfilled
    ? returns.bind(result)
    : throws.bind(new Error('Promise is rejected'));

  var isRejected = !isFulfilled;
  var reason = isRejected
    ? returns.bind(err)
    : throws.bind(new Error('Promise is fulfilled'));

  return {
    isFulfilled: returns.bind(isFulfilled),
    isRejected: returns.bind(isRejected),
    value: value,
    reason: reason
  };
}

function returns() {
  return this;
}

function throws() {
  throw this;
}

},{}]},{},[1])(1)
});