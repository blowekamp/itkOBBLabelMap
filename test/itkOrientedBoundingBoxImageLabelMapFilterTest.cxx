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
#include "itkAttributeImageLabelObject.h"
#include "itkOrientedBoundingBoxLabelObject.h"
#include "itkShapeLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"

#include "itkLabelImageToLabelMapFilter.h"

#include "itkImageFileReader.h"

#include "itkImageFileWriter.h"

#include <cstdlib>

#include "itkTestingMacros.h"
#include "itkFilterWatcher.h"

int itkOrientedBoundingBoxImageLabelMapFilterTest( int argc, char **argv )
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
  typedef unsigned int LabelPixelType;
  typedef itk::Image<LabelPixelType, ImageDimension> LabelImageType;

  typedef itk::Image<float, ImageDimension> ImageType;

  typedef itk::OrientedBoundingBoxLabelObject< LabelPixelType, ImageDimension > OBBLabelObjectType;
  typedef itk::AttributeImageLabelObject< LabelPixelType, ImageDimension, ImageType, OBBLabelObjectType > LabelObjectType;
  typedef itk::LabelMap<LabelObjectType> LabelMapType;
  typedef itk::LabelImageToLabelMapFilter<LabelImageType, LabelMapType> ToLabelMapFilterType;
  ToLabelMapFilterType::Pointer toLabelMap = ToLabelMapFilterType::New();

  FilterWatcher watcher2(toLabelMap);

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


  typedef itk::OrientedBoundingBoxImageLabelMapFilter<LabelMapType> OBBILabelMapFilter;

  OBBILabelMapFilter::Pointer toOBBILabelMap =  OBBILabelMapFilter::New();
  toOBBILabelMap->SetInput(toLabelMap->GetOutput());
  toOBBILabelMap->SetAttributeImageSpacing(featureReader->GetOutput()->GetSpacing());
  toOBBILabelMap->SetFeatureImage( featureReader->GetOutput() );
  FilterWatcher watcher(toOBBILabelMap);
  toOBBILabelMap->Update();

  const LabelObjectType* labelObject = toOBBILabelMap->GetOutput()->GetLabelObject(label);
  //labelObject->Print( std::cout );


  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputImageFileName);
  writer->SetInput(labelObject->GetAttributeImage());
  writer->Update();


  return EXIT_SUCCESS;
}
