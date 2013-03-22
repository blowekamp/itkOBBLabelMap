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


#include "itkTestingMacros.h"
#include "itkVectorImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkConvertLabelMapFilter.h"


#include "itkOrientedBoundingBoxLabelMapFilter.h"
#include "itkOrientedBoundingBoxImageLabelMapFilter.h"
#include "itkBoundingBoxImageLabelMapFilter.h"
#include "itkOrientedBoundingBoxLabelObject.h"
#include "itkAttributeImageLabelObject.h"
#include "itkShapeLabelMapFilter.h"
#include "itkStatisticsLabelObject.h"

#include "itkLabelImageToLabelMapFilter.h"


template<class  TFeatureImageType >
int itkOBBExampleImp( const std::string &imageFileName,
                      const std::string &labelImageFileName)
{
  const unsigned int ImageDimension = TFeatureImageType::ImageDimension;
  typedef unsigned long LabelPixelType;
  typedef itk::Image<LabelPixelType, ImageDimension> LabelImageType;

  typedef TFeatureImageType ImageType;

  typedef itk::OrientedBoundingBoxLabelObject< LabelPixelType, ImageDimension > OBBLabelObjectType;
  typedef itk::AttributeImageLabelObject< LabelPixelType, ImageDimension, ImageType, OBBLabelObjectType > LabelObjectType;
  typedef itk::LabelMap<LabelObjectType> LabelMapType;
  typedef itk::LabelImageToLabelMapFilter<LabelImageType, LabelMapType> ToLabelMapFilterType;
  typename ToLabelMapFilterType::Pointer toLabelMap = ToLabelMapFilterType::New();

  // read input image and convert to label map
  {
  std::cout << "Reading label image and converting to label map..." << std::endl;
  typedef itk::ImageFileReader<LabelImageType> ImageReaderType;
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( labelImageFileName );
  toLabelMap->SetInput(reader->GetOutput());
  toLabelMap->Update();
  }

  typename ImageType::Pointer featureImage;

  {
  std::cout << "Reading feature image..." << std::endl;
  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( imageFileName );
  reader->UpdateLargestPossibleRegion();
  featureImage = reader->GetOutput();
  }

  std::cout << "Computing label attributes and oriented bounding box..." << std::endl;
  typedef itk::OrientedBoundingBoxLabelMapFilter<LabelMapType> OBBLabelMapFilter;
  typename OBBLabelMapFilter::Pointer toOBBLabelMap =  OBBLabelMapFilter::New();
  toOBBLabelMap->SetInput(toLabelMap->GetOutput());
  toOBBLabelMap->Update();

  for(unsigned int i = 0; i < toOBBLabelMap->GetOutput()->GetNumberOfLabelObjects(); ++i)
    {
    LabelObjectType* labelObject = toOBBLabelMap->GetOutput()->GetNthLabelObject(i);
    labelObject->Print(std::cout);
    }

  std::cout << "Computing oriented images..." << std::endl;
  typedef itk::OrientedBoundingBoxImageLabelMapFilter<LabelMapType, ImageType, itk::InPlaceLabelMapFilter< LabelMapType > > OBBILabelMapFilter;
  typename OBBILabelMapFilter::Pointer toOBBI = OBBILabelMapFilter::New();
  toOBBI->SetInput( toOBBLabelMap->GetOutput() );
  toOBBI->SetFeatureImage( featureImage );
  toOBBI->SetAttributeImageSpacing( featureImage->GetSpacing() );

  double spacing = *std::min_element( featureImage->GetSpacing().Begin(), featureImage->GetSpacing().End());
  toOBBI->SetPaddingOffset( spacing*-0.5 );
  toOBBI->UpdateLargestPossibleRegion();

  typedef itk::ImageFileWriter<ImageType> WriterType;
  typename WriterType::Pointer writer = WriterType::New();

  std::cout << "Writing oriented images..." << std::endl;
  for(unsigned int i = 0; i < toOBBI->GetOutput()->GetNumberOfLabelObjects(); ++i)
    {
    LabelObjectType* labelObject = toOBBI->GetOutput()->GetNthLabelObject(i);

    std::ostringstream ss;
    ss << "obb_" << labelObject->GetLabel() << itksys::SystemTools::GetFilenameName( imageFileName );

    try
      {
      writer->SetFileName(ss.str());
      writer->SetInput(labelObject->GetAttributeImage());
      writer->Update();
      }
    catch(std::exception &e)
      {
      std::cerr << e.what();
      std::cerr << "Exception while writing oriented image for : " << std::endl;
      labelObject->Print(std::cout);
      }
    }

  std::cout << "Computing bounding box images..." << std::endl;
  typedef itk::BoundingBoxImageLabelMapFilter<LabelMapType, ImageType, itk::InPlaceLabelMapFilter< LabelMapType > > BBILabelMapFilter;
  typename BBILabelMapFilter::Pointer toBBI = BBILabelMapFilter::New();
  toBBI->SetInput( toOBBLabelMap->GetOutput() );
  toBBI->SetFeatureImage( featureImage );
  toBBI->UpdateLargestPossibleRegion();


  std::cout << "Writing bounding box images..." << std::endl;
  for(unsigned int i = 0; i < toBBI->GetOutput()->GetNumberOfLabelObjects(); ++i)
    {
    LabelObjectType* labelObject = toBBI->GetOutput()->GetNthLabelObject(i);

    std::ostringstream ss;
    ss << "bb_" << labelObject->GetLabel() << itksys::SystemTools::GetFilenameName( imageFileName );

    try
      {
      writer->SetFileName(ss.str());
      writer->SetInput(labelObject->GetAttributeImage());
      writer->Update();
      }
    catch(std::exception &e)
      {
      std::cerr << e.what();
      std::cerr << "Exception while writing bounding box image for : " << std::endl;
      labelObject->Print(std::cout);
      }
    }

  return 0;
}


