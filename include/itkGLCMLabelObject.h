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
#ifndef itkGLCMLabelObject_h
#define itkGLCMLabelObject_h

#include "itkImage.h"
#include "itkLabelMap.h"
#include "itkLabelObject.h"

namespace itk
{

/** \class GLCMLabelObject
 *  \brief A LabelObject with Gray Level Cooccurrence Matrix (GLCM) texture features.
 *
 * The features calculated are as follows (where \f$ g(i, j) \f$ is the element in
 * cell i, j of a a normalized GLCM):
 *
 * "Energy" \f$ = f_1 = \sum_{i,j}g(i, j)^2 \f$
 *
 * "Entropy" \f$ = f_2 = -\sum_{i,j}g(i, j) \log_2 g(i, j)\f$, or 0 if \f$g(i, j) = 0\f$
 *
 * "Correlation" \f$ = f_3 = \sum_{i,j}\frac{(i - \mu)(j - \mu)g(i, j)}{\sigma^2} \f$
 *
 * "Difference Moment" \f$= f_4 = \sum_{i,j}\frac{1}{1 + (i - j)^2}g(i, j) \f$
 *
 * "Inertia" \f$ = f_5 = \sum_{i,j}(i - j)^2g(i, j) \f$ (sometimes called "contrast.")
 *
 * "Cluster Shade" \f$ = f_6 = \sum_{i,j}((i - \mu) + (j - \mu))^3 g(i, j) \f$
 *
 * "Cluster Prominence" \f$ = f_7 = \sum_{i,j}((i - \mu) + (j - \mu))^4 g(i, j) \f$
 *
 * "Haralick's Correlation" \f$ = f_8 = \frac{\sum_{i,j}(i, j) g(i, j) -\mu_t^2}{\sigma_t^2} \f$
 * where \f$\mu_t\f$ and \f$\sigma_t\f$ are the mean and standard deviation of the row
 * (or column, due to symmetry) sums.
 *
 * Above, \f$ \mu =  \f$ (weighted pixel average) \f$ = \sum_{i,j}i \cdot g(i, j) =
 * \sum_{i,j}j \cdot g(i, j) \f$ (due to matrix summetry), and
 *
 * \f$ \sigma =  \f$ (weighted pixel variance) \f$ = \sum_{i,j}(i - \mu)^2 \cdot g(i, j) =
 * \sum_{i,j}(j - \mu)^2 \cdot g(i, j)  \f$  (due to matrix summetry)
 *
 * \sa HistogramToTextureFeaturesFilter
 *
 * \ingroup DataRepresentation
 * \ingroup ITKOBBLabelMap
 */
template < class TLabel,
           unsigned int VImageDimension,
           class TSuperclass = LabelObject<TLabel, VImageDimension> >
class GLCMLabelObject : public TSuperclass
{
public:
  /** Standard class typedefs */
  typedef GLCMLabelObject                        Self;
  typedef TSuperclass                            Superclass;
  typedef SmartPointer<Self>                     Pointer;
  typedef typename Superclass::LabelObjectType   LabelObjectType;
  typedef SmartPointer<const Self>               ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(GLCMLabelObject, LabelObject);

  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension);

  typedef LabelMap< Self > LabelMapType;


  /** Return energy texture value. */
  double GetEnergy() const
  {
    return m_Energy;
  }

  void SetEnergy(double v)
  {
    m_Energy = v;
  }

  /** Return entropy texture value. */
  double GetEntropy() const
  {
    return m_Entropy;
  }

  void SetEntropy(double v)
  {
    m_Entropy = v;
  }

  /** return correlation texture value. */
  double GetCorrelation() const
  {
    return m_Correlation;
  }

  void SetCorrelation(double v)
  {
    m_Correlation = v;
  }

  /** Return inverse difference moment texture value. */
  double GetInverseDifferenceMoment() const
  {
    return m_InverseDifferenceMoment;
  }

  void SetInverseDifferenceMoment(double v)
  {
    m_InverseDifferenceMoment = v;
  }

  /** Return inertia texture value. */
  double GetInertia() const
  {
    return m_Inertia;
  }

  void SetInertia(double v)
  {
    m_Inertia = v;
  }

  /** Return cluster shade texture value. */
  double GetClusterShade() const
  {
    return m_ClusterShade;
  }

  void SetClusterShade(double v)
  {
    m_ClusterShade = v;
  }

  /** Return cluster prominence texture value. */
  double GetClusterProminence() const
  {
    return m_ClusterProminence;
  }

  void SetClusterProminence(double v)
  {
    m_ClusterProminence = v;
  }

  /** Return Haralick correlation texture value. */
  double GetHaralickCorrelation() const
  {
    return m_HaralickCorrelation;
  }

  void SetHaralickCorrelation(double v)
  {
    m_HaralickCorrelation = v;
  }

  virtual void CopyAttributesFrom( const LabelObjectType * lo ) ITK_OVERRIDE
    {
    Superclass::CopyAttributesFrom( lo );

    // copy the data of the current type if possible
    const Self * src = dynamic_cast<const Self *>( lo );
    if( src == NULL || this == src)
      {
      return;
      }

    this->m_Energy = src->m_Energy;
    this->m_Entropy = src->m_Entropy;
    this->m_Correlation = src->m_Correlation;
    this->m_InverseDifferenceMoment = src->m_InverseDifferenceMoment;
    this->m_Inertia = src->m_Inertia;
    this->m_ClusterShade = src->m_ClusterShade;
    this->m_ClusterProminence = src->m_ClusterProminence;
    this->m_HaralickCorrelation = src->m_HaralickCorrelation;
    }

protected:

  GLCMLabelObject()
    {
    this->m_Energy = 0.0;
    this->m_Entropy = 0.0;
    this->m_Correlation = 0.0;
    this->m_InverseDifferenceMoment = 0.0;
    this->m_Inertia = 0.0;
    this->m_ClusterShade = 0.0;
    this->m_ClusterProminence = 0.0;
    this->m_HaralickCorrelation = 0.0;
    }


  void PrintSelf(std::ostream& os, Indent indent) const ITK_OVERRIDE
    {
    Superclass::PrintSelf( os, indent );

    os << indent << "Energy: " << m_Energy << std::endl;
    os << indent << "Entropy: " << m_Entropy << std::endl;
    os << indent << "Correlation: " << m_Correlation << std::endl;
    os << indent << "InverseDifferenceMoment: " << m_InverseDifferenceMoment << std::endl;
    os << indent << "Inertia: " << m_Inertia << std::endl;
    os << indent << "ClusterShade: " << m_ClusterShade << std::endl;
    os << indent << "ClusterProminence: " << m_ClusterProminence << std::endl;
    os << indent << "HaralickCorrelation: " << m_HaralickCorrelation << std::endl;
    }

private:
  GLCMLabelObject(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  double m_Energy;
  double m_Entropy;
  double m_Correlation;
  double m_InverseDifferenceMoment;
  double m_Inertia;
  double m_ClusterShade;
  double m_ClusterProminence;
  double m_HaralickCorrelation;

};

} // end namespace itk

#endif
