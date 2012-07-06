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
      'src/logging.cc',
      'src/main.cc',
    ],
    'defines': [
      '_GNU_SOURCE',
    ],
    'cflags': [ '-Wall -Wextra -Werror -pedantic -Wno-long-long -Wno-unused-parameter' ],
  },
  ],
}
