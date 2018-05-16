# cmake_example for pybind11

An example [pybind11](https://github.com/pybind/pybind11) module built with a
CMake-based build system. This is useful for C++ codebases that have an existing
CMake project structure.


## Prerequisites

**On Unix (Linux, OS X)**

* A compiler with C++11 support
* CMake >= 2.8.12

**On Windows**

* Visual Studio 2015 (required for all Python versions, see notes below)
* CMake >= 3.1


## Installation

Just clone this repository and pip install. Note the `--recursive` option which is
needed for the pybind11 submodule:

To build as dynamic library without installing (recommended)

```bash
git clone --recursive http://git.visionlabs.ru/m.safina/FaceEnginePythonBinding.git
mkdir build
cd build
cmake .. -DFSDK_ROOT=<path to FSDK> -DCMAKE_INSTALL_PREFIX=./install
cmake --build . --config Release
```

To install in system (not recommended).
In `setup.py` please change path to LUNA SDK, by default `cmake_args = ['-DFSDK_ROOT=./..' `  
```bash
git clone --recursive http://git.visionlabs.ru/m.safina/FaceEnginePythonBinding.git
pip install ./FaceEngine
```

With the `setup.py` file included in this example, the `pip install` command will
invoke CMake and build the pybind11 module as specified in `CMakeLists.txt`.


## Special notes for Windows

**Compiler requirements**

Pybind11 requires a C++11 compliant compiler, i.e Visual Studio 2015 on Windows.
This applies to all Python versions, including 2.7. Unlike regular C extension
modules, it's perfectly fine to compile a pybind11 module with a VS version newer
than the target Python's VS version. See the [FAQ] for more details.

**Runtime requirements**

The Visual C++ 2015 redistributable packages are a runtime requirement for this
project. It can be found [here][vs2015_runtime]. If you use the Anaconda Python
distribution, you can add `vs2015_runtime` as a platform-dependent runtime
requirement for you package: see the `conda.recipe/meta.yaml` file in this example.

## License

Pybind11 is provided under a BSD-style license that can be found in the LICENSE
file. By using, distributing, or contributing to this project, you agree to the
terms and conditions of this license.


## Test call

From root directory with 

```bash
python3.6 tests/test_estimators.py --bind-path build
```

```python3
import FaceEngine as f
faceEnginePtr = f.createPyFaceEnginePtr("data", "data/faceengine.conf")
```


[FAQ]: http://pybind11.rtfd.io/en/latest/faq.html#working-with-ancient-visual-studio-2009-builds-on-windows
[vs2015_runtime]: https://www.microsoft.com/en-us/download/details.aspx?id=48145
