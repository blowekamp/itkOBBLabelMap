itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set( ITK${itk-module}_DATA_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/data/ )

set(ITK${itk-module}Tests
  itkOBBExample.cxx
  itkBoundingBoxImageLabelMapFilterTest.cxx
  itkOrientedBoundingBoxImageLabelMapFilterTest1.cxx
  itkOrientedBoundingBoxImageLabelMapFilterTest2.cxx
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
    DATA{${ITK_DATA_ROOT}/Input/cthead1Label.png} DATA{${ITK_DATA_ROOT}/Input/cthead1Label.png}
    ${ITK_TEST_OUTPUT_DIR}/itkOrientedBoundingBoxLabelMapFilterTest2.mha 94)

itk_add_test(NAME itkBoundingBoxImageLabelMapFilterTest1
  COMMAND ${itk-module}TestDriver itkBoundingBoxImageLabelMapFilterTest1)

itk_add_test(NAME itkBoundingBoxImageLabelMapFilterTest2
  COMMAND ${itk-module}TestDriver itkBoundingBoxImageLabelMapFilterTest2
    DATA{${ITK_DATA_ROOT}/Input/cthead1Label.png} DATA{${ITK_DATA_ROOT}/Input/cthead1Label.png}
    ${ITK_TEST_OUTPUT_DIR}/itkBoundingBoxLabelMapFilterTest2.mha 94)

itk_add_test(NAME itkOrientedBoundingBoxImageLabelMapFilterTest
  COMMAND ${itk-module}TestDriver itkBoundingBoxImageLabelMapFilterTest
    DATA{${ITK_DATA_ROOT}/Input/cthead1Label.png} DATA{${ITK_DATA_ROOT}/Input/cthead1Label.png}
    ${ITK_TEST_OUTPUT_DIR}/itkOrientedBoundingBoxImageLabelMapFilterTest.mha 94)


itk_add_test(NAME itkOBBExample1
  WORKING_DIRECTORY ${ITK_TEST_OUTPUT_DIR}
  COMMAND ${itk-module}TestDriver itkOBBExample
  ${ITK${itk-module}_DATA_ROOT}/jelly_beans.png ${ITK${itk-module}_DATA_ROOT}/jelly_beans_seg.png)
