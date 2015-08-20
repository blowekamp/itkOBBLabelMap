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
#ifndef itkGLCMLabelMapFilter_hxx
#define itkGLCMLabelMapFilter_hxx

#include "itkGLCMLabelMapFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkHistogramToTextureFeaturesFilter.h"
#include "itkConstNeighborhoodIterator.h"

namespace itk
{
template< typename TImage, typename TFeatureImage, class TSuperclass >
GLCMLabelMapFilter< TImage, TFeatureImage, TSuperclass >
::GLCMLabelMapFilter()
{

  this->SetNumberOfRequiredInputs(2);

  this->m_Min = NumericTraits< PixelType >::max();
  this->m_Max = NumericTraits< PixelType >::NonpositiveMin();

  this->m_NumberOfBinsPerAxis = DefaultBinsPerAxis;
  this->m_Normalize = false;

  m_WindowSize.Fill( 1 );
}


template< typename TImage, typename TFeatureImage, class TSuperclass >
void
GLCMLabelMapFilter< TImage, TFeatureImage, TSuperclass >
::SetOffset(const OffsetType &offset)
{
  OffsetVectorType offsetVector;

  offsetVector.push_back(offset);
  this->SetOffsets(offsetVector);
}


template< typename TImage, typename TFeatureImage, class TSuperclass >
void
GLCMLabelMapFilter< TImage, TFeatureImage, TSuperclass >
::SetPixelValueMinMax(PixelType min, PixelType max)
{
  itkDebugMacro("setting Min to " << min << "and Max to " << max);
  m_Min = min;
  m_Max = max;
  this->Modified();
}


template< typename TImage, typename TFeatureImage, class TSuperclass >
void
GLCMLabelMapFilter< TImage, TFeatureImage, TSuperclass >
::BeforeThreadedGenerateData()
{
  Superclass::BeforeThreadedGenerateData();

  if ( this->m_Min == NumericTraits< PixelType >::max()
       && this->m_Max == NumericTraits< PixelType >::NonpositiveMin() )
    {

    // get the min and max of the feature image, to use those value as the bounds
    // of our
    // histograms
    typedef MinimumMaximumImageCalculator< FeatureImageType > MinMaxCalculatorType;
    typename MinMaxCalculatorType::Pointer minMax = MinMaxCalculatorType::New();
    minMax->SetImage( this->GetFeatureImage() );
    minMax->Compute();

    m_Min = minMax->GetMinimum();
    m_Max = minMax->GetMaximum();
    }


  typename OffsetVectorType::const_iterator iter = m_Offsets.begin();
  while( iter != m_Offsets.end() )
    {
    for ( unsigned int i = 0; i < ImageDimension; ++i )
      {
      m_WindowSize[i] = std::max<typename RadiusType::SizeValueType>(m_WindowSize[i], std::abs((*iter)[i]));
      }
    ++iter;
    }
}

template< typename TImage, typename TFeatureImage, class TSuperclass >
void
GLCMLabelMapFilter< TImage, TFeatureImage, TSuperclass >
::ThreadedProcessLabelObject(LabelObjectType *labelObject)
{
  Superclass::ThreadedProcessLabelObject(labelObject);

  typedef typename LabelObjectType::LengthType  LengthType;

  const FeatureImageType *input = this->GetFeatureImage();


  // constant for a coocurrence matrix.
  const unsigned int measurementVectorSize = 2;


  //
  // Create Histogram
  //
  typename HistogramType::Pointer histogram = HistogramType::New();

  histogram->SetMeasurementVectorSize(measurementVectorSize);


  //initialize parameters
  MeasurementVectorType lowerBound(measurementVectorSize);
  MeasurementVectorType upperBound(measurementVectorSize);

  lowerBound.Fill( this->GetMin() );
  upperBound.Fill( this->GetMax() );


  // First, create an appropriate histogram with the right number of bins
  // and mins and maxs correct for the image type.
  typename HistogramType::SizeType size( histogram->GetMeasurementVectorSize() );
  size.Fill(m_NumberOfBinsPerAxis);
  histogram->Initialize(size, lowerBound, upperBound);

  typedef Statistics::HistogramToTextureFeaturesFilter<HistogramType> FeatureFilterType;
  typename FeatureFilterType::Pointer featureFilter = FeatureFilterType::New();


  const unsigned int numLines = labelObject->GetNumberOfLines();

  // compute bounding box
  RegionType bb = input->GetLargestPossibleRegion();

  bb.ShrinkByRadius( m_WindowSize );


  typedef itk::ConstNeighborhoodIterator< FeatureImageType > NeighborhoodIteratorType;
  typename NeighborhoodIteratorType::RadiusType radius = m_WindowSize;

  NeighborhoodIteratorType nIter( radius, input, bb );
  nIter.NeedToUseBoundaryConditionOff();


  for( unsigned int l = 0; l < numLines; ++l )
    {
    typename LabelObjectType::LineType line = labelObject->GetLine(l);

    IndexType idx = line.GetIndex();
    LengthType length = line.GetLength();

    // trim line for bounding box.
    if ( bb.GetIndex(0) > idx[0] )
      {
      length -= bb.GetIndex(0) - idx[0];
      idx[0] = bb.GetIndex(0);
      }

    if ( !bb.IsInside(idx) )
      {
      continue;
      }

    if ( idx[0] + (IndexValueType)length > bb.GetUpperIndex()[0] )
      {
      length = bb.GetUpperIndex()[0] - idx[0] + 1;
      }

    nIter.SetLocation( idx );

    MeasurementVectorType cooccur( histogram->GetMeasurementVectorSize() );

    for( LengthType i = 0; i < length; ++i )
      {

      const PixelType p1 = nIter.GetCenterPixel();

      // to do use more efficient neighborIndex
      typename OffsetVectorType::const_iterator iter = m_Offsets.begin();
      while( iter != m_Offsets.end() )
        {

        const PixelType p2 = nIter.GetPixel(*iter);

        if (    p1 >= m_Min
                && p2 >= m_Min
                && p1 <= m_Max
                && p2 <= m_Max )
          {
          cooccur[0] = p1;
          cooccur[1] = p2;
          histogram->IncreaseFrequencyOfMeasurement(cooccur, 1.0 );

          cooccur[1] = p1;
          cooccur[0] = p2;
          histogram->IncreaseFrequencyOfMeasurement(cooccur, 1.0 );
          }
        ++iter;
        }
      ++nIter;
      } // end label object line
    }



  featureFilter->SetInput( histogram );
  featureFilter->Modified();
  featureFilter->Update();

  labelObject->SetEnergy( featureFilter->GetEnergy() );
  labelObject->SetEntropy( featureFilter->GetEntropy() );
  labelObject->SetCorrelation( featureFilter->GetCorrelation() );
  labelObject->SetInverseDifferenceMoment( featureFilter->GetInverseDifferenceMoment() );
  labelObject->SetInertia( featureFilter->GetInertia() );
  labelObject->SetClusterShade( featureFilter->GetClusterShade() );
  labelObject->SetClusterProminence( featureFilter->GetClusterProminence() );
  labelObject->SetHaralickCorrelation( featureFilter->GetHaralickCorrelation() );

}

template< typename TImage, typename TFeatureImage, class TSuperclass >
void
GLCMLabelMapFilter< TImage, TFeatureImage, TSuperclass >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);


  os << indent << "Min: " << this->m_Min << std::endl;
  os << indent << "Max: " << this->m_Max << std::endl;

  os << indent << "NumberOfBinsPerAxis: " << this->m_NumberOfBinsPerAxis << std::endl;
  os << indent << "Normalize: " << this->m_Normalize << std::endl;

  os << indent << "WindowSize: " << this->m_WindowSize << std::endl;

}
} // end namespace itk
#endif
