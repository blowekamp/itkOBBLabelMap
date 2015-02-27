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

#include "itkOrientedBoundingBoxImageLabelMapFilter.h"
#include "itkShapeLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"
#include "itkAttributeImageLabelObject.h"
#include "itkOrientedBoundingBoxLabelObject.h"
#include <cstdlib>

#include "itkNearestNeighborInterpolateImageFunction.h"

#include "itkTestingMacros.h"

int itkOrientedBoundingBoxImageLabelMapFilterTest1( int , char ** )
{
  // The basic goal of this test is to cover the methods of the class

  const unsigned int ImageDimension = 3;
  typedef unsigned int LabelPixelType;
  //typedef itk::Image<LabelPixelType, ImageDimension> LabelImageType;
  typedef itk::Image<double, ImageDimension> ImageType;

  typedef itk::OrientedBoundingBoxLabelObject< LabelPixelType, ImageDimension > OBBLabelObjectType;
  typedef itk::AttributeImageLabelObject< LabelPixelType, ImageDimension, ImageType, OBBLabelObjectType > LabelObjectType;
  typedef itk::LabelMap<LabelObjectType> LabelMapType;
  typedef itk::OrientedBoundingBoxImageLabelMapFilter<LabelMapType> LabelMapFilter;

  LabelObjectType::Pointer labelObject = LabelObjectType::New();
  LabelMapType::Pointer  labelMap =  LabelMapType::New();

  LabelMapFilter::Pointer labelMapFilter =  LabelMapFilter::New();

  EXERCISE_BASIC_OBJECT_METHODS( labelMapFilter, LabelMapFilter );

  LabelMapFilter::SpacingType offset;
  offset.Fill(2);

  labelMapFilter->SetPaddingOffset( 2.0 );
  TEST_SET_GET_VALUE( offset, labelMapFilter->GetPaddingOffset() );

  offset.Fill(3);
  labelMapFilter->SetPaddingOffset( offset );
  TEST_SET_GET_VALUE( offset, labelMapFilter->GetPaddingOffset() );
  labelMapFilter->SetPaddingOffset( offset );


  LabelMapFilter::SpacingType spacing;
  spacing.Fill(2);

  labelMapFilter->SetAttributeImageSpacing( spacing );
  TEST_SET_GET_VALUE( spacing, labelMapFilter->GetAttributeImageSpacing() );
  labelMapFilter->SetAttributeImageSpacing( spacing );


  typedef itk::NearestNeighborInterpolateImageFunction< ImageType, double >  InterpolatorType;
  InterpolatorType::Pointer interp =   InterpolatorType::New();
  labelMapFilter->SetInterpolator( interp );
  TEST_SET_GET( interp, labelMapFilter->GetInterpolator() );
  labelMapFilter->SetInterpolator( interp );

  labelMapFilter->SetDefaultPixelValue(1.0);
  TEST_SET_GET_VALUE( 1.0,  labelMapFilter->GetDefaultPixelValue() );
  labelMapFilter->SetDefaultPixelValue(1.0);

  return EXIT_SUCCESS;
}
