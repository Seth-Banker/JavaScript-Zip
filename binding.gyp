{
  "targets": [
    {
      "target_name": "javascript-zip-native",
      "sources": [
        "src/javascript_zip.cc",
        "zipper.cpp",
        "deps/miniz.c"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "defines": [
        "NAPI_VERSION=8"
      ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],

      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
        "CLANG_CXX_LIBRARY": "libc++",
        "MACOSX_DEPLOYMENT_TARGET": "10.7"
      },

      "msvs_settings": {
        "VCCLCompilerTool": { "ExceptionHandling": 1 }
      },

      "conditions": [
        [ "OS==\"win\"", {
          "defines": [ "NOMINMAX", "_CRT_SECURE_NO_WARNINGS" ],
          "msvs_settings": {
            "VCCLCompilerTool": {
              "AdditionalOptions": [ "/std:c++17" ]
            }
          }
        }],

        [ "OS==\"linux\" or OS==\"mac\"", {
          "cflags": [ "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE" ],
          "cflags_cc": [ "-std=c++17" ]
        }]
      ]
    }
  ]
}
