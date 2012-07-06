{
  'target_defaults': {
    'default_configuration': 'Debug',
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'cflags': [ '-g', '-O0' ],
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'cflags': [ '-O3' ],
      },
    },
  },
}
