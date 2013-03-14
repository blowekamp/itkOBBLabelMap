
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
#include "itkOrientedBoundingBoxLabelObject.h"
#include "itkStatisticsLabelObject.h"
#include <cstdlib>

#include "itkTestingMacros.h"

int itkOrientedBoundingBoxLabelObjectTest( int , char ** )
{
  // The basic goal of this test to cover the methods of the class


  // check some template parameters for the super-class
  typedef itk::OrientedBoundingBoxLabelObject< int, 3 > LabelObject1Type;
  typedef itk::OrientedBoundingBoxLabelObject< int, 3, itk::LabelObject<int,3> > LabelObject2Type;

  LabelObject1Type::Pointer labelObject1 = LabelObject1Type::New();
  LabelObject2Type::Pointer labelObject2 = LabelObject2Type::New();

  EXERCISE_BASIC_OBJECT_METHODS( labelObject1, LabelObject1Type );

  LabelObject1Type::ConstPointer clabelObject1 = labelObject1.GetPointer();

  // todo test set/get methods...

  return EXIT_SUCCESS;
}
