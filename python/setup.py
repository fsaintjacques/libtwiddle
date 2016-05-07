import sys
from setuptools import setup, find_packages
from setuptools.command.test import test as TestCommand

class PyTest(TestCommand):
    '''Define how to use pytest to test the code'''

    def finalize_options(self):
        TestCommand.finalize_options(self)
        self.test_args = []
        self.test_suite = True

    def run_tests(self):
        #import here, cause outside the eggs aren't loaded
        import pytest
        sys.exit(pytest.main(['tests']))

setup(
    name="twiddle",
    description="python bindings to libtwiddle",
    version="0.0.0",
    author="Francois Saint-Jacques",
    author_email="fsaintjacques@gmail.com",
    url="https://github.com/fsaintjacques/libtwiddle",
    py_modules=['twiddle'],
    license="LGPL License, version 3.0",
    packages=find_packages(exclude=['tests']),
    tests_require=['hypothesis==1.19.0', 'pytest'],
    cmdclass={'test': PyTest}
    )
