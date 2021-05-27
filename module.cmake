set(DOCUMENTATION
"This package contains."
)

vtk_module(vtkMYPROJECT
  DESCRIPTION
    "${DOCUMENTATION}"
  DEPENDS
    vtkCommonCore
    vtkRenderingCore
    vtkInteractionStyle
  PRIVATE_DEPENDS
    vtkRenderingOpenGL2
  COMPILE_DEPENDS
    vtkInteractionStyle
    vtkRendering${VTK_RENDERING_BACKEND}
)
