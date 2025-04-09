import os
import re
import sys
import subprocess
import platform

# import versioneer

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

# for more info, check
# https://github.com/pybind/cmake_example
# TODO: update the CMakeBuild class


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=""):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(["cmake", "--version"])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: "
                + ", ".join(e.name for e in self.extensions)
            )
        cmake_version = LooseVersion(
            re.search(r"version\s*([\d.]+)", out.decode()).group(1)
        )
        if cmake_version < "3.11.0":
            raise RuntimeError("CMake >= 3.11.0 is required")
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = [
            "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=" + extdir,
            "-DGENERICIO_PYTHON_LIBRARY=ON",
            "-DGENERICIO_LEGACY_PYTHON_LIBRARY=OFF",
            "-DGENERICIO_MPI_EXECUTABLES=OFF",
            "-DGENERICIO_FRONTEND_EXECUTABLES=OFF",
            "-DPYTHON_EXECUTABLE=" + sys.executable,
        ]
        cfg = "Debug" if self.debug else "Release"
        build_args = ["--config", cfg]

        if platform.system() == "Windows":
            cmake_args += [
                "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}".format(cfg.upper(), extdir)
            ]
            if sys.maxsize > 2**32:
                cmake_args += ["-A", "x64"]
            build_args += ["--", "/m"]
        else:
            cmake_args += ["-DCMAKE_BUILD_TYPE=" + cfg]
            build_args += ["--", "-j"]

        env = os.environ.copy()
        env["CXXFLAGS"] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get("CXXFLAGS", ""), self.distribution.get_version()
        )
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(
            ["cmake", ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env
        )
        subprocess.check_call(
            ["cmake", "--build", "."] + build_args, cwd=self.build_temp
        )


# def get_cmdclass():
#    cmdclass = versioneer.get_cmdclass()
#    cmdclass.update({"build_ext": CMakeBuild})
#    return cmdclass


setup(
    name="pygio",
    use_scm_version={
        "write_to": "python/pygio/_version.py",
        "write_to_template": '__version__ = "{version}"\n',
    },
    author="",
    author_email="",
    ext_package="pygio",
    packages=["pygio"],
    package_dir={"": "python"},
    ext_modules=[CMakeExtension("pygio")],
    # cmdclass=get_cmdclass(),
    cmdclass={
        "build_ext": CMakeBuild,
    },
    zip_safe=False,
    install_requires=["numpy"],
)
