from setuptools import Extension, setup
from Cython.Build import cythonize
import os

ext_modules = []

if os.uname()[4][:3] == 'arm': # Detect arm platform
    ext_modules = [
        Extension(
            "tinyscaler",
            ["src/*.pyx"],
            extra_compile_args=['-mfpu=neon'],
            extra_link_args=['-mfpu=neon'],
        )
    ]
else: # x86_64
    ext_modules = [
        Extension(
            "tinyscaler",
            ["src/*.pyx"]
        )
    ]

setup(
    name='tinyscaler',
    version='1.2.2',
    description='A tiny, simple image scaler',
    long_description='https://github.com/Farama-Foundation/TinyScaler',
    install_requires=[
       'numpy',
       'Cython',
    ],
    license='MIT',
    classifiers=[
        'Environment :: Console',
        'Programming Language :: Python :: 3',
        'Intended Audience :: Science/Research',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries :: Python Modules'
    ],
    ext_modules=cythonize(ext_modules, language_level=3, compiler_directives={'annotation_typing': False}),
    zip_safe=True,
    include_package_data=True,
)
