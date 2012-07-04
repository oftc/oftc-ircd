{
  'targets': [
  {
    'target_name': 'ircd',
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
