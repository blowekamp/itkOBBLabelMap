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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"
#include "itkResampleImageFilter.h"
#include <iostream>
#include <string>

#include "itkTestDriverIncludeRequiredIOFactories.h"

int main( int argc, char *argv[] )
{

  RegisterRequiredFactories();

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
  typedef uint32_t                                   LabelPixelType;
  typedef itk::Image<LabelPixelType, ImageDimension> LabelImageType;

  typedef itk::Image<float, ImageDimension> ImageType;

  LabelImageType::Pointer labelImage;
  {
  typedef itk::ImageFileReader<LabelImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( labelImageFileName );


  std::cout << "Reading labelImage..." << std::endl;
  reader->UpdateLargestPossibleRegion();
  labelImage = reader->GetOutput();
  }


  typedef itk::StatisticsLabelObject<LabelPixelType, ImageDimension> LabelObjectType;

  typedef itk::LabelMap<LabelObjectType> LabelMapType;
  typedef itk::LabelImageToShapeLabelMapFilter<LabelImageType, LabelMapType> ShapeLabelMapFilter;

  typedef vnl_matrix<double> MatrixType;
  typedef vnl_vector<double> VectorType;


  ShapeLabelMapFilter::Pointer stats = ShapeLabelMapFilter::New();
  stats->SetInput(labelImage);
  stats->ComputePerimeterOn();
  std::cout << "Computing shape labels..." << std::endl;
  stats->UpdateLargestPossibleRegion();


  LabelMapType::Pointer labelCollection = stats->GetOutput();
  LabelObjectType *labelObject = labelCollection->GetLabelObject(label);


  std::cout << "Computing label OBB..." << std::endl;
  const MatrixType rotationMatrix = labelObject->GetPrincipalAxes().GetVnlMatrix();
  const LabelObjectType::CentroidType centroid = labelObject->GetCentroid();
  const unsigned int numLines = labelObject->GetNumberOfLines();

  // Create a matrix where the columns are the physical points of the
  // start and end of each RLE line from the label map, relative to
  // the centroid
  MatrixType pixelLocations( ImageDimension, labelObject->GetNumberOfLines()*2 );
  for( unsigned int l = 0; l < numLines; ++l )
    {
    LabelObjectType::LineType line = labelObject->GetLine(l);

    // add start index of line as physical point
    LabelImageType::IndexType idx = line.GetIndex();
    LabelImageType::PointType pt;
    labelImage->TransformIndexToPhysicalPoint(idx, pt);
    for(unsigned int j = 0; j < ImageDimension; ++j)
      {
      pixelLocations(j,l*2) = pt[j] - centroid[j];
      }

    // add end index of line as physical point
    idx[0] += line.GetLength()-1;
    labelImage->TransformIndexToPhysicalPoint(idx, pt);
    for(unsigned int j = 0; j < ImageDimension; ++j)
      {
      pixelLocations(j,l*2+1) = pt[j] - centroid[j];
      }
    }


  // Project the physical points onto principal axes
  MatrixType transformedPixelLocations = rotationMatrix * pixelLocations;

  // find the bounds in the projected domain
  assert( transformedPixelLocations.columns() != 0 );
  VectorType proj_min = transformedPixelLocations.get_column(0);
  VectorType proj_max = transformedPixelLocations.get_column(0);

  for ( unsigned int column = 1; column < transformedPixelLocations.columns(); column++ )
    {
    for ( unsigned int i = 0; i < ImageDimension; ++i )
      {
      const double value = transformedPixelLocations(i, column);
      proj_min[i] = std::min(proj_min[i], value);
      proj_max[i] = std::max(proj_max[i], value);
      }
    }

  std::cout << "proj_min w/o offset: " << proj_min << std::endl;
  std::cout << "proj_max w/o offset: " << proj_max << std::endl;

  // the center of the input voxel should the origin?
  VectorType proj_origin = proj_min;

  // The proj_min/max is from center of pixel to center of pixel. The
  // full extent of the pixels needs to include the offset bits to the
  // corners, projected onto the principle axis basis (rotationMatrix).
  const itk::Vector<double, ImageDimension> gridOffset = 0.5*labelImage->GetSpacing();
  itk::Vector<double, ImageDimension> physicalOffset;
  labelImage->TransformLocalVectorToPhysicalVector(gridOffset, physicalOffset);
  VectorType proj_offset = rotationMatrix * physicalOffset.GetVnlVector();

  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    proj_min[i] -= proj_offset[i];
    proj_max[i] += proj_offset[i];
    }


  LabelImageType::PointType outOrigin((rotationMatrix.transpose()*proj_origin).data_block()); // offset by spacing....

  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    outOrigin[i] += centroid[i];
    }

  itk::Vector<double, ImageDimension> rsize; // real physical size
  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    rsize[i] = vnl_math_abs(proj_max[i]-proj_min[i]);
    }

  std::cout << "proj_min: " << proj_min << std::endl;
  std::cout << "proj_max: " << proj_max << std::endl;
  std::cout << "origin: " << outOrigin << std::endl;
  std::cout << "size: " << rsize << std::endl;


  LabelImageType::DirectionType outDirection( rotationMatrix.transpose() );
  std::cout << "outDirection: " << outDirection;


  LabelImageType::SpacingType outSpacing;
  outSpacing.Fill(10.0);

  LabelImageType::SizeType outSize;
  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    outSize[i] = itk::Math::Floor<itk::SizeValueType>( rsize[i]/ outSpacing[i] );
    }


  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( imageFileName );
  reader->UpdateLargestPossibleRegion();
  ImageType::Pointer image = reader->GetOutput();

  std::cout << "Read Real image..." << std::endl;

  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilter;
  ResampleFilter::Pointer resampler = ResampleFilter::New();

  resampler->SetOutputDirection(outDirection);
  resampler->SetOutputOrigin(outOrigin);
  resampler->SetOutputSpacing(outSpacing);
  resampler->SetSize(outSize);

  resampler->SetInput(image);

  resampler->UpdateLargestPossibleRegion();

  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputImageFileName);
  writer->SetInput(resampler->GetOutput());
  writer->Update();

  return 0;
}
