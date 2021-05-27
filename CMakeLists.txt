cmake_minimum_required(VERSION 3.19)

project(MYPROJECT)

# Allow custom cmake overrides
set(MYPROJECT_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
set(CMAKE_MODULE_PATH ${MYPROJECT_CMAKE_DIR} ${CMAKE_MODULE_PATH})

# Use ctest for dashboard submissions
include(CTest)

string(TIMESTAMP timestring %Y%m%d%H%M%S UTC)
set(MYPROJECT_MAJOR_VERSION 0)
set(MYPROJECT_MINOR_VERSION 7)
set(MYPROJECT_PATCH_VERSION 0)
set(MYPROJECT_SHORT_VERSION "${MYPROJECT_MAJOR_VERSION}.${MYPROJECT_MINOR_VERSION}")
set(MYPROJECT_VERSION "${MYPROJECT_SHORT_VERSION}.${MYPROJECT_PATCH_VERSION}")

set(MYPROJECT_TWEAK_VERSION ${timestring})

# Include directories
#set(DICOM_INCLUDE_DIRS
#  "${DICOM_SOURCE_DIR}/Source"
#  "${DICOM_BINARY_DIR}/Source")

# Is this CMakeLists.txt file being called directly by cmake?
if("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
  set(_myproject_direct_build ON)
else()
  set(_myproject_direct_build OFF)
endif()

set(MYPROJECT_EXTERNAL_BUILD ${_myproject_direct_build} CACHE BOOL
  "Building MYPROJECT module external to the VTK build")
mark_as_advanced(MYPROJECT_EXTERNAL_BUILD)

if(DEFINED VTK_MODULE_ENABLE_VTK_MYPROJECT AND NOT MYPROJECT_EXTERNAL_BUILD)
  # When building as a remote module for VTK 9 (or VTK 8.90+)
  message(STATUS "vtkMYPROJECT: Building vtkMYPROJECT as a Remote VTK Module")

  # Clear VTK_LIBS, linking will be handled by modular build
  set(VTK_LIBS)
  set(VTK_MYPROJECT_LINK_TARGET VTK::MYPROJECT)

  option(BUILD_MYPROJECT_PROGRAMS "Build the vtkMYPROJECT tools" OFF)
  option(BUILD_MYPROJECT_EXAMPLES "Build the vtkMYPROJECT examples" OFF)
  set(BUILD_PROGRAMS ${BUILD_MYPROJECT_PROGRAMS})
  set(BUILD_EXAMPLES ${BUILD_MYPROJECT_EXAMPLES})
else()
  # When building external to VTK (default)
  message(STATUS "vtkMYPROJECT: Building vtkMYPROJECT outside of VTK")

  # Find VTK and get VTK configuration information
  find_package(VTK REQUIRED)

  message(${VTK_VERSION})

  # define VTK_VERSION if not already defined
  if(NOT DEFINED VTK_VERSION)
    set(VTK_VERSION ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION})
  endif()
  if(VTK_VERSION VERSION_LESS 8.90)
    message(FATAL_ERROR "VTK 8.90+ or later is required.")
  endif()

  # We are being built as a stand-alone package
  set(VTK_MYPROJECT_LINK_TARGET vtkMYPROJECT)

  option(BUILD_SHARED_LIBS "Build shared libraries." ON)

  # Which parts of the package to build
  option(BUILD_TESTING "Build the tests" OFF)
  option(BUILD_PROGRAMS "Build some useful MYPROJECT tools" OFF)
  option(BUILD_EXAMPLES "Build the examples" OFF)

  # Wrap Python option
  if(VTK_WRAP_PYTHON)
    option(BUILD_PYTHON_WRAPPERS "Build python wrappers" ON)
  else()
    unset(BUILD_PYTHON_WRAPPERS CACHE)
  endif()

  # Defines CMAKE_INSTALL_LIBDIR
  include(GNUInstallDirs)

  # Set up our directory structure for output libraries and binaries
  # (Note: these are the build locations, not the install locations)
  if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MYPROJECT_BINARY_DIR}/bin")
  endif()
  if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    if(UNIX)
      set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MYPROJECT_BINARY_DIR}/lib")
    else()
      set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MYPROJECT_BINARY_DIR}/bin")
    endif()
  endif()
  if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MYPROJECT_BINARY_DIR}/lib")
  endif()

  if(NOT VTK_VERSION VERSION_LESS 8.90)
    # When building as a stand-alone module for VTK 9
    message(STATUS "vtkMYPROJECT: Building as an External VTK Module")

    # Clear VTK_LIBS, linking will be handled by modular build
    set(VTK_LIBS)
    set(VTK_MYPROJECT_LINK_TARGET VTK::MYPROJECT)

    vtk_module_scan(
      MODULE_FILES        "${CMAKE_CURRENT_SOURCE_DIR}/Source/myproject.module"
      REQUEST_MODULES     "VTK::MYPROJECT"
      PROVIDES_MODULES    myproject_modules
      ENABLE_TESTS        "${BUILD_TESTING}")

    vtk_module_build(
      MODULES             ${myproject_modules}
      INSTALL_EXPORT      MYPROJECT
      ARCHIVE_DESTINATION "lib"
      HEADERS_DESTINATION "include/myproject${MYPROJECT_SHORT_VERSION}"
      CMAKE_DESTINATION   "${CMAKE_INSTALL_LIBDIR}/cmake/vtk"
      LICENSE_DESTINATION "${CMAKE_INSTALL_LIBDIR}/vtk"
      HIERARCHY_DESTINATION "${CMAKE_INSTALL_LIBDIR}/vtk/hierarchy/${CMAKE_PROJECT_NAME}"
      LIBRARY_NAME_SUFFIX "${MYPROJECT_CUSTOM_LIBRARY_SUFFIX}"
      VERSION             "${MYPROJECT_VERSION}"
      SOVERSION           "1"
      TEST_DATA_TARGET    MyProjectData
      USE_EXTERNAL        "${MYPROJECT_USE_EXTERNAL}"
      TEST_INPUT_DATA_DIRECTORY   "${myproject_test_data_directory_input}"
      TEST_OUTPUT_DATA_DIRECTORY  "${myproject_test_data_directory_output}")

    if(BUILD_PYTHON_WRAPPERS)
      find_package(PythonInterp ${VTK_PYTHON_VERSION} QUIET)

      vtk_module_wrap_python(
        MODULES         ${myproject_modules}
        INSTALL_EXPORT  MYPROJECTPython
        PYTHON_PACKAGE  "vtk.modules"
        #MODULE_DESTINATION  ""
        CMAKE_DESTINATION   "${CMAKE_INSTALL_LIBDIR}/cmake/vtk"
        LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        BUILD_STATIC    OFF)
    endif()

  endif() #(NOT VTK_VERSION VERSION_LESS 8.90)

