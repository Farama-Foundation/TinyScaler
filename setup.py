from setuptools import Extension, setup
from Cython.Build import cythonize

setup(
    name='tinyscaler',
    version='1.2.0',
    description='A tiny, simple image scaler',
    long_description='https://github.com/Farama-Foundation/TinyScaler',
    license='MIT',
    classifiers=[
        'Environment :: Console',
        'Programming Language :: Python :: 3',
        'Intended Audience :: Science/Research',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python',
        'Topic :: Software Development :: Libraries :: Python Modules'
    ],
    ext_modules=cythonize('src/*.pyx', language_level=3, compiler_directives={'annotation_typing': False}),
    zip_safe=True,
    include_package_data=True,
)
