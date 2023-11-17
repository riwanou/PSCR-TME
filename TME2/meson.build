project('TME2', 'cpp', default_options: ['cpp_std=c++14'])

src = []

executable('tme2', 'main.cpp', src)
executable('tme3', 'main-tme3.cpp', src)

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
tests = ['test/tests.cpp']

executable('test_tme2', tests, src, inc, dependencies: gtest_dep)