endif()

# Store the git hash of the current head
if(EXISTS "${MYPROJECT_SOURCE_DIR}/.git/HEAD")
  file(READ "${MYPROJECT_SOURCE_DIR}/.git/HEAD" MYPROJECT_SOURCE_VERSION)
  if("${MYPROJECT_SOURCE_VERSION}" MATCHES "^ref:")
    string(REGEX REPLACE "^ref: *([^ \n\r]*).*" "\\1"
      MYPROJECT_GIT_REF "${MYPROJECT_SOURCE_VERSION}")
    file(READ "${MYPROJECT_SOURCE_DIR}/.git/${MYPROJECT_GIT_REF}"
      MYPROJECT_SOURCE_VERSION)
  endif()
  string(STRIP "${MYPROJECT_SOURCE_VERSION}" MYPROJECT_SOURCE_VERSION)
endif()

string(TIMESTAMP MYPROJECT_BUILD_TIME "%H:%M:%S" UTC)
string(TIMESTAMP MYPROJECT_BUILD_DATE "%d %B %Y" UTC)


if(Module_vtkMYPROJECT)
  set(MYPROJECT_BIN_DIR ${VTK_INSTALL_RUNTIME_DIR})
  set(MYPROJECT_LIB_DIR ${VTK_INSTALL_LIBRARY_DIR})
  set(MYPROJECT_ARC_DIR ${VTK_INSTALL_ARCHIVE_DIR})
  set(MYPROJECT_INC_DIR ${VTK_INSTALL_INCLUDE_DIR})
