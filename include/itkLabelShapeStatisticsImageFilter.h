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
#include "itkShapeLabelObject.h"
#include "itkOrientedBoundingBoxLabelObject.h"

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

  /** LabelObject typedefs */
  typedef itk::ShapeLabelObject<LabelPixelType,  ImageDimension>    BaseLabelObjectType;
  typedef itk::OrientedBoundingBoxLabelObject< LabelPixelType,
                                               ImageDimension,
                                               BaseLabelObjectType> DerivedLabelObjectType;

  /** LabelObject attribute typedefs */
  typedef DerivedLabelObjectType                                 LabelObjectType;
  typedef typename LabelObjectType::RegionType                   LabelObjectRegionType;
  typedef double                                                 LabelObjectRealType;
  typedef typename LabelObjectType::CentroidType                 LabelObjectCentroidType;
  typedef typename LabelObjectType::VectorType                   LabelObjectVectorType;
  typedef typename LabelObjectType::MatrixType                   LabelObjectMatrixType;
  typedef typename LabelObjectType::AffineTransformPointer       LabelObjectAffineTransformPointer;
  typedef typename LabelObjectType::OBBVerticesType              LabelObjectOBBVerticesType;
  typedef typename LabelObjectType::OBBPointType                 LabelObjectOBBPointType;
  typedef typename LabelObjectType::OBBDirectionType             LabelObjectOBBDirectionType;
  typedef typename LabelObjectType::OBBSizeType                  LabelObjectOBBSizeType;

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

  itkSetMacro(BackgroundValue, LabelPixelType);
  itkGetConstMacro(BackgroundValue, LabelPixelType);

  itkSetMacro(ComputeFeretDiameter, bool);
  itkGetConstReferenceMacro(ComputeFeretDiameter, bool);
  itkBooleanMacro(ComputeFeretDiameter);

  itkSetMacro(ComputePerimeter, bool);
  itkGetConstReferenceMacro(ComputePerimeter, bool);
  itkBooleanMacro(ComputePerimeter);


#define itkGetLabelObjectAttribute(name, type)                     \
  inline type Get##name( LabelPixelType label ) const            \
    {                                                                   \
      if (this->m_LabelMap.IsNotNull())                                 \
        {                                                               \
        return this->m_LabelMap->GetLabelObject(label)->Get##name();    \
        }                                                               \
      return type ();                                                   \
    }

  // ShapeLabel Object
  itkGetLabelObjectAttribute( BoundingBox, LabelObjectRegionType );
  itkGetLabelObjectAttribute( PhysicalSize, LabelObjectRealType );
  itkGetLabelObjectAttribute( NumberOfPixels, SizeValueType );
  itkGetLabelObjectAttribute( Centroid, LabelObjectCentroidType );
  itkGetLabelObjectAttribute( NumberOfPixelsOnBorder, SizeValueType );
  itkGetLabelObjectAttribute( PerimeterOnBorder, LabelObjectRealType );
  itkGetLabelObjectAttribute( FeretDiameter, LabelObjectRealType );
  itkGetLabelObjectAttribute( PrincipalMoments, LabelObjectVectorType );
  itkGetLabelObjectAttribute( PrincipalAxes, LabelObjectMatrixType );
  itkGetLabelObjectAttribute( Elongation, LabelObjectRealType );
  itkGetLabelObjectAttribute( Perimeter, LabelObjectRealType );
  itkGetLabelObjectAttribute( Roundness, LabelObjectRealType );
  itkGetLabelObjectAttribute( EquivalentSphericalRadius, LabelObjectRealType );
  itkGetLabelObjectAttribute( EquivalentSphericalPerimeter, LabelObjectRealType );
  itkGetLabelObjectAttribute( EquivalentEllipsoidDiameter, LabelObjectVectorType );
  itkGetLabelObjectAttribute( Flatness, LabelObjectRealType );
  itkGetLabelObjectAttribute( PerimeterOnBorderRatio, LabelObjectRealType );
  itkGetLabelObjectAttribute( GetPrincipalAxesToPhysicalAxesTransform, LabelObjectAffineTransformPointer );
  itkGetLabelObjectAttribute( PhysicalAxesToPrincipalAxesTransform, LabelObjectAffineTransformPointer );

  // OrientedBOundingBox Label Object attributes
  itkGetLabelObjectAttribute( OrientedBoundingBoxVertices, LabelObjectOBBVerticesType );
  itkGetLabelObjectAttribute( OrientedBoundingBoxOrigin, LabelObjectOBBPointType );
  itkGetLabelObjectAttribute( OrientedBoundingBoxDirection, LabelObjectOBBDirectionType );
  itkGetLabelObjectAttribute( OrientedBoundingBoxSize, LabelObjectOBBSizeType );

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
  LabelShapeStatisticsImageFilter();

  ~LabelShapeStatisticsImageFilter();

  void PrintSelf(std::ostream & os, Indent indent) const;

  void EnlargeOutputRequestedRegion(DataObject *data);

  void AllocateOutputs();

  void GenerateData();

private:
  LabelShapeStatisticsImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);           //purposely not implemented

  LabelPixelType m_BackgroundValue;
  bool           m_ComputeFeretDiameter;
  bool           m_ComputePerimeter;

  typename LabelMapType::ConstPointer m_LabelMap;

}; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLabelShapeStatisticsImageFilter.hxx"
#endif

#endif
