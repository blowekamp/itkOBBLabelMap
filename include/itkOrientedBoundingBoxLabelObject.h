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
#ifndef __itkOrientedBoundingBoxLabelObject_h
#define __itkOrientedBoundingBoxLabelObject_h

#include "itkLabelMap.h"

namespace itk
{

template< class TLabel, unsigned int VImageDimension > class ShapeLabelObject;

/** \class OrientedBoundingBoxLabelObject
 *  \brief A LabelObject with a generic attribute
 *
 * \ingroup DataRepresentation
 * \ingroup ITKOBBLabelMap
 */
template < class TLabel, unsigned int VImageDimension,
           class TSuperclass = ShapeLabelObject<TLabel, VImageDimension> >
class ITK_EXPORT OrientedBoundingBoxLabelObject : public TSuperclass
{
public:
  /** Standard class typedefs */
  typedef OrientedBoundingBoxLabelObject         Self;
  typedef TSuperclass                            Superclass;
  typedef SmartPointer<Self>                     Pointer;
  typedef typename Superclass::LabelObjectType   LabelObjectType;
  typedef SmartPointer<const Self>               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(OrientedBoundingBoxLabelObject, LabelObject);

  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  typedef LabelMap< Self > LabelMapType;

  typedef typename LabelMapType::DirectionType OBBDirectionType;
  typedef typename LabelMapType::PointType     OBBPointType;
  typedef Vector<double,ImageDimension >       OBBSizeType;

  void SetOrientedBoundingBoxOrigin( const OBBPointType &o )
  {
    m_OBBOrigin = o;
  }
  const OBBPointType & GetOrientedBoundingBoxOrigin() const
  {
      return m_OBBOrigin;
  }
  OBBPointType GetOrientedBoundingBoxOrigin()
  {
    return m_OBBOrigin;
  }

  void SetOrientedBoundingBoxDirection( const OBBDirectionType &d )
  {
    m_OBBDirection = d;
  }
  const OBBDirectionType & GetOrientedBoundingBoxDirection() const
  {
      return m_OBBDirection;
  }
  OBBDirectionType GetOrientedBoundingBoxDirection()
  {
    return m_OBBDirection;
  }


  void SetOrientedBoundingBoxSize( const OBBSizeType &d )
  {
    m_OBBSize = d;
  }
  const OBBSizeType & GetOrientedBoundingBoxSize() const
  {
      return m_OBBSize;
  }
  OBBSizeType GetOrientedBoundingBoxSize()
  {
    return m_OBBSize;
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
    this->m_OBBOrigin = src->m_OBBOrigin;
    this->m_OBBDirection = src->m_OBBDirection;
    this->m_OBBSize = src->m_OBBSize;
    }

protected:
  OrientedBoundingBoxLabelObject()
    {
    // how to initialize the attribute ?
    }


  void PrintSelf(std::ostream& os, Indent indent) const
    {
    Superclass::PrintSelf( os, indent );

    os << indent << "OBBOrigin: " << m_OBBOrigin << std::endl;
    os << indent << "OBBDirection: " << m_OBBDirection << std::endl;
    os << indent << "OBBSize: " << m_OBBSize << std::endl;
    }

private:
  OrientedBoundingBoxLabelObject(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  OBBPointType m_OBBOrigin;

  OBBDirectionType m_OBBDirection;

  OBBSizeType m_OBBSize;


};

} // end namespace itk

#endif