template< class TComponent >
int itkOBBExampleDispatch( const std::string &imageFileName,
                      const std::string &labelImageFileName,
                      unsigned int dimensions,
                      unsigned int numberOfComponents )
{
  if ( dimensions == 2 )
    {
    if (numberOfComponents > 1)
      {
      return itkOBBExampleImp<itk::VectorImage<TComponent,2> >(imageFileName, labelImageFileName);
      }
    return itkOBBExampleImp<itk::Image<TComponent,2> >(imageFileName, labelImageFileName);
    }
  else if (dimensions == 3 )
    {
     if (numberOfComponents == 3)
       {
       return itkOBBExampleImp<itk::Image<itk::Vector<TComponent>,3> >(imageFileName, labelImageFileName);
       }
     else if (numberOfComponents > 1)
      {
      return itkOBBExampleImp<itk::VectorImage<TComponent,3> >(imageFileName, labelImageFileName);
      }
    return itkOBBExampleImp<itk::Image<TComponent,3> >(imageFileName, labelImageFileName);
    }
  else
    {
    std::cerr << "Only 2D and 3D image supported." << std::endl;
    return 1;
    }
}

int itkOBBExample(int argc, char* argv[] )
{

  if ( argc < 3 )
  {
    std::cerr << "Missing arguments" << std::endl;
    std::cerr << "Usage: " << argv[0] << "imageFileName labelImageFileName" << std::endl;
    return EXIT_FAILURE;
  }


  const std::string imageFileName = argv[1];
  const std::string labelImageFileName = argv[2];


  itk::ImageIOBase::Pointer iobase =
    itk::ImageIOFactory::CreateImageIO( imageFileName.c_str(), itk::ImageIOFactory::ReadMode);

  if ( iobase.IsNull() )
    {
    itkGenericExceptionMacro( "Unable to determine ImageIO reader for \"" << imageFileName << "\"" );
    }

  iobase->SetFileName( imageFileName );
  iobase->ReadImageInformation();


  const itk::ImageIOBase::IOComponentType componentType = iobase->GetComponentType();
  const unsigned int dimension = iobase->GetNumberOfDimensions();
  const unsigned int numComp = iobase->GetNumberOfComponents();

  switch(componentType)
    {
    case itk::ImageIOBase::CHAR:
      return itkOBBExampleDispatch< char >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::UCHAR:
      return itkOBBExampleDispatch< unsigned char >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::SHORT:
      return itkOBBExampleDispatch< short >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::USHORT:
      return itkOBBExampleDispatch< unsigned short >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::INT:
      return itkOBBExampleDispatch< int >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::UINT:
      return itkOBBExampleDispatch< unsigned int >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::ULONG:
      return itkOBBExampleDispatch< unsigned long >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::LONG:
      return itkOBBExampleDispatch< long >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::FLOAT:
      return itkOBBExampleDispatch< float >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::DOUBLE:
      return itkOBBExampleDispatch< double >( imageFileName, labelImageFileName, dimension, numComp );
    case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
    default:
      itkGenericExceptionMacro( "Input image is a real, long or a unknown component type" );
    }

  std::cerr << "Unexcpected program flow!" << std::endl;
  return EXIT_FAILURE;
}
