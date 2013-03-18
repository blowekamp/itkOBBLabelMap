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
#ifndef __itkBoundingBoxImageLabelMapFilter_hxx
#define __itkBoundingBoxImageLabelMapFilter_hxx

#include "itkBoundingBoxImageLabelMapFilter.h"
#include "itkRegionOfInterestImageFilter.h"

namespace itk
{

template< class TImage, class TFeatureImage, class TLabelImage  >
BoundingBoxImageLabelMapFilter< TImage, TFeatureImage, TLabelImage >
::BoundingBoxImageLabelMapFilter()
{
  this->AddRequiredInputName("FeatureImage");
}


template< class TImage, class TFeatureImage, class TLabelImage >
void
BoundingBoxImageLabelMapFilter< TImage, TFeatureImage, TLabelImage >
::SetPaddingOffset( typename OffsetType::OffsetValueType o )
{
  OffsetType offset;
  offset.Fill(o);
  this->SetPaddingOffset(offset);
}


template< class TImage, class TFeatureImage, class TLabelImage >
void
BoundingBoxImageLabelMapFilter< TImage, TFeatureImage, TLabelImage >
::ThreadedProcessLabelObject(LabelObjectType *labelObject)
{
  Superclass::ThreadedProcessLabelObject(labelObject);

  bool bbOK = true;

  typename ImageType::RegionType bb = labelObject->GetBoundingBox();

  for (unsigned int i = 0; i < ImageDimension; ++i)
    {
    SizeType o;
    if (this->m_PaddingOffset[i] > 0 )
      {
      o[i] = this->m_PaddingOffset[i];
      bb.PadByRadius(o);
      }
    else if (this->m_PaddingOffset[i] < 0 &&  bb.GetSize(i) > 2)
      {
      o[i] = std::min<SizeValueType>(-this->m_PaddingOffset[i], bb.GetSize(i)/2 -1);
      bbOK |= bb.ShrinkByRadius(o);
      assert(bbOK);
      }
    }

  // create a shallow copy for the first image, so that the pipeline
  // calls do not go beyond this method.
  typename FeatureImageType::Pointer feature = FeatureImageType::New();
  feature->Graft( this->GetFeatureImage() );

  if( !bb.Crop(feature->GetBufferedRegion()) )
    {
    itkExceptionMacro("Label Object: " << labelObject->GetLabel() << " has Bounding Box: "
                      << labelObject->GetBoundingBox() << " outside of buffered region!");
    }

  typedef RegionOfInterestImageFilter<FeatureImageType, AttributeImageType> ROIFilterType;
  typename ROIFilterType::Pointer roi = ROIFilterType::New();

  roi->SetInput(feature);
  roi->SetRegionOfInterest( bb );
  roi->SetNumberOfThreads(1);
  roi->UpdateLargestPossibleRegion();

  labelObject->SetAttributeImage(roi->GetOutput());

}

template< class TImage, class TFeatureImage, class TLabelImage  >
void
BoundingBoxImageLabelMapFilter< TImage, TFeatureImage, TLabelImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  std::cout << "PaddingOffset: " << m_PaddingOffset;
}

} // end namespace itk
#endif
