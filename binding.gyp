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
				'src/ObjectProxyFactory.cc',
				'src/ObjectProxy.cc',
				'src/PrimitiveProxy.cc',
				'src/NumberProxy.cc'
				'src/FuncrionProxyFactory.cc'
			],

			'cflags_cc!'  : [ '-fno-rtti' ],       			# exclude rtti flag
			'cflags_cc'   : [ '<@(root_cflags)' ],
			'ldflags_cc'  : [ '<@(root_ldflags)' ],
			'include_dirs': [ '<@(root_incdir)' ], 			# using variable instead of '/usr/include/root'
			'libraries'   : [ '<@(root_glibs)' ]   			# using variable instead of '/usr/lib/root/libXXX.so'

			# TODO: Use 'conditions' to adress OS specifics that are not covered by root-config
		}
	]
}
