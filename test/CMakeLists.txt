itk_module_test()
set(ITKExternalTemplateTests
itkExternalTemplateTest.cxx
)

CreateTestDriver(ITKExternalTemplate  "${ITKExternalTemplate-Test_LIBRARIES}" "${ITKExternalTemplateTests}")

itk_add_test(NAME itkExternalTemplateTest
      COMMAND ITKExternalTemplateTestDriver itkExternalTemplateTest)
