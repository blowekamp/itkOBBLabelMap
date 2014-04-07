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

#include "itkLabelShapeStatisticsImageFilter.h"
#include "itkImageFileReader.h"
#include "itkSimpleFilterWatcher.h"


int itkLabelShapeStatisticsImageFilterTest( int argc, char ** argv )
{
  if (argc < 3)
    {
    std::cerr << "Usage: " << argv[0] << "imageFileName labelImageFileName" << std::endl;
    return 0;
    }

  const unsigned int Dimension = 2;
  typedef itk::Image<float, Dimension>        IntensityImageType;
  typedef itk::Image<unsigned int, Dimension> LabelImageType;

  typedef itk::ImageFileReader<IntensityImageType> Reader1Type;
  Reader1Type::Pointer reader1 = Reader1Type::New();
  reader1->SetFileName(argv[1]);

  typedef itk::ImageFileReader<LabelImageType>     Reader2Type;
  Reader2Type::Pointer reader2 = Reader2Type::New();
  reader2->SetFileName(argv[2]);

  typedef itk:: LabelShapeStatisticsImageFilter<IntensityImageType, LabelImageType> ShapeStatisticsType;
  ShapeStatisticsType::Pointer filter = ShapeStatisticsType::New();

  filter->SetInput(reader1->GetOutput());
  filter->SetLabelImage(reader2->GetOutput());

  itk::SimpleFilterWatcher watcher(filter, "filter");

  filter->Update();

  ShapeStatisticsType::ValidLabelValuesContainerType labels = filter->GetValidLabelValues();
  ShapeStatisticsType::ValidLabelValuesContainerType::const_iterator iter = labels.begin();

  std::cout << "Number Labels: " << labels.size() << std::endl;
  while ( iter != labels.end() )
    {
    std::cout << "Label: " << *iter << std::endl;
#define printHelper(name) std::cout << "\t" << #name << ": " << filter->Get##name(*iter) << std::endl;
    printHelper( BoundingBox );
    printHelper( PhysicalSize );
    printHelper( NumberOfPixels );
    printHelper( Centroid );
    printHelper( NumberOfPixelsOnBorder );
    printHelper( PerimeterOnBorder );
    printHelper( FeretDiameter );
    printHelper( PrincipalMoments );
    printHelper( PrincipalAxes );
    printHelper( Elongation );
    printHelper( Perimeter );
    printHelper( Roundness );
    printHelper( EquivalentSphericalRadius );
    printHelper( EquivalentEllipsoidDiameter );
    printHelper( Flatness );
    printHelper( PerimeterOnBorderRatio );

    printHelper( OrientedBoundingBoxVertices );
    printHelper( OrientedBoundingBoxOrigin );
    printHelper( OrientedBoundingBoxDirection );
    printHelper( OrientedBoundingBoxSize );
    ++iter;
    }


  return 0;
}
