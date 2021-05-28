# Template for VTK remote modules

This is a CMake template for creating a remote module for VTK 8.9 (or
later). The template is an example of a simple VTK module, which can
be built together with VTK as a remote module or as a stand-alone
module for an existing installation of VTK. Wrapper are created for
Python, but wrapper for TCL and Java has been left out.

## Remote module

In the VTK source repository, a folder is reserved for remote
modules, <code>VTK-9.0.1/Remote</code>. To include this module as a remote module, do the following

1. Create a file <code>MyProject.remote.cmake</code> containing the following
   \#
   \# MyProject
   \#
   vtk_fetch_module(MyProject
     "Short description of the module"
     GIT_REPOSITORY https://gitlab.bkmedical.com/MyProject
     \# May 28, 2021 v0.7
     GIT_TAG 196f99dbbf52003c52323501081861a909f94301
   )
2. When VTK is build, they module vtkMYPROJECT will appear as an option to build and install
   cmake .. -DVTK_MODULE_ENABLE_VTK_vtkMYPROJECT=ON

I have tested this only using VTK 9.0.1 and only the build phase.

## Standalone module (preferred)

Simply run CMake. Note that only VTK 8.9+ is supported and this sample
project depends on Qt since it is an interactor for a game pad. 3D/4D
algorithm will be independent of Qt.

1. Simply run CMake (Python wrapper are enabled by default)
   cmake ..

The solution will contain projects for continous integration, Nightly
runs, memory check etc. The only projects of interest are
<code>MYPROJECT</code>, <code>TestInteractorStyleGame</code> and
<code>vtkMYPROJECTPython</code>. The first project is the module, the
second is a test project and the last is the wrapper project for
Python.

