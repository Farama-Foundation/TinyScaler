import platform

from Cython.Build import cythonize
from setuptools import Extension, setup


def get_description():
    """Gets the description from the readme."""
    with open("README.md") as fh:
        long_description = ""
        header_count = 0
        for line in fh:
            if line.startswith("##"):
                header_count += 1
            if header_count < 2:
                long_description += line
            else:
                break
    return header_count, long_description


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

header_count, long_description = get_description()

setup(
    name="tinyscaler",
    version="1.2.5",
    author="Farama Foundation",
    author_email="contact@farama.org",
    description="A tiny, simple image scaler",
    url="https://github.com/Farama-Foundation/TinyScaler",
    license="MIT",
    license_files=("LICENSE",),
    long_description=long_description,
    long_description_content_type="text/markdown",
    keywords=["Reinforcement Learning", "Gymnasium", "PettingZoo"],
    python_requires=">=3.7, <3.11",
    include_package_data=True,
    install_requires=[
        "numpy",
    ],
    ext_modules=cythonize(
        ext_modules, language_level=3, compiler_directives={"annotation_typing": False}
    ),
    zip_safe=True,
)
