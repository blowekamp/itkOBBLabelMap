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
#ifndef __itkAttributeImageLabelObject_h
#define __itkAttributeImageLabelObject_h

#include "itkImage.h"
#include "itkLabelMap.h"
#include "itkShapeLabelObject.h"

namespace itk
{

//template< class TLabel, unsigned int VImageDimension > class ShapeLabelObject;

/** \class AttributeImageLabelObject
 *  \brief A LabelObject with an image
 *
 * \ingroup DataRepresentation
 * \ingroup ITKOBBLabelMap
 */
template < class TLabel,
           unsigned int VImageDimension,
           class TAttributeImage = Image<TLabel, VImageDimension>,
           class TSuperclass = ShapeLabelObject<TLabel, VImageDimension> >
class ITK_EXPORT AttributeImageLabelObject : public TSuperclass
{
public:
  /** Standard class typedefs */
  typedef AttributeImageLabelObject              Self;
  typedef TSuperclass                            Superclass;
  typedef SmartPointer<Self>                     Pointer;
  typedef typename Superclass::LabelObjectType   LabelObjectType;
  typedef SmartPointer<const Self>               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(AttributeImageLabelObject, LabelObject);

  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  typedef LabelMap< TSuperclass > LabelMapType;

  typedef TAttributeImage AttributeImageType;

  void SetAttributeImage( AttributeImageType* i  )
  {
    m_AttributeImage = i;
  }
  const AttributeImageType* GetAttributeImage() const
  {
    return m_AttributeImage.GetPointer();
  }
  AttributeImageType* GetAttributeImage()
  {
    return m_AttributeImage.GetPointer();
  }



  virtual void CopyAttributesFrom( const LabelObjectType * lo )
    {
    Superclass::CopyAttributesFrom( lo );

    // copy the data of the current type if possible
    const Self * src = dynamic_cast<const Self *>( lo );
    if( src == NULL || this == src)
      {
      return;
      }
    this->m_AttributeImage = src->m_AttributeImage;
    }

protected:
  AttributeImageLabelObject() { }


  void PrintSelf(std::ostream& os, Indent indent) const
    {
    Superclass::PrintSelf( os, indent );

    os << indent << "AttributeImage: ";

    if ( m_AttributeImage.IsNull() )
      {
      os << "NULL" << std::endl;
      }
    else
      {
      os << m_AttributeImage << std::endl;
      }
    }

private:
  AttributeImageLabelObject(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename AttributeImageType::Pointer m_AttributeImage;

};

} // end namespace itk

#endif
