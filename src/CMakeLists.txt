

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(${itk-module}_SRC
itkSomeFile.cxx
)

add_library(${itk-module} ${${itk-module}_SRC})
target_link_libraries(${itk-module}  ${${itk-module}_LIBRARIES})
itk_module_target(${itk-module})
