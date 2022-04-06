import os
import re
import sys
import platform
import subprocess
import shutil

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[
            'src/scaler.h',
            'src/scaler.c',
            ])

        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))

        # Required for auto-detection of auxiliary 'native' libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == 'Windows':
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]

            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']

            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CFLAGS', ''), self.distribution.get_version())

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        subprocess.check_call(['cmake', ext.sourcedir ] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)

        # Call manually due to ill-defined order when using cffi_modules
        import ffi_build

        ffi_build.build(self.build_temp, extdir)

setup(
    name='tinyscaler',
    version='1.0.0',
    description='A tiny, simple image scaler',
    long_description='https://github.com/Farama-Foundation/TinyScaler',
    install_requires=[
       'cffi',
       'numpy',
    ],
    packages=['tinyscaler'],
    package_dir={'tinyscaler': 'tinyscaler'},
    package_data={'tinyscaler': ['_scaler_cffi.*']},
    license='MIT',
    classifiers=[
        'Environment :: Console',
        'Programming Language :: Python :: 3',
        'Intended Audience :: Science/Research',
        'License :: MIT License',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries :: Python Modules'
    ],
    ext_modules=[ CMakeExtension('tinyscaler') ],
    cmdclass={
        'build_ext': CMakeBuild,
    },
    zip_safe=True,
    include_package_data=True,
)
