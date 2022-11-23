{
    "targets": [{
        "target_name": "sahararenode",
        "cflags!": ["-w", "-fpermissive", "-fno-exceptions"],
        "cflags_cc!": ["-fno-exceptions", "-fpermissive"],
        "sources": [
            "./src/libraries/sahararenode/log.cc",
            "./src/libraries/sahararenode/config.cc",
            "./src/libraries/sahararenode/node.cc",
            "./src/libraries/sahararenode/global.cc",
            "./src/libraries/sahararenode/utils.cc",
            "./src/libraries/sahararenode/libtelnet.cc",
            "./src/libraries/sahararenode/telnetclient.cc",
            "./src/libraries/sahararenode/i_renode.cc",
            "./src/libraries/sahararenode/v-terminal.cc",
            "./src/libraries/sahararenode/analyze_response.cc",
            "./src/libraries/sahararenode/monitor-script.cc",
            "./src/libraries/sahararenode/machine/peripherals.cc",
            "./src/libraries/sahararenode/machine/peripheral.cc",
            "./src/libraries/sahararenode/machine/machine.cc",
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
            "./src/libraries/sahararenode/",
        ],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")",
        ],
        'defines': [],
        'conditions': [
            ['OS=="linux"', {
                # 'link_settings': {
                #     'libraries': ['<!(pwd)/build_ngspice/lib/libngspice.a']
                # },
            }],
            ['OS=="mac"', {
                'xcode_settings': {
                    'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                },
                # 'link_settings': {
                #     'libraries': ['<!(pwd)/build_ngspice/lib/libngspice.a']
                # },
            }]
        ]
    }]
}
