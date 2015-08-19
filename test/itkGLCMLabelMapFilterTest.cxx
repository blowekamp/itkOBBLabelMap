/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include "itkGLCMLabelObject.h"
#include "itkGLCMLabelMapFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include <cstdlib>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkTestingMacros.h"

int itkGLCMLabelMapFilterTest( int argc, char **argv )
{

  if ( argc < 3 )
  {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << argv[0] << "imageFileName labelImageFileName" << std::endl;
    return EXIT_FAILURE;
  }


  const std::string imageFileName = argv[1];
  const std::string labelImageFileName = argv[2];


  // The basic goal of this test to cover the methods of the class

  const unsigned int Dimension = 2;

  // check some template parameters for the super-class
  typedef itk::GLCMLabelObject< int, Dimension > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;
  typedef itk::Image< float, Dimension > ImageType;
  typedef itk::Image< unsigned short, Dimension > LabelImageType;
  typedef itk::LabelImageToLabelMapFilter<LabelImageType, LabelMapType> ToLabelMapFilterType;
  typedef itk::GLCMLabelMapFilter< LabelMapType, ImageType > FilterType;


  FilterType::Pointer filter = FilterType::New();

  EXERCISE_BASIC_OBJECT_METHODS( filter, FilterType );


  ImageType::Pointer featureImage;

  {
  std::cout << "Reading feature image..." << std::endl;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( imageFileName );
  reader->UpdateLargestPossibleRegion();
  featureImage = reader->GetOutput();
  }

  ToLabelMapFilterType::Pointer toLabelMap = ToLabelMapFilterType::New();

  {
  std::cout << "Reading label image..." << std::endl;
  typedef itk::ImageFileReader<LabelImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( labelImageFileName );

  toLabelMap->SetInput(reader->GetOutput());
  toLabelMap->Update();
  }

  filter->SetInput( toLabelMap->GetOutput() );
  filter->SetFeatureImage( featureImage );


  FilterType::RadiusType window;
  window.Fill( 1 );

  FilterType::OffsetType o1 = {{1,0}};
  FilterType::OffsetType o2 = {{0,1}};
  FilterType::OffsetType o3 = {{-2,-2}};

  FilterType::OffsetVectorType offsets;
  offsets.push_back( o1 );
  offsets.push_back( o2 );
  offsets.push_back( o3 );

  filter->SetOffsets( offsets );
  filter->SetNumberOfBinsPerAxis( 16 );

  std::cout << "Computing GLCM per label..." << std::endl;

  filter->Update();

  const unsigned int numberOfLabels = filter->GetOutput()->GetNumberOfLabelObjects();

  std::cout << "Number Labels: " << numberOfLabels << std::endl;

 for(unsigned int i = 0; i < numberOfLabels; ++i)
   {
   const LabelObjectType* labelObject =  filter->GetOutput()->GetNthLabelObject(i);

   std::cout << "Label: " << labelObject->GetLabel() << std::endl;

#define printHelper(name) std::cout << "\t" << #name << ": " << labelObject->Get##name() << std::endl;
    printHelper( Energy );
    printHelper( Entropy );
    printHelper( Correlation );
    printHelper( InverseDifferenceMoment );
    printHelper( Inertia );
    printHelper( ClusterShade );
    printHelper( ClusterProminence );
    printHelper( HaralickCorrelation );

   }




  // todo test set/get methods...

  return EXIT_SUCCESS;
}
