
{
    "variables": {
    },
    "targets": [
        {
            "include_dirs": [
                "<!(node -e \"require(\'nan\')\")",
                "includes"
            ],
            'cflags!': ['-fno-exceptions'],
            'cflags_cc!': ['-fno-exceptions'],
            "target_name": "pkcs11",
            "sources": [
                "src/main.cpp",
                "src/dl.cpp",
                "src/const.cpp",
                "src/pkcs11/error.cpp",
                "src/pkcs11/v8_convert.cpp",
                "src/pkcs11/template.cpp",
                "src/pkcs11/mech.cpp",
                "src/pkcs11/param.cpp",
                "src/pkcs11/param_aes.cpp",
                "src/pkcs11/param_rsa.cpp",
                "src/pkcs11/param_ecdh.cpp",
                "src/pkcs11/pkcs11.cpp",
                "src/async.cpp",
                "src/node.cpp"
            ],
            'conditions': [
                [
                    'OS=="mac"', {
                        'xcode_settings': {
                            'OTHER_CPLUSPLUSFLAGS': ['-std=c++11', '-stdlib=libc++', '-v'],
                            'OTHER_CFLAGS': ['-ObjC++'],
                            'OTHER_LDFLAGS': ['-stdlib=libc++'],
                            'MACOSX_DEPLOYMENT_TARGET': '10.7',
                            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                        },
                        'libraries': [
                            '-lobjc'
                        ],
                    },
                    'OS == "win"', {
                        'msvs_settings': {
                            'VCCLCompilerTool': {
                                'ExceptionHandling': 1,
                            }
                        }
                    }
                ]
            ]
        }
    ]
}
