{
  'variables': 
  {
    'python-includes': 'python -c "from distutils import sysconfig; import ntpath; print sysconfig.get_python_inc().replace(ntpath.sep, \'/\')"',
    'python-version': 'python -c "from distutils import sysconfig; print sysconfig.get_config_var(\\"VERSION\\")"',
    'python-libpath': 'python -c "from distutils import sysconfig; print sysconfig.get_config_var(\\"LIBPL\\")"',
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
      'etc/default.msgs',
      'include/baseclient.h',
      'include/channel.h',
      'include/channelsection.h',
      'include/client.h',
      'include/command.h',
      'include/config.h',
      'include/configsection.h',
      'include/connection.h',
      'include/event.h',
      'include/generalsection.h',
      'include/ircstring.h',
      'include/listener.h',
      'include/listenersection.h',
      'include/logging.h',
      'include/loggingsection.h',
      'include/module.h',
      'include/modulesection.h',
      'include/murmurhash3.h',
      'include/nuhmask.h',
      'include/numeric.h',
      'include/parser.h',
      'include/server.h',
      'include/ssl.h',
      'include/sslconnection.h',
      'include/stdinc.h',
      'include/system.h',
      'include/python/pbool.h',
      'include/python/pchannel.h',
      'include/python/pclient.h',
      'include/python/pclist.h',
      'include/python/pcollection.h',
      'include/python/pcmap.h',
      'include/python/pctype.h',
      'include/python/pdict.h',
      'include/python/pevent.h',
      'include/python/pexception.h',
      'include/python/pint.h',
      'include/python/pmembership.h',
      'include/python/pmethod.h',
      'include/python/pnuhmask.h',
      'include/python/pparser.h',
      'include/python/pobject.h',
      'include/python/pstring.h',
      'include/python/ptuple.h',
      'include/python/ptupleitem.h',
      'include/python/pythonloader.h',
      'include/python/pythonutil.h',
      'src/baseclient.cc',
      'src/channel.cc',
      'src/channelsection.cc',
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
      'src/murmur3.cc',
      'src/main.cc',
      'src/nuhmask.cc',
      'src/numeric.cc',
      'src/parser.cc',
      'src/server.cc',
      'src/ssl.cc',
      'src/sslconnection.cc',
      'src/system.cc',
      'src/python/pbool.cc',
      'src/python/pchannel.cc',
      'src/python/pclient.cc',
      'src/python/pctype.cc',
      'src/python/pdict.cc',
      'src/python/pevent.cc',
      'src/python/pexception.cc',
      'src/python/pint.cc',
      'src/python/pmembership.cc',
      'src/python/pnuhmask.cc',
      'src/python/pobject.cc',
      'src/python/pparser.cc',
      'src/python/pstring.cc',
      'src/python/ptuple.cc',
      'src/python/ptupleitem.cc',
      'src/python/pythonloader.cc',
      'src/python/pythonutil.cc',
      'modules/python/ircd/__init__.py',
      'modules/python/ircd/channel.py',
      'modules/python/ircd/msflags.py',
      'modules/python/ircd/numerics.py',
      'modules/python/ircd/user.py',
      'modules/python/ircclient.py'
    ],
    'conditions': 
    [
      [ 'OS=="win"', 
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
        'xcode_settings': 
        {
          'WARNING_CFLAGS': 
          [
            '-Wall',
            '-Wextra',
            '-Wno-long-long',
            '-Wno-unused-parameter',
            '-Wno-deprecated-declarations',
            '-Wno-newline-eof',
          ],
          'OTHER_CFLAGS': 
          [
            '-std=c++0x',
            '-stdlib=libc++',
          ],
        },
        'cflags': 
        [ 
          '-std=c++0x',
          '-Wall',
          '-Wextra',
          '-pedantic',
          '-Wno-long-long',
          '-Wno-unused-parameter',
          '-Wno-missing-field-initializers',
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
        ],
        'conditions': 
        [
          ['OS == "mac"', 
          {
            'libraries': [ '-lc++' ],
          }],
          ['OS == "freebsd"',
          {
            'cflags+': [ '-stdlib=libc++' ],
            'libraries': [ '-lc++' ],
            'ldflags': [ '-L<!@(<(python-libpath))' ],
          }],
        ]
      }
    ],
  ], 
  }]
}
