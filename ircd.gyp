{
  'variables': 
  {
    'python-includes': 'python -c "from distutils import sysconfig; import ntpath; print sysconfig.get_python_inc().replace(ntpath.sep, \'/\')"',
    'python-version': 'python -c "from distutils import sysconfig; print sysconfig.get_config_var(\\"VERSION\\")"',
  },
  
  'targets': 
  [{
    'target_name': 'ircd',
    'dependencies': 
    [
      'jsoncpp/jsoncpp.gyp:jsoncpp',
      'libuv/uv.gyp:uv',
    ],
    'type': 'executable',
    'include_dirs': 
    [
      'include',
      '<!@(<(python-includes))',
      'c:\openssl\include',
    ],
    'sources': 
    [
      'common.gypi',
      'etc/example.conf',
      'include/client.h',
      'include/command.h',
      'include/config.h',
      'include/configsection.h',
      'include/connection.h',
      'include/generalsection.h',
      'include/listener.h',
      'include/listenersection.h',
      'include/logging.h',
      'include/loggingsection.h',
      'include/module.h',
      'include/modulesection.h',
      'include/parser.h',
      'include/ssl.h',
      'include/sslconnection.h',
      'include/stdinc.h',
      'include/system.h',
      'include/python/clientwrap.h',
      'include/python/pythonloader.h',
      'include/python/pythonwrap.h',
      'include/python/parserwrap.h',
      'src/client.cc',
      'src/command.cc',
      'src/config.cc',
      'src/connection.cc',
      'src/generalsection.cc',
      'src/listener.cc',
      'src/listenersection.cc',
      'src/logging.cc',
      'src/loggingsection.cc',
      'src/module.cc',
      'src/modulesection.cc',
      'src/main.cc',
      'src/parser.cc',
      'src/ssl.cc',
      'src/sslconnection.cc',
      'src/system.cc',
      'src/python/ircclient.py',
      'src/python/ircd/__init__.py',
      'src/python/clientwrap.cc',
      'src/python/pythonloader.cc',
      'src/python/pythonwrap.cc',
      'src/python/parserwrap.cc'
    ],
    'conditions': 
    [
    [ 
    'OS=="win"', 
    {
      'variables':
      {
        'python-binlibdest': 'python -c "from distutils import sysconfig; print sysconfig.get_config_var(\\"BINLIBDEST\\")"',
      },
      'msvs_settings':
      {
        'VCLinkerTool':
        {
          'AdditionalLibraryDirectories': '<!@(<(python-binlibdest))s;c:\openssl\lib',
        },
      },
      'defines': 
      [
        '_WIN32_WINNT=0x0600',
        '_GNU_SOURCE',
      ],
      'include_dirs': 
      [
        'c:\openssl\include',
      ],
      'libraries': 
      [ 
        'python<!@(<(python-version))_d.lib',
        'libeay32.lib',
        'ssleay32.lib',
      ],
    },
    {
      'cflags': 
      [ 
        '-std=c++0x', 
        '-stdlib=libc++',
        '-Wall', 
        '-Wextra', 
        '-pedantic', 
        '-Wno-long-long', 
        '-Wno-unused-parameter' 
      ],
      'defines': 
      [ 
        '_GNU_SOURCE' 
      ],
      'libraries': 
      [ 
        '-lpython<!@(<(python-version))',
        '-lssl',
        '-lcrypto',
        '-lc++',
      ],
    }
    ],
  ], 
  }]
}
