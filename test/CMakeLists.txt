itk_module_test()

#${itk-module} will be the name of this module and will not need to be
#changed when this module is renamed.

set(ITK${itk-module}Tests
  itkExternalTemplateTest.cxx
)

CreateTestDriver(${itk-module}  "${${itk-module}-Test_LIBRARIES}" "${${itk-module}Tests}")

itk_add_test(NAME itkExternalTemplateTest
  COMMAND ${itk-module}TestDriver itkExternalTemplateTest)
