set(TEST_SRCS
  TestInteractorStyleGame.cxx
)

if(DEFINED VTK_MODULE_ENABLE_VTK_MYPROJECT AND NOT MYPROJECT_EXTERNAL_BUILD)
  # When building as a remote module for VTK 9 (or VTK 8.90+)
  vtk_add_test_cxx(vtkMYPROJECTCxxTests TEST_NAMES NO_VALID ${TEST_SRCS})
  vtk_test_cxx_executable(vtkMYPROJECTCxxTests TEST_NAMES)

elseif(Module_vtkMYPROJECT)
  # When building as a remote module for older versions of VTK
  vtk_add_test_cxx(vtkMYPROJECTCxxTests TEST_NAMES NO_VALID ${TEST_SRCS})
  vtk_test_cxx_executable(vtkMYPROJECTCxxTests TEST_NAMES)

else()

include_directories(${MYPROJECT_INCLUDE_DIRS})

set(BASE_LIBS ${VTK_MYPROJECT_LINK_TARGET} ${VTK_LIBS})

foreach(_src ${TEST_SRCS})
  get_filename_component(_test ${_src} NAME_WE)
  add_executable(${_test} ${_src})
  target_link_libraries(${_test} ${BASE_LIBS})
  get_target_property(_pth ${_test} RUNTIME_OUTPUT_DIRECTORY)
  add_test(${_test} ${_pth}/${_test})
endforeach()

#if(BUILD_PYTHON_WRAPPERS)
if(0)
  # For VTK 9 and up, executable targets use VTK:: namespace prefix
  if(VTK_VERSION VERSION_LESS 8.90)
    set(_vtk vtk)
    set(_python_module_path "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
  else()
    set(_vtk VTK::)
    get_target_property(_python_module_path vtkMYPROJECTPython
                        LIBRARY_OUTPUT_DIRECTORY)
  endif()
  if(NOT VTK_PYTHON_EXE)
    get_target_property(WRAP_PYTHON_PATH ${_vtk}WrapPython
                        LOCATION_<CONFIG>)
    get_filename_component(VTK_EXE_DIR ${WRAP_PYTHON_PATH} PATH)
    find_program(VTK_PYTHON_EXE vtkpython "${VTK_EXE_DIR}")
  endif()
  if(NOT VTK_PYTHON_EXE)
    # fall back to using python rather than vtkpython
    find_package(PythonInterp QUIET)
    mark_as_advanced(PYTHON_EXECUTABLE)
    set(VTK_PYTHON_EXE ${PYTHON_EXECUTABLE})
  endif()
  add_test(TestMYPROJECTPython "${VTK_PYTHON_EXE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/TestMYPROJECTPython.py")
  if(NOT CMAKE_CONFIGURATION_TYPES)
    set_tests_properties(TestMYPROJECTPython PROPERTIES ENVIRONMENT
      "PYTHONPATH=${_python_module_path}")
  endif()
endif()



endif()
