# PKCS11js

[![license](https://img.shields.io/badge/license-MIT-green.svg?style=flat)](https://raw.githubusercontent.com/PeculiarVentures/graphene/master/LICENSE)
[![Build Status](https://travis-ci.org/PeculiarVentures/pkcs11js.svg?branch=master)](https://travis-ci.org/PeculiarVentures/pkcs11js)
[![Coverage Status](https://coveralls.io/repos/github/PeculiarVentures/pkcs11js/badge.svg?branch=master)](https://coveralls.io/github/PeculiarVentures/pkcs11js?branch=master)
[![npm version](https://badge.fury.io/js/pkcs11js.svg)](https://badge.fury.io/js/pkcs11js)

[![NPM](https://nodei.co/npm-dl/pkcs11js.png?months=2&height=2)](https://nodei.co/npm/pkcs11js/)


We make a package called [Graphene](https://github.com/PeculiarVentures/graphene), it provides a simplistic Object Oriented interface for interacting with PKCS#11 devices, for most people this is the right level to build on. In some cases you may want to interact directly with the PKCS#11 API, if so PKCS11js is the package for you.

PKCS#11 (also known as CryptoKI or PKCS11) is the standard interface for interacting with hardware crypto devices such as Smart Cards and Hardware Security Modules (HSMs). 

This was developed to the PKCS#11 2.3 specification, the 2.4 headers were not availible at the time we created this, it should be easy enough to extend it for the new version at a later date.

It has been tested with :
- [SoftHSM2](https://www.opendnssec.org/softhsm/)
- [Thales NShield](https://www.thales-esecurity.com/products-and-services/products-and-services/hardware-security-modules/general-purpose-hsms/nshield-solo)
- [Safenet Luna HSMs](http://www.safenet-inc.com/)
- [RuToken](http://www.rutoken.ru/)

**NOTE:** For testing purposes it may be easier to work with SoftHSM2 which is a software implementation of PKCS#11 based on OpenSSL or Botan.


## Installation

```
$ npm install pkcs11js
```

### Install SoftHSM2

- For OSX see the [instructions here](https://github.com/opendnssec/SoftHSMv2/blob/develop/OSX-NOTES.md)
- For linux [instructions here](https://github.com/opendnssec/SoftHSMv2/blob/develop/README.md)


## Examples

### Example #1

```javascript
var pkcs11js = require("pkcs11js");

var pkcs11 = new pkcs11js.PKCS11();
pkcs11.load("/usr/local/lib/softhsm/libsofthsm2.so");

pkcs11.C_Initialize();

try {
    // Getting info about PKCS11 Module
    var module_info = pkcs11.C_GetInfo();

    // Getting list of slots
    var slots = pkcs11.C_GetSlotList(true);
    var slot = slots[0];

    // Getting info about slot
    var slot_info = pkcs11.C_GetSlotInfo(slot);
    // Getting info about token
    var token_info = pkcs11.C_GetTokenInfo(slot);

    // Getting info about Mechanism
    var mechs = pkcs11.C_GetMechanismList(slot);
    var mech_info = pkcs11.C_GetMechanismInfo(slot, mechs[0]);

    var session = pkcs11.C_OpenSession(slot, pkcs11js.CKF_RW_SESSION | pkcs11js.CKF_SERIAL_SESSION);

    // Getting info about Session
    var info = pkcs11.C_GetSessionInfo(session);
    pkcs11.C_Login(session, 1, "password");

    /**
    * Your app code here
    */
    
    pkcs11.C_Logout(session);
    pkcs11.C_CloseSession(session);
}
catch(e){
    console.error(e);
}
finally {
    pkcs11.C_Finalize();
}
```

### Example #2

Generating secret key using AES mechanism

```javascript
var template = [
    { type: pkcs11js.CKA_CLASS, value: pkcs11js.CKO_SECRET_KEY },
    { type: pkcs11js.CKA_TOKEN, value: false },
    { type: pkcs11js.CKA_LABEL, value: "My AES Key" },
    { type: pkcs11js.CKA_VALUE_LEN, value: 256 / 8 },
    { type: pkcs11js.CKA_ENCRYPT, value: true },
    { type: pkcs11js.CKA_DECRYPT, value: true },
];
var key = pkcs11.C_GenerateKey(session, { mechanism: pkcs11js.CKM_AES_KEY_GEN }, template);
```

### Example #3

Generating key pair using RSA-PKCS1 mechanism

```javascript
var publicKeyTemplate = [
    { type: pkcs11js.CKA_CLASS, value: pkcs11js.CKO_PUBLIC_KEY },
    { type: pkcs11js.CKA_TOKEN, value: false },
    { type: pkcs11js.CKA_LABEL, value: "My RSA Public Key" },
    { type: pkcs11js.CKA_PUBLIC_EXPONENT, value: new Buffer([1, 0, 1]) },
    { type: pkcs11js.CKA_MODULUS_BITS, value: 2048 },
    { type: pkcs11js.CKA_VERIFY, value: true }
];
var privateKeyTemplate = [
    { type: pkcs11js.CKA_CLASS, value: pkcs11js.CKO_PRIVATE_KEY },
    { type: pkcs11js.CKA_TOKEN, value: false },
    { type: pkcs11js.CKA_LABEL, value: "My RSA Private Key" },
    { type: pkcs11js.CKA_SIGN, value: true },
];
var keys = pkcs11.C_GenerateKeyPair(session, { mechanism: pkcs11js.CKM_RSA_PKCS_KEY_PAIR_GEN }, publicKeyTemplate, privateKeyTemplate);
```

### Example #4

Generating key pair using ECDSA mechanism

```javascript
var publicKeyTemplate = [
    { type: pkcs11js.CKA_CLASS, value: pkcs11js.CKO_PUBLIC_KEY },
    { type: pkcs11js.CKA_TOKEN, value: false },
    { type: pkcs11js.CKA_LABEL, value: "My EC Public Key" },
    { type: pkcs11js.CKA_EC_PARAMS, value: new Buffer("06082A8648CE3D030107", "hex") }, // secp256r1
];
var privateKeyTemplate = [
    { type: pkcs11js.CKA_CLASS, value: pkcs11js.CKO_PRIVATE_KEY },
    { type: pkcs11js.CKA_TOKEN, value: false },
    { type: pkcs11js.CKA_LABEL, value: "My EC Private Key" },
    { type: pkcs11js.CKA_DERIVE, value: true },
];
var keys = pkcs11.C_GenerateKeyPair(session, { mechanism: pkcs11js.CKM_EC_KEY_PAIR_GEN }, publicKeyTemplate, privateKeyTemplate);
```

### Example #4

Working with Object

```javascript
var nObject = pkcs11.C_CreateObject(session, [
    { type: pkcs11js.CKA_CLASS, value: pkcs11js.CKO_DATA },
    { type: pkcs11js.CKA_TOKEN, value: false },
    { type: pkcs11js.CKA_PRIVATE, value: false },
    { type: pkcs11js.CKA_LABEL, value: "My custom data" },
]);

// Updating lable of Object
pkcs11.C_SetAttributeValue(session, nObject, [{ type: pkcs11js.CKA_LABEL, value: nObjetcLabel + "!!!" }]);

// Getting attribute value
var label = pkcs11.C_GetAttributeValue(session, nObject, [
    { type: pkcs11js.CKA_LABEL },
    { type: pkcs11js.CKA_TOKEN }
]);
console.log(label[0].value.toString()); // My custom data!!!
console.log(!!label[1].value[0]; // false

// Copying Object
var cObject = pkcs11.C_CopyObject(session, nObject, [
    { type: pkcs11js.CKA_CLASS},
    { type: pkcs11js.CKA_TOKEN},
    { type: pkcs11js.CKA_PRIVATE},
    { type: pkcs11js.CKA_LABEL},
])

// Removing Object
pkcs11.C_DestroyObject(session, cObject);
```

### Example #4

Searching objects

**NOTE:** If template is not setted for C_FindObjectsInit, then C_FindObjects returns all objects from slot  

```javascript
pkcs11.C_FindObjectsInit(session, [{ type: pkcs11js.CKA_CLASS, value: pkcs11js.CKO_DATA }]);
var hObject = pkcs11.C_FindObjects(session);
while (hObject) {
    var attrs = pkcs11.C_GetAttributeValue(session, hObject, [
        { type: pkcs11js.CKA_CLASS },
        { type: pkcs11js.CKA_TOKEN },
        { type: pkcs11js.CKA_LABEL }
    ]);
    // Output info for objects from token only
    if (attrs[1].value[0]){
        console.log(`Object #${hObject}: ${attrs[2].value.toString()}`);
    }
    hObject = pkcs11.C_FindObjects(session);
}
pkcs11.C_FindObjectsFinal(session);
```

### Example #5

Generating random values

```javascript
var random = pkcs11.C_GenerateRandom(session, new Buffer(20));
console.log(random.toString("hex"));
```

or

```javascript
var random = new Buffer(20);
pkcs11.C_GenerateRandom(session, random);
console.log(random.toString("hex"));
```

### Example #6

Digest

```javascript
pkcs11.C_DigestInit(_session, { mechanism: pkcs11js.CKM_SHA256 });

pkcs11.C_DigestUpdate(session, new Buffer("Incomming message 1"));
pkcs11.C_DigestUpdate(session, new Buffer("Incomming message N"));

var digest = pkcs11.C_DigestFinal(_session, Buffer(256 / 8));

console.log(digest.toString("hex"));
```

### Example #7

Signing data

```javascript
pkcs11.C_SignInit(session, { mechanism: pkcs11js.CKM_SHA256_RSA_PKCS }, keys.privateKey);

pkcs11.C_SignUpdate(session, new Buffer("Incomming message 1"));
pkcs11.C_SignUpdate(session, new Buffer("Incomming message N"));

var signature = pkcs11.C_SignFinal(session, Buffer(256));
```

Verifying data

```javascript
pkcs11.C_VerifyInit(session, { mechanism: pkcs11js.CKM_SHA256_RSA_PKCS }, keys.publicKey);

pkcs11.C_VerifyUpdate(session, new Buffer("Incomming message 1"));
pkcs11.C_VerifyUpdate(session, new Buffer("Incomming message N"));

var verify = pkcs11.C_VerifyFinal(session, signature);
```

### Example #8

Encrypting data with AES-CBC mechanism

```javascript
var cbc_param = pkcs11.C_GenerateRandom(new Buffer(16));

pkcs11.C_EncryptInit(
    session,
    {
        mechanism: pkcs11js.CKM_AES_CBC,
        parameter: cbc_param
    },
    secretKey
);

var enc = new Buffer(0);
enc = Buffer.concat([enc, pkcs11.C_EncryptUpdate(session, new Buffer("Incomming data 1"), new Buffer(16))]);
enc = Buffer.concat([enc, pkcs11.C_EncryptUpdate(session, new Buffer("Incomming data N"), new Buffer(16))]);
enc = Buffer.concat([enc, pkcs11.C_EncryptFinal(session, new Buffer(16))]);

console.log(enc.toString("hex"));
```

Decrypting data with AES-CBC mechanism

```javascript
pkcs11.C_DecryptInit(
    session,
    {
        mechanism: pkcs11js.CKM_AES_CBC,
        parameter: cbc_param
    },
    secretKey
);

var dec = new Buffer(0);
dec = Buffer.concat([dec, pkcs11.C_DecryptUpdate(session, enc, new Buffer(32))]);
dec = Buffer.concat([dec, pkcs11.C_DecryptFinal(session, new Buffer(16))]);

console.log(dec.toString());
```

### Example #9

Deriving key with ECDH mechanism

```javascript
// Recieve public data from EC public key
var attrs = pkcs11.C_GetAttributeValue(session, publicKeyEC, [{ type: pkcs11js.CKA_EC_POINT }])
var ec = attrs[0].value;

var derivedKey = pkcs11.C_DeriveKey(
    session,
    {
        mechanism: pkcs11js.CKM_ECDH1_DERIVE,
        parameter: {
            type: pkcs11js.CK_PARAMS_EC_DH,
            kdf: 2,
            publicData: ec
        }
    },
    privateKeyEC,
    [
        { type: pkcs11js.CKA_CLASS, value: pkcs11js.CKO_SECRET_KEY },
        { type: pkcs11js.CKA_TOKEN, value: false },
        { type: pkcs11js.CKA_KEY_TYPE, value: pkcs11js.CKK_AES },
        { type: pkcs11js.CKA_LABEL, value: "Derived AES key" },
        { type: pkcs11js.CKA_ENCRYPT, value: true },
        { type: pkcs11js.CKA_VALUE_LEN, value: 256 / 8 }
    ]
);
```

## Suitability
At this time this solution should be considered suitable for research and experimentation, further code and security review is needed before utilization in a production application.

## Bug Reporting
Please report bugs either as pull requests or as issues in the issue tracker. Graphene has a full disclosure vulnerability policy. Please do NOT attempt to report any security vulnerability in this code privately to anybody.

## Related
- [PKCS #11 2.40 Specification](http://docs.oasis-open.org/pkcs11/pkcs11-curr/v2.40/pkcs11-curr-v2.40.html)
- [Many PKCS #11 Specifications](http://www.cryptsoft.com/pkcs11doc/)
- [Attacking and Fixing PKCS#11 Security Tokens](http://www.lsv.ens-cachan.fr/Publis/PAPERS/PDF/BCFS-ccs10.pdf)
- [PERL PKCS #11 binding](https://github.com/dotse/p5-crypt-pkcs11)
- [.NET PKCS #11 binding](https://github.com/jariq/Pkcs11Interop)
- [Ruby PKCS #11 binding](https://github.com/larskanis/pkcs11)
- [OCaml PKCS #11 binding](https://github.com/ANSSI-FR/caml-crush)
- [OCaml PKCS #11 CLI](https://github.com/ANSSI-FR/opkcs11-tool)
- [Go PKCS #11 binding](https://github.com/miekg/pkcs11) 
- [PKCS #11 Admin](http://www.pkcs11admin.net)
- [Node.js Foreign Function Interface](https://github.com/node-ffi/node-ffi)
- [GOST PKCS#11 constants](https://github.com/romanovskiy-k/pkcs11/blob/master/rtpkcs11t.h)
- [PKCS#11 logging proxy module](https://github.com/jariq/pkcs11-logger)
- [PKCS#11 Proxy](https://github.com/iksaif/pkcs11-proxy)
- [PKCS#11 Tests](https://github.com/google/pkcs11test)
- [OpenCryptoKi](http://sourceforge.net/projects/opencryptoki/)
- [SoftHSM](https://www.opendnssec.org/softhsm/)
- [SofHSM2 for Windows](https://github.com/disig/SoftHSM2-for-Windows/)
- [node-pcsc](https://github.com/santigimeno/node-pcsclite)
- [PKCS#11 URIs](https://tools.ietf.org/html/rfc7512)
- [Key Length Recommendations](http://www.keylength.com/en/compare/)


