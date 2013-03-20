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
#ifndef __itkOrientedBoundingBoxImageLabelMapFilter_hxx
#define __itkOrientedBoundingBoxImageLabelMapFilter_hxx

#include "itkOrientedBoundingBoxImageLabelMapFilter.h"
#include "itkResampleImageFilter.h"

namespace itk
{


template< class TImage, class TFeatureImage, class TLabelImage  >
OrientedBoundingBoxImageLabelMapFilter< TImage, TFeatureImage, TLabelImage >
::OrientedBoundingBoxImageLabelMapFilter()
{
  this->AddRequiredInputName("FeatureImage");

  m_PaddingOffset.Fill(-0.5);
  m_AttributeImageSpacing.Fill(1.0);

  typedef LinearInterpolateImageFunction< AttributeImageType, double >   LinearInterpolatorType;
  m_Interpolator = LinearInterpolatorType::New().GetPointer();
  m_DefaultPixelValue = NumericTraits<AttributeImagePixelType>::ZeroValue( m_DefaultPixelValue );
}


template< class TImage, class TFeatureImage, class TLabelImage >
void
OrientedBoundingBoxImageLabelMapFilter< TImage, TFeatureImage, TLabelImage >
::SetPaddingOffset( typename SpacingType::ValueType o )
{
  SpacingType offset;
  offset.Fill(o);
  this->SetPaddingOffset(offset);
}

template< class TImage, class TFeatureImage, class TLabelImage >
void
OrientedBoundingBoxImageLabelMapFilter< TImage, TFeatureImage, TLabelImage >
::ThreadedProcessLabelObject(LabelObjectType *labelObject)
{
  Superclass::ThreadedProcessLabelObject(labelObject);

  // create a shallow copy for the first image, so that the pipeline
  // calls do not go beyond this method.
  typename FeatureImageType::Pointer feature = FeatureImageType::New();
  feature->Graft( this->GetFeatureImage() );

  // transform padding offset from offset in output basis to physical space
  Vector<double,ImageDimension> offset = labelObject->GetOrientedBoundingBoxDirection()*m_PaddingOffset;

  typedef itk::ResampleImageFilter<FeatureImageType, AttributeImageType> ResampleFilter;

  typename ResampleFilter::Pointer resampler = ResampleFilter::New();

  typename AttributeImageType::SizeType outSize;
  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    if ( m_PaddingOffset[i] < 0 && labelObject->GetOrientedBoundingBoxSize()[i]  <= -2.0*m_PaddingOffset[i] )
      {
      outSize[i] = 1;
      }
    else
      {
      outSize[i] = Math::Round<itk::SizeValueType>( (labelObject->GetOrientedBoundingBoxSize()[i]+2.0*m_PaddingOffset[i])/m_AttributeImageSpacing[i] )+1;
      }
    }

  resampler->SetOutputDirection(labelObject->GetOrientedBoundingBoxDirection());
  resampler->SetOutputOrigin(labelObject->GetOrientedBoundingBoxOrigin()-offset);
  resampler->SetOutputSpacing(this->m_AttributeImageSpacing);
  resampler->SetSize(outSize);
  resampler->SetInput(feature);

  resampler->SetNumberOfThreads(1);

  resampler->UpdateLargestPossibleRegion();

  labelObject->SetAttributeImage(resampler->GetOutput());

}

template< class TImage, class TFeatureImage, class TLabelImage  >
void
OrientedBoundingBoxImageLabelMapFilter< TImage, TFeatureImage, TLabelImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PaddingOffset: " << m_PaddingOffset << std::endl;
  os << indent << "AttributeimageSpacing: " << m_AttributeImageSpacing << std::endl;

  os << indent << "Interpolator: " << m_Interpolator << std::endl;
  os << indent << "DefaultPixelValue: " << m_DefaultPixelValue << std::endl;
}

} // end namespace itk
#endif
