itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(ITK${itk-module}Tests
  itkOrientedBoundingBoxLabelObjectTest.cxx
)

CreateTestDriver(${itk-module}  "${${itk-module}-Test_LIBRARIES}" "${ITK${itk-module}Tests}")

itk_add_test(NAME itkOrientedBoundingBoxLabelObjectTest
  COMMAND ${itk-module}TestDriver itkOrientedBoundingBoxLabelObjectTest )

add_executable(itkOBBLabelMapExample OBB.cxx )

target_link_libraries(itkOBBLabelMapExample ${${itk-module}-Test_LIBRARIES} )
