itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(ITK${itk-module}Tests
  itkOBBExample.cxx
  itkBoundingBoxImageLabelMapFilterTest.cxx
  itkOrientedBoundingBoxImageLabelMapFilterTest.cxx
  itkOrientedBoundingBoxLabelObjectTest.cxx
  itkOrientedBoundingBoxLabelMapFilterTest1.cxx
  itkOrientedBoundingBoxLabelMapFilterTest2.cxx
)

CreateTestDriver(${itk-module}  "${${itk-module}-Test_LIBRARIES}"
  "${ITK${itk-module}Tests}")

add_executable(itkOBBLabelMapExample OBB.cxx )

target_link_libraries(itkOBBLabelMapExample ${${itk-module}-Test_LIBRARIES} )


itk_add_test(NAME itkOrientedBoundingBoxLabelObjectTest
  COMMAND ${itk-module}TestDriver itkOrientedBoundingBoxLabelObjectTest )

itk_add_test(NAME itkOrientedBoundingBoxLabelMapFilterTest1
  COMMAND ${itk-module}TestDriver itkOrientedBoundingBoxLabelMapFilterTest1 )

itk_add_test(NAME itkOrientedBoundingBoxLabelMapFilterTest2
  COMMAND ${itk-module}TestDriver itkOrientedBoundingBoxLabelMapFilterTest2
    DATA{${ITK_DATA_ROOT}/Input/cthead1Label.png} DATA{${ITK_DATA_ROOT}/Input/cthead1-markers.png} ${ITK_TEST_OUTPUT_DIR}/itkOrientedBoundingBoxLabelMapFilterTest2.mha 94)
