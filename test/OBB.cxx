#include "itkImageFileReader.h"
#include "itkImage.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"
#include <iostream>
#include <string>

#include "itkTestDriverIncludeRequiredIOFactories.h"

int main( int argc, char *argv[] )
{

  RegisterRequiredFactories();

  if (argc < 4)
    {
    std::cerr << "Not enough arguments." << std::endl;
    return 0;
    }

  std::string imageFileName = argv[1];
  std::string labelImageFileName = argv[2];
  itk::SizeValueType label = atoi(argv[3]);

  const unsigned int ImageDimension = 3;
  typedef uint32_t PixelType;
  typedef itk::Image<PixelType, ImageDimension> ImageType;


  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( labelImageFileName );
  reader->UpdateLargestPossibleRegion();


  ImageType::Pointer image = reader->GetOutput();

  typedef itk::StatisticsLabelObject<PixelType, ImageType::ImageDimension> LabelObjectType;

  typedef itk::LabelMap<LabelObjectType> LabelMapType;
  typedef itk::LabelImageToShapeLabelMapFilter<ImageType, LabelMapType> ShapeLabelMapFilter;

  typedef vnl_matrix<double> MatrixType;
  typedef vnl_vector<double> VectorType;


  ShapeLabelMapFilter::Pointer stats = ShapeLabelMapFilter::New();
  stats->SetInput(image);
  stats->ComputePerimeterOn();
  stats->UpdateLargestPossibleRegion();
  LabelMapType::Pointer labelCollection = stats->GetOutput();
  LabelObjectType *labelObject = labelCollection->GetLabelObject(label);

  const MatrixType rotationMatrix = labelObject->GetPrincipalAxes().GetVnlMatrix();
  const LabelObjectType::CentroidType centroid = labelObject->GetCentroid();
  const unsigned int numLines = labelObject->GetNumberOfLines();


  std::cout << "using label object: ";
  labelObject->Print(std::cout);
  std::cout << std::endl;

  // Create a matrix where the columns are the physical points of the
  // start and end of each RLE line from the label map, relative to
  // the centroid
  MatrixType pixelLocations( ImageDimension, labelObject->GetNumberOfLines()*2 );
  for( unsigned int l = 0; l < numLines; ++l )
    {
    LabelObjectType::LineType line = labelObject->GetLine(l);

    // add start index of line as physical point
    ImageType::IndexType idx = line.GetIndex();
    ImageType::PointType pt;
    image->TransformIndexToPhysicalPoint(idx, pt);
    for(unsigned int j = 0; j < ImageDimension; ++j)
      {
      pixelLocations(j,l*2) = pt[j] - centroid[j];
      }

    // add end index of line as physical point
    idx[0] +=  line.GetLength()-1;
    image->TransformIndexToPhysicalPoint(idx, pt);
    for(unsigned int j = 0; j < ImageDimension; ++j)
      {
      pixelLocations(j,l*2+1) = pt[j] - centroid[j];
      }
    }


  // Project the physical points onto principal axes
  MatrixType transformedPixelLocations = rotationMatrix * pixelLocations;

  // find the bounds in the projected domain
  VectorType proj_min(ImageDimension, 0);
  VectorType proj_max(ImageDimension, 0);

  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    const double value = transformedPixelLocations(i, 0);
    proj_min[i] = value;
    proj_max[i] = value;
    }

  for ( unsigned int column = 1; column < transformedPixelLocations.columns(); column++ )
    {
    for ( unsigned int i = 0; i < ImageDimension; ++i )
      {
      const double value = transformedPixelLocations(i, column);
      proj_min[i] = std::min(proj_min[i], value);
      proj_max[i] = std::max(proj_max[i], value);
      }
    }

  // the center of the input voxel should the origin?
  VectorType proj_origin = proj_min;


  const itk::Vector<double, ImageDimension> gridOffset = 0.5*image->GetSpacing();
  itk::Vector<double, ImageDimension> physicalOffset;
  image->TransformLocalVectorToPhysicalVector(gridOffset, physicalOffset);
  VectorType proj_offset = rotationMatrix * physicalOffset.GetVnlVector();

  std::cout << "proj_min w/o offset: " << proj_min << std::endl;
  std::cout << "proj_max w/o offset: " << proj_max << std::endl;

  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    proj_min[i] -= proj_offset[i];
    proj_max[i] += proj_offset[i];
    }


  VectorType origin =  rotationMatrix.transpose() *  proj_origin; // offset
                                                                  // by spacing....
  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    origin[i] += centroid[i];
    }

  ImageType::SizeType size;
  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    size[i] = vnl_math_abs(proj_max[i]-proj_min[i]);
    }

  std::cout << "proj_min: " << proj_min << std::endl;
  std::cout << "proj_max: " << proj_max << std::endl;
  std::cout << "origin: " << origin << std::endl;
  std::cout << "size: " << size << std::endl;


  ImageType::DirectionType direction( rotationMatrix.transpose() );
  std::cout << "direction: " << direction;

  return 0;
}
