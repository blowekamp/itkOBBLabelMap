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
#include "itkOrientedBoundingBoxLabelObject.h"
#include "itkShapeLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"

#include "itkLabelImageToLabelMapFilter.h"

#include "itkImageFileReader.h"

#include "itkResampleImageFilter.h"

#include "itkImageFileWriter.h"

#include <cstdlib>

#include "itkTestingMacros.h"
#include "itkFilterWatcher.h"

int itkOrientedBoundingBoxLabelMapFilterTest2( int argc, char **argv )
{

  if (argc < 5)
    {
    std::cerr << "Usage: " << argv[0] << "imageFileName labelImageFieName outputImageFileName labelID" << std::endl;
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

  typedef itk::OrientedBoundingBoxLabelObject< LabelPixelType, ImageDimension > LabelObjectType;
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


  typedef itk::OrientedBoundingBoxLabelMapFilter<LabelMapType> OBBLabelMapFilter;

  OBBLabelMapFilter::Pointer toOBBLabelMap =  OBBLabelMapFilter::New();
  toOBBLabelMap->SetInput(toLabelMap->GetOutput());
  toOBBLabelMap->SetNumberOfThreads(1);
  FilterWatcher watcher(toOBBLabelMap);
  toOBBLabelMap->Update();

  std::cout << "check" << std::endl;

  const LabelObjectType* labelObject = toOBBLabelMap->GetOutput()->GetLabelObject(label);
  labelObject->Print( std::cout );


  std::cout << "Resampling intensity image..." << std::endl;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( imageFileName );
  reader->UpdateLargestPossibleRegion();
  ImageType::Pointer image = reader->GetOutput();


  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilter;
  ResampleFilter::Pointer resampler = ResampleFilter::New();

  LabelImageType::SpacingType outSpacing;
  outSpacing.Fill(toOBBLabelMap->GetOutput()->GetSpacing()[0]);

  LabelImageType::SizeType outSize;
  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    outSize[i] = itk::Math::Floor<itk::SizeValueType>( labelObject->GetOrientedBoundingBoxSize()[i]/ outSpacing[i] );
    }

  resampler->SetOutputDirection(labelObject->GetOrientedBoundingBoxDirection());
  resampler->SetOutputOrigin(labelObject->GetOrientedBoundingBoxOrigin());
  resampler->SetOutputSpacing(outSpacing);
  resampler->SetSize(outSize);

  resampler->SetInput(image);

  resampler->UpdateLargestPossibleRegion();

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputImageFileName);
  writer->SetInput(resampler->GetOutput());
  writer->Update();

  return EXIT_SUCCESS;
}
