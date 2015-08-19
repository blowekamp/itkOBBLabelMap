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
#ifndef itkGLCMLabelMapFilter_h
#define itkGLCMLabelMapFilter_h


#include "itkHistogram.h"
#include "itkDenseFrequencyContainer2.h"

#include "itkInPlaceLabelMapFilter.h"

namespace itk
{

/** \class GLCMLabelMapFilter
 * \brief Compute Gray Level Co-occurrence Matrix (GLCM) texture features.
 *
 * The GLCMLabelMapFilter computes the attributes in the
 * GLCMLabelObject stored in a LabelMap.
 *
 * For each LabelObject, a co-occurrence matrix is computed from the
 * intensities in the feature image. Then the texture features are
 * computed with Statistics::HistogramToTextureFeaturesFilter.
 *
 * The offsets of the co-occurence must be provided to the filter. If
 * any of the offsets for a given pixel are outside the image
 * boundaries, then all are ignored in the pixel's neighborhood.
 *
 * \sa GLCMLabelObject
 * \ingroup ITKLabelMap
 * \ingroup ITKOBBLabelMap
 */

template< class TImage,
          typename TFeatureImage,
          class TSuperclass = InPlaceLabelMapFilter<TImage> >
class GLCMLabelMapFilter:
  public TSuperclass
{
public:
  /** Standard class typedefs. */
  typedef GLCMLabelMapFilter                Self;
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
  typedef typename ImageType::RegionType       RegionType;
  typedef typename ImageType::LabelObjectType  LabelObjectType;

  typedef TFeatureImage                           FeatureImageType;
  typedef typename FeatureImageType::Pointer      FeatureImagePointer;
  typedef typename FeatureImageType::ConstPointer FeatureImageConstPointer;
  typedef typename FeatureImageType::PixelType    FeatureImagePixelType;

  typedef typename FeatureImageType::OffsetType   OffsetType;
  typedef typename FeatureImageType::SizeType     RadiusType;
  typedef std::vector< OffsetType >               OffsetVectorType;

  typedef typename NumericTraits< PixelType >::RealType                             MeasurementType;
  typedef Statistics::DenseFrequencyContainer2                                      HistogramFrequencyContainerType;
  typedef Statistics::Histogram< MeasurementType, HistogramFrequencyContainerType > HistogramType;
//   typedef typename HistogramType::Pointer                            HistogramPointer;
//   typedef typename HistogramType::ConstPointer                       HistogramConstPointer;
  typedef typename HistogramType::MeasurementVectorType              MeasurementVectorType;



  /** ImageDimension constants */
  itkStaticConstMacro(ImageDimension, unsigned int, TImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(GLCMLabelMapFilter, TSuperclass);

  /** Set the feature image used to compute the GLCM. */
  void SetFeatureImage(const TFeatureImage *input)
  {
    // Process object is not const-correct so the const casting is required.
    this->SetNthInput( 1, const_cast< TFeatureImage * >( input ) );
  }

  /** Get the feature image */
  FeatureImageType * GetFeatureImage()
  {
    return static_cast< FeatureImageType * >( const_cast< DataObject * >( this->ProcessObject::GetInput(1) ) );
  }

  itkStaticConstMacro(DefaultBinsPerAxis, unsigned int, 64);

  /** Set the offset or offsets over which the co-occurrence pairs will be computed.
    *
    * Calling either of these methods clears the previous offsets.
    */
  itkGetConstReferenceMacro(Offsets, OffsetVectorType);
  void SetOffsets( const OffsetVectorType &offsets )
  {
    if ( this->m_Offsets != offsets )
      {
      this->m_Offsets.assign( offsets.begin(), offsets.end() );
      this->Modified();
      }
  }
  void SetOffset(const OffsetType &offset);

  /** Set number of histogram bins along each axis of image intensity */
  itkSetMacro(NumberOfBinsPerAxis, unsigned int);
  itkGetConstMacro(NumberOfBinsPerAxis, unsigned int);

  /** Set the min and max (inclusive) pixel value that will be placed in the
   *  histogram.
   *
   * If not set the values will automatically be computed from the
   * feature image.
  */
  void SetPixelValueMinMax(PixelType min, PixelType max);

  itkGetConstMacro(Min, PixelType);
  itkGetConstMacro(Max, PixelType);

  /** Set the calculator to normalize the histogram (divide all bins by the
    total frequency). Normalization is off by default. */
  itkSetMacro(Normalize, bool);
  itkGetConstMacro(Normalize, bool);
  itkBooleanMacro(Normalize);


protected:
  GLCMLabelMapFilter();

  virtual void ThreadedProcessLabelObject(LabelObjectType *labelObject) ITK_OVERRIDE;

  virtual void BeforeThreadedGenerateData() ITK_OVERRIDE;

  virtual void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;

private:
  GLCMLabelMapFilter(const Self &); //purposely not implemented
  void operator=(const Self &);      //purposely not implemented

  OffsetVectorType m_Offsets;
  std::vector<std::pair< OffsetType, OffsetType > > m_CooccurenceOffsetVector;

  RadiusType m_WindowSize;

  PixelType        m_Min;
  PixelType        m_Max;

  unsigned int          m_NumberOfBinsPerAxis;
  bool                  m_Normalize;

};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGLCMLabelMapFilter.hxx"
#endif

#endif // itkGLCMLabelMapFilter_h
