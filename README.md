# AUTD3 BLAS Backend

This repository provides a calculation backend for HoloGain (multiple foci).
* **Only OpenBLAS and Intel MKL are tested**; but, other BLAS libraries also may work. 

## Build

```
mkdir build
cd build
cmake ..
```
or
```
cmake .. -DBLAS_LIB_DIR=<your BLAS library path> -DBLAS_INCLUDE_DIR=<your BLAS include path>
```

* If you are using Windows, you may need to set `BLAS_DEPEND_LIB_DIR` to link some additional libraries.
    * For example, if you installed OpenBLAS as follow the below install example, you need link `flangmain.lib` by the following command;
        ```
        cmake .. -DBLAS_LIB_DIR=C:/opt/lib -DBLAS_INCLUDE_DIR=C:/opt/include/openblas -DBLAS_DEPEND_LIB_DIR=<your conda path>/Library/lib
        ``` 

* If you use Intel MKL, please set `USE_MKL` ON.
    ```
    cmake .. -DBLAS_LIB_DIR=<your MKL lib path> -DBLAS_INCLUDE_DIR=<your MKL include path> -DUSE_MKL=ON
    ```

## OpenBLAS install example in Windows

* Following is an example to install [OpenBLAS](https://github.com/xianyi/OpenBLAS). See also [official instruction](https://github.com/xianyi/OpenBLAS/wiki/How-to-use-OpenBLAS-in-Microsoft-Visual-Studio).
    * Install Visual Studio 2019 and Anaconda, then open Anaconda Prompt.
        ```
        git clone https://github.com/xianyi/OpenBLAS
        cd OpenBLAS
        conda update -n base conda
        conda config --add channels conda-forge
        conda install -y cmake flang clangdev perl libflang ninja
        "c:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat"
        set "LIB=%CONDA_PREFIX%\Library\lib;%LIB%"
        set "CPATH=%CONDA_PREFIX%\Library\include;%CPATH%"
        mkdir build
        cd build
        cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_C_COMPILER=clang-cl -DCMAKE_Fortran_COMPILER=flang -DCMAKE_MT=mt -DBUILD_WITHOUT_LAPACK=no -DNOFORTRAN=0 -DDYNAMIC_ARCH=ON -DCMAKE_BUILD_TYPE=Release
        cmake --build . --config Release
        cmake --install . --prefix c:\opt -v
        ```
    * Also, you may have to add `%CONDA_HOME%\Library\bin` to PATH, where `CONDA_HOME` is a home directory path of Anaconda (or miniconda).

# Author

Shun Suzuki, 2021
