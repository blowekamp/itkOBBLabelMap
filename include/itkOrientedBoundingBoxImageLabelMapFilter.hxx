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

  typedef itk::ResampleImageFilter<FeatureImageType, AttributeImageType> ResampleFilter;

  typename ResampleFilter::Pointer resampler = ResampleFilter::New();

  typename AttributeImageType::SpacingType outSpacing;
  outSpacing.Fill(feature->GetSpacing()[0]);

  typename AttributeImageType::SizeType outSize;
  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    outSize[i] = Math::Floor<itk::SizeValueType>( labelObject->GetOrientedBoundingBoxSize()[i]/ outSpacing[i] );
    }

  resampler->SetOutputDirection(labelObject->GetOrientedBoundingBoxDirection());
  resampler->SetOutputOrigin(labelObject->GetOrientedBoundingBoxOrigin());
  resampler->SetOutputSpacing(outSpacing);
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
}

} // end namespace itk
#endif
