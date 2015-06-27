# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('sun-harvester', ['core','energy'])
    module.source = [
    'model/sun.cc',
    'model/sun-harvester.cc',
    'helper/sun-helper.cc',
    'helper/sun-harvester-helper.cc',
        ]

    module_test = bld.create_ns3_module_test_library('sun-harvester')
    module_test.source = [
        'test/sun-harvester-test-suite.cc',
        'test/sun-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'sun-harvester'
    headers.source = [
     'model/sun.h',
        'model/sun-harvester.h',
        'helper/sun-harvester-helper.h',
        'helper/sun-helper.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

