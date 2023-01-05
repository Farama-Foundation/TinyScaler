import platform

from Cython.Build import cythonize
from setuptools import Extension, setup

ext_modules = []

if platform.system() == "Windows":  # Windows
    ext_modules = [Extension("tinyscaler", ["src/*.pyx"])]
else:  # Not Windows
    if platform.machine() == "x86_64":  # Detect x86_64 platform
        ext_modules = [Extension("tinyscaler", ["src/*.pyx"])]
    else:  # Arm assumed
        ext_modules = [
            Extension(
                "tinyscaler",
                ["src/*.pyx"],
                extra_compile_args=["-mfpu=neon"],
                extra_link_args=["-mfpu=neon"],
            )
        ]

setup(
    name="tinyscaler",
    ext_modules=cythonize(
        ext_modules, language_level=3, compiler_directives={"annotation_typing": False}
    ),
)
