{
  'variables': {
  },

  'targets': [
  {
    'target_name': 'ircd',
    'dependencies': [
      'jsoncpp/jsoncpp.gyp:jsoncpp',
      'libuv/uv.gyp:uv',
    ],
    'type': 'executable',
    'include_dirs': [
      'include',
    ],
    'sources': [
      'src/config.cc',
      'src/generalsection.cc',
      'src/listener.cc',
      'src/listenersection.cc',
      'src/logging.cc',
      'src/loggingsection.cc',
      'src/main.cc',
      'src/system.cc'
    ],
    'defines': [
      '_GNU_SOURCE',
    ],
    'cflags': [ '-Wall', '-Wextra', '-pedantic', '-Wno-long-long', '-Wno-unused-parameter' ],
  },
  ],
}
