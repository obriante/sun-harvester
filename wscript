# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('sun-harvester', ['core','energy'])
    module.source = [
    'model/sun.cc',
    'model/solar-energy-harvester.cc',
    'helper/solar-energy-harvester-helper.cc',
    'helper/solar-energy-trace-helper.cc',
        ]

    module_test = bld.create_ns3_module_test_library('sun-harvester')
    module_test.source = [
    'test/solar-energy-harvester-test.cc',
    'test/sun-test.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'sun-harvester'
    headers.source = [
        'model/sun.h',
        'model/solar-energy-harvester.h',
        'helper/solar-energy-harvester-helper.h',
        'helper/solar-energy-trace-helper.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # bld.ns3_python_bindings()