endif()

# Set up our target directory structure for "make install"
set(MYPROJECT_BIN_DIR "bin") # for executables and ".dll" libraries
if(NOT MYPROJECT_LIB_DIR)
  set(MYPROJECT_LIB_DIR "lib") # for unix/linux/osx shared libraries
endif()
if(NOT MYPROJECT_ARC_DIR)
  set(MYPROJECT_ARC_DIR "${MYPROJECT_LIB_DIR}") # for static or ".lib" libraries
endif()
if(NOT MYPROJECT_INC_DIR)
  set(MYPROJECT_INC_DIR "include") # for header files
endif()
if(NOT MYPROJECT_DATA_DIR)
  set(MYPROJECT_DATA_DIR "share/myproject-${MYPROJECT_SHORT_VERSION}")
endif()
if(NOT MYPROJECT_PGK_DIR)
  set(MYPROJECT_PGK_DIR "${MYPROJECT_LIB_DIR}/myproject-${MYPROJECT_SHORT_VERSION}/cmake")
endif()

# Set the full paths to the install tree
set(MYPROJECT_RUNTIME_INSTALL_DEST ${CMAKE_INSTALL_PREFIX}/${MYPROJECT_BIN_DIR})
set(MYPROJECT_LIBRARY_INSTALL_DEST ${CMAKE_INSTALL_PREFIX}/${MYPROJECT_LIB_DIR})
set(MYPROJECT_ARCHIVE_INSTALL_DEST ${CMAKE_INSTALL_PREFIX}/${MYPROJECT_ARC_DIR})
set(MYPROJECT_INCLUDE_INSTALL_DEST ${CMAKE_INSTALL_PREFIX}/${MYPROJECT_INC_DIR})
set(MYPROJECT_DATA_INSTALL_DEST ${CMAKE_INSTALL_PREFIX}/${MYPROJECT_DATA_DIR})
set(MYPROJECT_CMAKE_INSTALL_DEST ${CMAKE_INSTALL_PREFIX}/${MYPROJECT_PGK_DIR})

if(NOT Module_vtkMYPROJECT)
  # Libraries
  set(MYPROJECT_LIBRARIES vtkMYPROJECT)

  # The library dependencies
  if(NOT VTK_VERSION VERSION_LESS 6)
    set(VTK_TARGET_PREFIX "VTK::")
    set(VTK_LIBS)
    # required libraries
    foreach(_lib CommonCore CommonMisc CommonDataModel ImagingCore IOCore IOImage)
      list(APPEND VTK_LIBS ${VTK_TARGET_PREFIX}${_lib})
    endforeach()

    # factory libraries (only needed if present)
    foreach(_lib IOMPIImage)
      list(FIND VTK_LIBRARIES ${VTK_TARGET_PREFIX}${_lib} _index)
      if(_index GREATER -1)
        list(APPEND VTK_LIBS ${VTK_TARGET_PREFIX}${_lib})
      endif()
    endforeach()
  endif()
endif()

# Disable C and C++ deprecation warnings for MSVC 2005 and later
if(MSVC_VERSION EQUAL 1400 OR MSVC_VERSION GREATER 1400 OR MSVC10)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE
                  -D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
endif()

if(NOT MYPROJECT_EXTERNAL_BUILD OR NOT DEFINED VTK_MODULE_ENABLE_VTK_MYPROJECT)
  # The main library, if not handled by the VTK 9 functions
  add_subdirectory(Source)
