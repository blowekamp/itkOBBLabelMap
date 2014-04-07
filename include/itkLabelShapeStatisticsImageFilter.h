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
#ifndef __itkLabelShapeStatisticsImageFilter_h
#define __itkLabelShapeStatisticsImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkLabelImageToShapeLabelMapFilter.h"
#include "itkShapeLabelObject.h"
#include "itkProgressAccumulator.h"

namespace itk
{
/** \class LabelShapeStatisticsImageFilter
 *
 */
template< typename TInputImage, typename TLabelImage >
class LabelShapeStatisticsImageFilter:
    public ImageToImageFilter< TInputImage, TInputImage >
{
public:
  /** Standard Self typedef */
  typedef LabelShapeStatisticsImageFilter                Self;
  typedef ImageToImageFilter< TInputImage, TInputImage > Superclass;
  typedef SmartPointer< Self >                           Pointer;
  typedef SmartPointer< const Self >                     ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(LabelShapeStatisticsImageFilter, ImageToImageFilter);

  /** Image related typedefs. */
  typedef TInputImage                      IntensityImageType;
  typedef typename TInputImage::PixelType  PixelType;

  /** Label image related typedefs. */
  typedef TLabelImage                      LabelImageType;
  typedef typename TLabelImage::PixelType  LabelPixelType;

  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int, TLabelImage::ImageDimension);

  /** LabelObject related typedefs */
  typedef itk::ShapeLabelObject<LabelPixelType,  ImageDimension> LabelObjectType;
  typedef typename LabelObjectType::RegionType   LabelObjectRegionType;
  typedef double                                 LabelObjectRealType;
  typedef typename LabelObjectType::CentroidType LabelObjectCentroidType;
  typedef typename LabelObjectType::VectorType   LabelObjectVectorType;
  typedef typename LabelObjectType::MatrixType   LabelObjectMatrixType;

  typedef itk::LabelMap<LabelObjectType>         LabelMapType;
  typedef typename LabelMapType::LabelVectorType ValidLabelValuesContainerType;

  /** Set/Get intensity image input to process object */
  itkSetInputMacro(IntensityImage, IntensityImageType);
  itkGetInputMacro(IntensityImage, IntensityImageType);

  /** Set/Get label image input to process object */
  itkSetInputMacro(LabelImage, LabelImageType);
  itkGetInputMacro(LabelImage, LabelImageType);

  ValidLabelValuesContainerType GetValidLabelValues() const { return this->m_LabelMap->GetLabels();}
  bool HasLabel(LabelPixelType label) const { return this->m_LabelMap->HasLabel(label); }
  SizeValueType GetNumberOfLabels() const { return this->m_LabelMap->GetNumberOfLabelObject(); }

  // todo background, initial condition

#define itkGetLabelObjectAttribute(name, type)                     \
  inline const type &Get##name( LabelPixelType label ) const       \
    {                                                              \
      return this->m_LabelMap->GetLabelObject(label)->Get##name(); \
    }

  // Shape Label Object
  itkGetLabelObjectAttribute( BoundingBox, LabelObjectRegionType );
  itkGetLabelObjectAttribute( PhysicalSize, double );
  itkGetLabelObjectAttribute( NumberOfPixels, SizeValueType );
  itkGetLabelObjectAttribute( Centroid, LabelObjectCentroidType );
  itkGetLabelObjectAttribute( NumberOfPixelsOnBorder, SizeValueType );
  itkGetLabelObjectAttribute( PerimeterOnBorder, double );
  itkGetLabelObjectAttribute( FeretDiameter, double );
  itkGetLabelObjectAttribute( PrincipalMoments, LabelObjectVectorType );
  itkGetLabelObjectAttribute( PrincipalAxes, LabelObjectMatrixType );
  itkGetLabelObjectAttribute( Elongation, double );
  itkGetLabelObjectAttribute( Perimeter, double );
  itkGetLabelObjectAttribute( Roundness, double );
  itkGetLabelObjectAttribute( EquivalentSphericalRadius, double );
  itkGetLabelObjectAttribute( EquivalentEllipsoidDiameter, double );
  itkGetLabelObjectAttribute( Flattness, double );
  itkGetLabelObjectAttribute( PerimeterOnBorderRatio, double );

#if 0
  GetPrincipalAxesToPhysicalAxesTransform AffineTransformPointer
  PhysicalAxesToPrincipalAxesTransform AffineTransformPointer
#endif

#undef itkGetLabelObjectAttribute

#ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
  itkConceptMacro( InputHasNumericTraitsCheck,
                   ( Concept::HasNumericTraits< PixelType > ) );
  itkConceptMacro( LabelInputHasNumericTraitsCheck,
                   ( Concept::HasNumericTraits< LabelPixelType > ) );
  // End concept checking
#endif

protected:
  LabelShapeStatisticsImageFilter()
    {
      this->SetPrimaryInputName( "IntensityImage" );
      this->AddRequiredInputName( "LabelImage", 1 );
    }
  ~LabelShapeStatisticsImageFilter() {}
  void PrintSelf(std::ostream & os, Indent indent) const
    {
      Superclass::PrintSelf(os, indent);
    }

  void EnlargeOutputRequestedRegion(DataObject *data)
    {
      Superclass::EnlargeOutputRequestedRegion(data);
      data->SetRequestedRegionToLargestPossibleRegion();
    }

  void AllocateOutputs()
    {
      // Pass the input through as the output
      IntensityImageType *image =
        const_cast< IntensityImageType * >( this->GetInput() );

      this->GraftOutput(image);

      // Nothing that needs to be allocated for the remaining outputs
    }

  void GenerateData()
    {

      const TInputImage* inputImage( this->GetInput() );
      const TLabelImage* labelImage( this->GetLabelImage() );

      // Create a process accumulator for tracking the progress of minipipeline
      ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
      progress->SetMiniPipelineFilter(this);

      typedef itk::LabelImageToShapeLabelMapFilter< TLabelImage, LabelMapType > ToShapeLabelMapFilter;

      typename ToShapeLabelMapFilter::Pointer filter = ToShapeLabelMapFilter::New();
      filter->SetInput(labelImage);

      filter->SetNumberOfThreads(this->GetNumberOfThreads());
      progress->RegisterInternalFilter(filter, 1.0);

      filter->Update();

      m_LabelMap = filter->GetOutput();

    }

private:
  LabelShapeStatisticsImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);           //purposely not implemented

  typename LabelMapType::ConstPointer m_LabelMap;

}; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
//#include "itkLabelShapeStatisticsImageFilter.hxx"
#endif

#endif
