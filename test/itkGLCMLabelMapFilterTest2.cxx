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
#include "itkImageRegionIterator.h"
#include <cstdlib>

#include "itkTestingMacros.h"

int itkGLCMLabelMapFilterTest2( int, char ** )
{


  // The basic goal of this test to cover the methods of the class

  const unsigned int Dimension = 2;

  typedef unsigned char PixelType;

  // check some template parameters for the super-class
  typedef itk::GLCMLabelObject< PixelType, Dimension > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::Image< PixelType, Dimension > LabelImageType;
  typedef itk::LabelImageToLabelMapFilter<LabelImageType, LabelMapType> ToLabelMapFilterType;
  typedef itk::GLCMLabelMapFilter< LabelMapType, ImageType > FilterType;
  typedef ImageType::RegionType      RegionType;
  typedef ImageType::IndexType       IndexType;
  typedef ImageType::SizeType        SizeType;


  FilterType::Pointer filter = FilterType::New();


  IndexType index;
  index.Fill(0);

  SizeType size;
  size.Fill(5);


  RegionType region(index, size);

  ImageType::Pointer image = ImageType::New();
  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer( 1u );

  {
  ToLabelMapFilterType::Pointer toLabelMap = ToLabelMapFilterType::New();
  toLabelMap->SetInput(image);
  toLabelMap->Update();

  filter->SetInput( toLabelMap->GetOutput() );
  }


  filter->SetFeatureImage( image );



  FilterType::OffsetType o1 = {{1,0}};

  FilterType::OffsetVectorType offsets;
  offsets.push_back( o1 );

  filter->SetOffsets( offsets );
  filter->SetPixelValueMinMax(0u,255u);
  filter->SetNumberOfBinsPerAxis( 256 );

  std::cout << "Computing GLCM per label..." << std::endl;

  filter->Update();

  {
  const LabelObjectType* labelObject =  filter->GetOutput()->GetLabelObject( 1u );

  if (labelObject == ITK_NULLPTR)
    {
    std::cerr << "unable to get 1 label object." << std::endl;
    return EXIT_FAILURE;
    }

  TEST_EXPECT_EQUAL( 1.0, labelObject->GetEnergy() );
  TEST_EXPECT_EQUAL( 0.0, labelObject->GetEntropy() );
  TEST_EXPECT_EQUAL( 0.0, labelObject->GetCorrelation() );
  TEST_EXPECT_EQUAL( 1.0, labelObject->GetInverseDifferenceMoment() );
  TEST_EXPECT_EQUAL( 0.0, labelObject->GetInertia() );
  TEST_EXPECT_EQUAL( 0.0, labelObject->GetClusterShade() );
  TEST_EXPECT_EQUAL( 0.0, labelObject->GetClusterProminence() );
  TEST_EXPECT_TRUE( itk::Math::FloatAlmostEqual( 257.0, labelObject->GetHaralickCorrelation()) );
  }

  typedef itk::ImageRegionIterator<ImageType> ImageIterator;
  ImageIterator outIt( image, region );

  bool bit = true;
  while ( !outIt.IsAtEnd() )
    {
    // copy the input pixel to the output
    outIt.Set(  bit );
    bit = !bit;
    std::cout << bit << " ";
    ++outIt;
    }
  image->Modified();


  std::cout << "Computing GLCM per label..." << std::endl;

  filter->SetFeatureImage( image );
  filter->Update();

  {

  const LabelObjectType* labelObject =  filter->GetOutput()->GetLabelObject( 1u );

  if (labelObject == ITK_NULLPTR)
    {
    std::cerr << "unable to get 1 label object." << std::endl;
    return EXIT_FAILURE;
    }

  TEST_EXPECT_TRUE( itk::Math::FloatAlmostEqual(0.5, labelObject->GetEnergy()) );
  TEST_EXPECT_EQUAL( 1.0, labelObject->GetEntropy() );
  TEST_EXPECT_EQUAL( 1.0, labelObject->GetInertia() );
  }

  return EXIT_SUCCESS;


}
