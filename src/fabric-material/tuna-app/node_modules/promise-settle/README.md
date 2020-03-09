# promise-settle
[![npm version](https://badge.fury.io/js/promise-settle.svg)](http://badge.fury.io/js/promise-settle)
[![Build Status](https://travis-ci.org/pgaubatz/node-promise-settle.svg?branch=master)](https://travis-ci.org/pgaubatz/node-promise-settle)
[![Coverage Status](https://coveralls.io/repos/pgaubatz/node-promise-settle/badge.svg)](https://coveralls.io/r/pgaubatz/node-promise-settle)
[![Dependency Status](https://david-dm.org/pgaubatz/node-promise-settle.svg)](https://david-dm.org/pgaubatz/node-promise-settle)

settle([...]) returns a Promise that is fulfilled when all items in the array are either fulfilled or rejected.  

## Installation

    npm install --save promise-settle

## Usage
```javascript
var settle = require('promise-settle');

settle([true, Promise.resolve(3), Promise.reject(new Error('error'))])
  .then(function (results) {
    results.forEach(function (result) {
      if (result.isFulfilled()) {
        console.log('Promise is fulfilled', result.value());
      } else {
        console.log('Promise is rejected', result.reason());
      }
    })
  });
```
