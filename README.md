# Build ITK-SNAP 
:page_facing_up: &nbsp; **To see the implemented Documentation click <a href="https://docs.google.com/presentation/d/e/2PACX-1vSwEqnJPaQiE5gsg4227Yb_QzFEcQWMkNBO7O6yMYAR4QaBPB_jwFnAo89bQe2vBu1bGrFQl9S5XiKo/pub?start=false&loop=false&delayms=3000">here</a>.**
<br>
<!-- 
:floppy_disk: &nbsp; **To download executable only, click <a href="https://drive.google.com/file/d/1ZCM4IjsqnoV7v21ieSqEVJKNSl9fllPi/view?usp=sharing">here</a>** (might not work).
-->

## ⚙️ Version control

| Library | Version | Source |
| - | - | - |
| ITK | 4.13.3 | [zip](https://github.com/InsightSoftwareConsortium/ITK/releases/download/v4.13.3/InsightToolkit-4.13.3.zip), [tar.gz](https://github.com/InsightSoftwareConsortium/ITK/releases/download/v4.13.3/InsightToolkit-4.13.3.tar.gz) |
| VTK | 6.3.0 | [zip](https://github.com/Kitware/VTK/archive/refs/tags/v6.3.0.zip), [tar.gz](https://github.com/Kitware/VTK/archive/refs/tags/v6.3.0.tar.gz) |
| QT | 5.6.3 | [download link](https://download.qt.io/new_archive/qt/5.6/5.6.3/) |

--- 

## 🗂 Folder Structure
```
    .
    ITK-SNAP              
    ├── itk
    │   ├── src           # source folder of itk
    │   └── build         # build directory of itk  
    ├── vtk
    │   ├── src           # source folder of vtk
    │   └── build         # build directory of vtk 
    ├── qt5               # downloaded qt5
    ├── build             # build directory of itksnap
    └── itksnap-v2        # source folder of itksnap   
    ...
    └── cmake             # version seems not matter. Works in v3.16.0-rc3
``` 

--- 

## 👷🏼‍♂️ Installation

### Windows
| Visual Studio | 2015 | 
|-|-|



#### ITK download

1. Download src file as <a href="https://github.com/InsightSoftwareConsortium/ITK/releases/download/v4.13.3/InsightToolkit-4.13.3.zip">.zip</a> or <a href="https://github.com/InsightSoftwareConsortium/ITK/releases/download/v4.13.3/InsightToolkit-4.13.3.tar.gz">.tar.gz</a>
2. Move the source file to ```ITK-SNAP/itk/src``` and Compile itk using CMAKE GUI
:warning: Make sure to check **Visual Studio 2015** and **x64** (depends on machine) when *Configure*
3. Build itk using Visual Studio

#### VTK download
1. Download src file as <a href="https://github.com/Kitware/VTK/archive/refs/tags/v6.3.0.zip">.zip</a> or <a href="https://github.com/Kitware/VTK/archive/refs/tags/v6.3.0.tar.gz">.tar.gz</a>
2. Move the source file to ```ITK-SNAP/vtk/src``` and Compile vtk using CMAKE GUI
:warning: Make sure to check **Visual Studio 2015** and **x64** (depends on machine) when *Configure*
3. Build vtk using Visual Studio

#### Qt download
1. Download Qt with a [link](https://download.qt.io/new_archive/qt/5.6/5.6.3/)



### ITK-SNAP Donwload
1. Clone this project. 
```shell
# in ITK-SNAP
git clone https://github.com/donghakang/itksnap-v2.git
```
2. Download submodule
```shell
cd itksnap-v2           # in ITK-SNAP/itksnap
git submodule init
git submodule update
``` 
3. Build the itksnap by using CMake.app 
    - CMake.app
    ```shell
    ITK_DIR           |  <absolute dir>/ITK-SNAP/itk/build
    VTK_DIR           |  <absolute dir>/ITK-SNAP/vtk/build
    CMAKE_PREFIX_PATH |  <absolute dir>/ITK-SNAP/qt5/5.6.3/clang_64/lib/cmake
    ```

4. Build using Visual Studio
5. Package using Visual Studio
⚠️ Sometimes, Visual Studio gives error message saying "vtkxxxx.dll" not found, just simply copy & paste all the dll files in vtk build library to project folder


### MacOS

#### ITK download

1. Download src file as <a href="https://github.com/InsightSoftwareConsortium/ITK/releases/download/v4.13.3/InsightToolkit-4.13.3.zip">.zip</a> or <a href="https://github.com/InsightSoftwareConsortium/ITK/releases/download/v4.13.3/InsightToolkit-4.13.3.tar.gz">.tar.gz</a>
2. Move the source file to ```ITK-SNAP/itk/src``` and Compile itk.
```shell
# in ITK-SNAP/itk/build
cmake ../src
```
3. Build itk
```shell
# in ITK-SNAP/itk/build
make -j7                # depends on RAM size
```

#### VTK download
1. Download src file as <a href="https://github.com/Kitware/VTK/archive/refs/tags/v6.3.0.zip">.zip</a> or <a href="https://github.com/Kitware/VTK/archive/refs/tags/v6.3.0.tar.gz">.tar.gz</a>
2. Move the source file to ```ITK-SNAP/vtk/src``` and Compile vtk.
```shell
# in ITK-SNAP/vtk/build
cmake ../src
```
3. Build itk
```shell
# in ITK-SNAP/vtk/build
make -j7                # depends on RAM size
```

#### Qt download
1. Download Qt with a [link](https://download.qt.io/new_archive/qt/5.6/5.6.3/)



### ITK-SNAP Donwload
1. Clone this project. 
```shell
# in ITK-SNAP
git clone https://github.com/donghakang/itksnap-v2.git
```
2. Download submodule
```shell
cd itksnap-v2           # in ITK-SNAP/itksnap
git submodule init
git submodule update
``` 
3. Build the itksnap by using CMake.app or ```ccmake``` 
    - CMake.app
    ```shell
    ITK_DIR           |  <absolute dir>/ITK-SNAP/itk/build
    VTK_DIR           |  <absolute dir>/ITK-SNAP/vtk/build
    CMAKE_PREFIX_PATH |  <absolute dir>/ITK-SNAP/qt5/5.6.3/clang_64/lib/cmake
    ```
    Both *Unix Configuration* and *XCode configuration* works (MacOS)
    <br>
    - ```ccmake``` 
    ```shell
    # in ITK-SNAP/build
    ccmake ../itksnap-v2 -D ITK_DIR:FILEPATH=<absolute dir>/ITK-SNAP/itk/build -D VTK_DIR:FILEPATH=<absolute dir>/ITK-SNAP/vtk/build -D CMAKE_PREFIX_DIR:FILEPATH=<absolute dir>/ITK-SNAP/qt5/5.6.3/clang_64/lib/cmake
    ```
4. Build
```shell
make -j7  # in ITK-SNAP/build
```
5. Package
```shell
make package 
```

---

For the source code/documentation of ITKSNAP click <a href="https://github.com/pyushkevich/itksnap">here</a> (Link to ITKSNAP github)
