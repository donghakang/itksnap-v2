## ITK-SNAP README
To see the implemented Documentation click <a href="https://docs.google.com/presentation/d/e/2PACX-1vSwEqnJPaQiE5gsg4227Yb_QzFEcQWMkNBO7O6yMYAR4QaBPB_jwFnAo89bQe2vBu1bGrFQl9S5XiKo/pub?start=false&loop=false&delayms=3000">here</a>.
To download executable only, click <a href="https://drive.google.com/file/d/1y48tK0r7OCHd8ta0d0JubFIR228Ogw8Y/view?usp=sharing">here</a> (might not work).

### Installation
First, below is the folder structure.
```
    .
    ITK-SNAP              
    ├── itk               # v4.13.3
    │   ├── src           # source folder of itk
    │   └── build         # build directory of itk  
    ├── vtk               # v6.3.0
    │   ├── src           # source folder of vtk
    │   └── build         # build directory of vtk 
    ├── qt5               # v5.12.0 // seems like newest version also works.
    │   ├── src           # source folder of qt5
    │   └── build         # build directory of qt5 
    ├── build             # build directory of itksnap
    └── itksnap-v2        # source folder of itksnap   
    ...
    └── cmake             # version seems not matter. Works in v3.16.0-rc3
```         

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

#### QT5 download
1. Clone src file from and change the branch or download the v5.12.0 version.
```shell
# to clone and change the branch
git clone https://github.com/qt/qt5.git
git fetch 
git checkout -b 5.12 origin/5.12
```
2. Download all submodule in qt5
```shell
# in ITK-SNAP/qt5/src
perl init-repository
```
3. Use qmake to create build make file.
```shell
# in ITK-SANP/qt5/build
../src/configure -developer-build -opensource -nomake examples -nomake tests -skip webengine
```
4. Build qt5
```shell
make -j7
```
This will take a LONG time.


#### ITK-SNAP Donwload
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
    CMAKE_PREFIX_DIR  |  <absolute dir>/ITK-SNAP/qt5/build/qtbase/lib/cmake
    ```
    <img src="./img/cmake.jpg" alt="cmake sample" width="50%"></img>

    - ```ccmake``` 
    ```shell
    # in ITK-SNAP/build
    ccmake ../itksnap-v2 -D ITK_DIR:FILEPATH=<absolute dir>/ITK-SNAP/itk/build -D VTK_DIR:FILEPATH=<absolute dir>/ITK-SNAP/vtk/build -D CMAKE_PREFIX_DIR:FILEPATH=<absolute dir>/ITK-SNAP/qt5/build/qtbase/lib/cmake
    ```
4. Build
```shell
make -j7  # in ITK-SNAP/build
```

---

For the source code/documentation of ITKSNAP click <a href="https://github.com/pyushkevich/itksnap">here</a> (Link to ITKSNAP github)
