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

#include "itkOrientedBoundingBoxLabelMapFilter.h"
#include "itkShapeLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"
#include "itkOrientedBoundingBoxLabelObject.h"
#include <cstdlib>

#include "itkTestingMacros.h"

int itkOrientedBoundingBoxLabelMapFilterTest1( int , char ** )
{
  // The basic goal of this test is to cover the methods of the class

  const unsigned int ImageDimension = 3;
  typedef unsigned int LabelPixelType;
  //typedef itk::Image<LabelPixelType, ImageDimension> LabelImageType;


  // check some template parameters for the super-class
  typedef itk::OrientedBoundingBoxLabelObject< LabelPixelType, ImageDimension > LabelObjectType;

  typedef itk::LabelMap<LabelObjectType>                       LabelMapType;
  typedef itk::OrientedBoundingBoxLabelMapFilter<LabelMapType> LabelMapFilterType;

  LabelObjectType::Pointer labelObject = LabelObjectType::New();
  LabelMapType::Pointer  labelMap =  LabelMapType::New();

  LabelMapFilterType::Pointer labelMapFilter =  LabelMapFilterType::New();

  EXERCISE_BASIC_OBJECT_METHODS( labelMapFilter, LabelMapFilterType );

  return EXIT_SUCCESS;
}