endif()

# Subdirectories
if(BUILD_PROGRAMS)
  # add_subdirectory(Programs)
endif()

if(BUILD_EXAMPLES)
  # add_subdirectory(Examples)
endif()

if(MYPROJECT_EXTERNAL_BUILD OR NOT DEFINED VTK_MODULE_ENABLE_VTK_MYPROJECT)

  if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(Testing)
  endif()

  # Install files
  install(FILES Copyright.txt
    DESTINATION ${MYPROJECT_DATA_INSTALL_DEST})

endif()

if(NOT Module_vtkMYPROJECT AND NOT DEFINED VTK_MODULE_ENABLE_VTK_MYPROJECT)
  #---------------------------------------------------
  # Export information for other projects

  # Add all targets to the build-tree export set
  export(TARGETS ${MYPROJECT_LIBRARIES}
    ${MYPROJECT_PYTHON_LIBRARIES} ${MYPROJECT_JAVA_LIBRARIES}
    FILE "${MYPROJECT_BINARY_DIR}/MYPROJECTTargets.cmake")

  # Export the package for use from the build-tree
  # (this registers the build-tree with a global CMake-registry)
  export(PACKAGE MYPROJECT)

  # Create the MYPROJECTConfig.cmake and MYPROJECTConfigVersion.cmake files

  # ... for the build tree
  set(INCLUDE_DIRS_CONFIG ${MYPROJECT_INCLUDE_DIRS})
  set(LIBRARY_DIRS_CONFIG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
  configure_file(CMake/MYPROJECTConfig.cmake.in
    "${MYPROJECT_BINARY_DIR}/MYPROJECTConfig.cmake" @ONLY)

  # ... for the install tree
  file(RELATIVE_PATH REL_INCLUDE_DIR "${MYPROJECT_CMAKE_INSTALL_DEST}"
    "${MYPROJECT_INCLUDE_INSTALL_DEST}")
  set(INCLUDE_DIRS_CONFIG "\${MYPROJECT_PGK_DIR}/${REL_INCLUDE_DIR}")
  file(RELATIVE_PATH REL_LIBRARY_DIR "${MYPROJECT_CMAKE_INSTALL_DEST}"
    "${MYPROJECT_LIBRARY_INSTALL_DEST}")
  set(LIBRARY_DIRS_CONFIG "\${MYPROJECT_PGK_DIR}/${REL_LIBRARY_DIR}")
  configure_file(CMake/MYPROJECTConfig.cmake.in
    "${MYPROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MYPROJECTConfig.cmake" @ONLY)

  # ... for both
  configure_file(CMake/MYPROJECTConfigVersion.cmake.in
    "${MYPROJECT_BINARY_DIR}/MYPROJECTConfigVersion.cmake" @ONLY)

  # Install the MYPROJECTConfig.cmake and MYPROJECTConfigVersion.cmake
  install(FILES
    "${MYPROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/MYPROJECTConfig.cmake"
    "${MYPROJECT_BINARY_DIR}/MYPROJECTConfigVersion.cmake"
    DESTINATION "${MYPROJECT_CMAKE_INSTALL_DEST}" COMPONENT Development)

  # Install the Use file in the build and install directories
  configure_file(${MYPROJECT_SOURCE_DIR}/CMake/UseMYPROJECT.cmake.in
    ${MYPROJECT_BINARY_DIR}/UseMYPROJECT.cmake COPYONLY IMMEDIATE)
  install(FILES
    "${MYPROJECT_BINARY_DIR}/UseMYPROJECT.cmake"
    DESTINATION "${MYPROJECT_CMAKE_INSTALL_DEST}" COMPONENT Development)

  # Install the export set for use with the install-tree
  install(EXPORT MYPROJECTTargets
    DESTINATION "${MYPROJECT_CMAKE_INSTALL_DEST}" COMPONENT Development)
endif()
