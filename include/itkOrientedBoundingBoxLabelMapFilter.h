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
#ifndef itkOrientedBoundingBoxLabelMapFilter_h
#define itkOrientedBoundingBoxLabelMapFilter_h

#include "itkInPlaceLabelMapFilter.h"
#include "itkShapeLabelMapFilter.h"

namespace itk
{

/** \class OrientedBoundingBoxLabelMapFilter
 * \ingroup ITKLabelMap
 * \ingroup ITKOBBLabelMap
 */

template< class TImage,
          class TSuperclass = ShapeLabelMapFilter<TImage> >
class OrientedBoundingBoxLabelMapFilter:
  public TSuperclass
{
public:
  /** Standard class typedefs. */
  typedef OrientedBoundingBoxLabelMapFilter Self;
  typedef TSuperclass                       Superclass;
  typedef SmartPointer< Self >              Pointer;
  typedef SmartPointer< const Self >        ConstPointer;

  /** Some convenient typedefs. */
  typedef TImage                               ImageType;
  typedef typename ImageType::Pointer          ImagePointer;
  typedef typename ImageType::ConstPointer     ImageConstPointer;
  typedef typename ImageType::PixelType        PixelType;
  typedef typename ImageType::IndexType        IndexType;
  typedef typename ImageType::SizeType         SizeType;
  typedef typename ImageType::LabelObjectType  LabelObjectType;

  typedef typename LabelObjectType::MatrixType MatrixType;

  /** ImageDimension constants */
  itkStaticConstMacro(ImageDimension, unsigned int, TImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(OrientedBoundingBoxLabelMapFilter, TSuperclass);

protected:
  OrientedBoundingBoxLabelMapFilter() {};

  virtual void ThreadedProcessLabelObject(LabelObjectType *labelObject) ITK_OVERRIDE;

  virtual void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

private:
  OrientedBoundingBoxLabelMapFilter(const Self &); //purposely not implemented
  void operator=(const Self &);      //purposely not implemented

};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkOrientedBoundingBoxLabelMapFilter.hxx"
#endif

#endif // itkOrientedBoundingBoxLabelMapFilter_h
