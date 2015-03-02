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

#include "itkBoundingBoxImageLabelMapFilter.h"
#include "itkAttributeImageLabelObject.h"
#include "itkShapeLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"

#include "itkLabelImageToLabelMapFilter.h"

#include "itkImageFileReader.h"

#include "itkImageFileWriter.h"

#include <cstdlib>

#include "itkTestingMacros.h"
#include "itkFilterWatcher.h"

int itkBoundingBoxImageLabelMapFilterTest( int argc, char **argv )
{

  if (argc < 5)
    {
    std::cerr << "Usage: " << argv[0] << "imageFileName labelImageFileName outputImageFileName labelID" << std::endl;
    return 0;
    }

  std::string imageFileName = argv[1];
  std::string labelImageFileName = argv[2];
  std::string outputImageFileName = argv[3];
  itk::SizeValueType label = atoi(argv[4]);

  const unsigned int ImageDimension = 3;
  typedef unsigned int                               LabelPixelType;
  typedef itk::Image<LabelPixelType, ImageDimension> LabelImageType;

  typedef itk::Image<float, ImageDimension> ImageType;

  typedef itk::AttributeImageLabelObject< LabelPixelType, ImageDimension, ImageType > LabelObjectType;
  typedef itk::LabelMap<LabelObjectType> LabelMapType;
  typedef itk::LabelImageToLabelMapFilter<LabelImageType, LabelMapType> ToLabelMapFilterType;
  ToLabelMapFilterType::Pointer toLabelMap = ToLabelMapFilterType::New();

  // read input image and convert to label map
  {
  typedef itk::ImageFileReader<LabelImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( labelImageFileName );
  toLabelMap->SetInput(reader->GetOutput());
  toLabelMap->Update();
  }

  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer featureReader = ImageReaderType::New();
  featureReader->SetFileName( imageFileName );
  featureReader->UpdateLargestPossibleRegion();
  ImageType::Pointer image = featureReader->GetOutput();


  typedef itk::BoundingBoxImageLabelMapFilter<LabelMapType> BBILabelMapFilter;

  BBILabelMapFilter::Pointer toBBILabelMap =  BBILabelMapFilter::New();
  toBBILabelMap->SetInput(toLabelMap->GetOutput());
  //toBBILabelMap->SetNumberOfThreads(1);
  toBBILabelMap->SetFeatureImage( featureReader->GetOutput() );
  FilterWatcher watcher(toBBILabelMap);
  toBBILabelMap->Update();

  const LabelObjectType* labelObject = toBBILabelMap->GetOutput()->GetLabelObject(label);
  //labelObject->Print( std::cout );


  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputImageFileName);
  writer->SetInput(labelObject->GetAttributeImage());
  writer->Update();


  return EXIT_SUCCESS;
}
