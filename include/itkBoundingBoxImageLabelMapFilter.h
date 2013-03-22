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
#ifndef __itkBoundingBoxImageLabelMapFilter_h
#define __itkBoundingBoxImageLabelMapFilter_h

#include "itkInPlaceLabelMapFilter.h"
#include "itkShapeLabelMapFilter.h"

namespace itk
{

template< class TImage, class TLabelImage > class ShapeLabelMapFilter;

/** \class BoundingBoxImageLabelMapFilter
 * \ingroup ITKLabelMap
 * \ingroup ITKOBBLabelMap
 */

template< class TImage,
          class TFeatureImage = typename TImage::LabelObjectType::AttributeImageType,
          class TSuperclass = ShapeLabelMapFilter<TImage> >
class ITK_EXPORT BoundingBoxImageLabelMapFilter:
  public TSuperclass
{
public:
  /** Standard class typedefs. */
  typedef BoundingBoxImageLabelMapFilter Self;
  typedef TSuperclass                       Superclass;
  typedef SmartPointer< Self >              Pointer;
  typedef SmartPointer< const Self >        ConstPointer;

  /** Some convenient typedefs. */
  typedef TImage                               ImageType;
  typedef typename ImageType::Pointer          ImagePointer;
  typedef typename ImageType::ConstPointer     ImageConstPointer;
  typedef typename ImageType::PixelType        PixelType;
  typedef typename ImageType::IndexType        IndexType;
  typedef typename ImageType::OffsetType       OffsetType;
  typedef typename ImageType::SizeType         SizeType;
  typedef typename ImageType::LabelObjectType  LabelObjectType;

  typedef TFeatureImage FeatureImageType;
  typedef typename LabelObjectType::AttributeImageType AttributeImageType;

  /** ImageDimension constants */
  itkStaticConstMacro(ImageDimension, unsigned int, TImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(BoundingBoxImageLabelMapFilter, TSuperclass);

  itkSetInputMacro(FeatureImage, FeatureImageType);
  itkGetInputMacro(FeatureImage, FeatureImageType);


  /** Specifies an additional amount to grow or shrink the bounding
   * box by when extracting to the output image.
   */
  itkSetMacro(PaddingOffset, OffsetType);
  itkGetConstMacro(PaddingOffset, OffsetType);
  void SetPaddingOffset( typename OffsetType::OffsetValueType o );

protected:
  BoundingBoxImageLabelMapFilter();

  virtual void ThreadedProcessLabelObject(LabelObjectType *labelObject);

  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  BoundingBoxImageLabelMapFilter(const Self &); //purposely not implemented
  void operator=(const Self &);      //purposely not implemented

  OffsetType m_PaddingOffset;

};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBoundingBoxImageLabelMapFilter.hxx"
#endif

#endif // __itkBoundingBoxImageLabelMapFilter_h
