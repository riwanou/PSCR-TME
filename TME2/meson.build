project('TME2', 'cpp', default_options: ['cpp_std=c++14'])

inc = ['hashmap.h', 'iterator.h']
src = []

executable('tme2', 'main.cpp', src, inc)

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
tests = ['test/tests.cpp']

executable('test_tme2', tests, src, inc, dependencies: gtest_dep)

 