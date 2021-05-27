# Configuration header
set(MYPROJECT_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(MYPROJECT_BUILD_TESTING ${BUILD_TESTING})

set(MYPROJECT_OVERRIDE " override")
set(MYPROJECT_DELETE " = delete")

# Moved from top directory
string(TIMESTAMP MYPROJECT_BUILD_TIME "%H:%M:%S" UTC)
string(TIMESTAMP MYPROJECT_BUILD_DATE "%d %B %Y" UTC)
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

configure_file("${MYPROJECT_CMAKE_DIR}/vtkMYPROJECTConfig.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/vtkMYPROJECTConfig.h" @ONLY)

configure_file("${MYPROJECT_CMAKE_DIR}/vtkMYPROJECTBuild.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/vtkMYPROJECTBuild.h" @ONLY)

# Include directories
include_directories(${MYPROJECT_INCLUDE_DIRS})

# Create the main library
set(LIB_NAME vtkMYPROJECT)

# Sources in the current directory (library sources only!)
set(LIB_SRCS
  vtkInteractorStyleGame.cpp
)

# Headers without a matching .cxx file are listed here
set(LIB_HDRS
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMYPROJECTBuild.h
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMYPROJECTConfig.h
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMYPROJECTModule.h
)

# Internal headers (not installed)
set(LIB_PRIVATE_HDRS
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMYPROJECTBuild.h
)

# Sources that are abstract
set(LIB_ABSTRACT
)

# Sources that are not vtkObjects
set(LIB_SPECIAL
  GamepadHandler.cxx
)

set_source_files_properties(${LIB_HDRS} ${LIB_SPECIAL}
  PROPERTIES WRAP_EXCLUDE ON)
set_source_files_properties(${LIB_ABSTRACT}
  PROPERTIES ABSTRACT ON)
set_source_files_properties(${LIB_PRIVATE_HDRS}
  PROPERTIES SKIP_HEADER_INSTALL ON)

# Third party library dependencies (none)



# Create the library

if(DEFINED VTK_MODULE_ENABLE_VTK_MYPROJECT)
  # When building as a module for VTK 8.90 or later
  set(LIB_SRC_HDRS)
  foreach(_src ${LIB_SRCS})
    get_filename_component(_base ${_src} NAME_WE)
    list(APPEND LIB_SRC_HDRS ${_base}.h)
  endforeach()
  vtk_module_add_module(VTK::MYPROJECT
    SOURCES ${LIB_SRCS} ${LIB_SPECIAL}
    HEADERS ${LIB_SRC_HDRS} ${LIB_HDRS})
  vtk_module_link(VTK::MYPROJECT
    PRIVATE )
endif()
