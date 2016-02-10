{
	'variables' : {
		'root_cflags' :  '<!(root-config --cflags)', 		# compiler flags + includes directory
		'root_ldflags':  '<!(root-config --ldflags)',		# linker flags
		'root_incdir' :  '<!(root-config --incdir)', 		# header directory
		'root_libdir' :  '<!(root-config --libdir)', 		# library directory
		'root_glibs'  :  '<!(root-config --glibs)'   		# core + graphical library directory
	},

	'targets': [
		{
			'target_name': 'rootjs',
			'sources': [
				'src/FunctionProxy.cc',
				'src/NodeHandler.cc',
				'src/Proxy.cc',
				'src/GlobalInfo.cc',
				'src/MemberInfo.cc',
				'src/ObjectProxyFactory.cc',
				'src/ObjectProxy.cc',
				'src/PrimitiveProxy.cc',
				'src/StringProxy.cc',
				'src/BooleanProxy.cc',
				'src/NumberProxy.cc',
				'src/FunctionProxyFactory.cc',
				'src/CallbackHandler.cc',
				'src/NodeApplication.cc',
				'src/AsyncRunner.cc',
				'src/CallbackHandler.cc',
				'src/TemplateFactory.cc',
				'src/Toolbox.cc',
				'src/PointerInfo.cc',
				'src/Types.cc'
			],

			'cflags_cc!'  : [ '-fno-rtti', '-fno-exceptions'],
			'cflags_cc'   : [ '<@(root_cflags)'],
			'ldflags_cc'  : [ '<@(root_ldflags)' ],
			'include_dirs': [ '<@(root_incdir)' ], 			# using variable instead of '/usr/include/root'
			'libraries'   : [ '<@(root_glibs)' ],   		# using variable instead of '/usr/lib/root/libXXX.so'

			"conditions": [
				[
					'OS=="mac"', {
						"xcode_settings": {
							"OTHER_CPLUSPLUSFLAGS" : [ "-std=c++11", "-stdlib=libc++" ],
							"OTHER_LDFLAGS": [ "-stdlib=libc++" ],
							"MACOSX_DEPLOYMENT_TARGET": "10.7",
							"GCC_ENABLE_CPP_RTTI": "YES",
							"GCC_ENABLE_CPP_EXCEPTIONS": "YES"
						}
					}
				]
			]
		}
	]
}
