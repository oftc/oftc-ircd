{
  'variables': {
  },

  'target_defaults': {
    'defines': [
      '_GNU_SOURCE',
    ],
  },

  'targets': [
  {
    'target_name': 'ircd',
    'dependencies': [
      'libuv/uv.gyp:uv',
    ],
    'type': 'executable',
    'include_dirs': [
      'include',
    ],
    'sources': [
      'src/config.cc',
      'src/config_general.cc',
      'src/config_logging.cc',
      'src/logging.cc',
      'src/main.cc',
    ],
  },
  ],
}
