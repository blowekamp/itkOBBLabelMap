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
#ifndef itkLabelShapeStatisticsImageFilter_hxx
#define itkLabelShapeStatisticsImageFilter_hxx

#include "itkLabelShapeStatisticsImageFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkShapeLabelMapFilter.h"
#include "itkOrientedBoundingBoxLabelMapFilter.h"
#include "itkProgressAccumulator.h"

namespace itk
{

template< typename TInputImage, typename TLabelImage >
LabelShapeStatisticsImageFilter<TInputImage, TLabelImage>
::LabelShapeStatisticsImageFilter()
{
  m_BackgroundValue = NumericTraits< LabelPixelType >::NonpositiveMin();
  m_ComputeFeretDiameter = false;
  m_ComputePerimeter = true;

  this->SetPrimaryInputName( "IntensityImage" );
  this->AddRequiredInputName( "LabelImage", 1 );
}

template< typename TInputImage, typename TLabelImage >
LabelShapeStatisticsImageFilter<TInputImage, TLabelImage>
::~LabelShapeStatisticsImageFilter()
{
}

template< typename TInputImage, typename TLabelImage >
void
LabelShapeStatisticsImageFilter<TInputImage, TLabelImage>
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "BackgroundValue: "
     << static_cast< typename NumericTraits< LabelPixelType >::PrintType >( m_BackgroundValue ) << std::endl;
  os << indent << "ComputeFeretDiameter: " << m_ComputeFeretDiameter << std::endl;
  os << indent << "ComputePerimeter: " << m_ComputePerimeter << std::endl;
}

template< typename TInputImage, typename TLabelImage >
void LabelShapeStatisticsImageFilter<TInputImage, TLabelImage>
::EnlargeOutputRequestedRegion(DataObject *data)
{
  Superclass::EnlargeOutputRequestedRegion(data);
  data->SetRequestedRegionToLargestPossibleRegion();
}

template< typename TInputImage, typename TLabelImage >
void
LabelShapeStatisticsImageFilter<TInputImage, TLabelImage>
::AllocateOutputs()
{
  // Pass the input through as the output
  IntensityImageType *image =
    const_cast< IntensityImageType * >( this->GetInput() );

  this->GraftOutput(image);

  // Nothing that needs to be allocated for the remaining outputs
}

template< typename TInputImage, typename TLabelImage >
void
LabelShapeStatisticsImageFilter<TInputImage, TLabelImage>
::GenerateData()
{

  //const TInputImage* inputImage( this->GetInput() );
  const TLabelImage* labelImage( this->GetLabelImage() );

  // Create a process accumulator for tracking the progress of minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);


  typedef itk::LabelImageToLabelMapFilter<LabelImageType, LabelMapType> ToLabelMapFilterType;
  typename ToLabelMapFilterType::Pointer toLabelMap = ToLabelMapFilterType::New();
  toLabelMap->SetInput( labelImage );
  toLabelMap->SetBackgroundValue( this->m_BackgroundValue );

  toLabelMap->SetNumberOfThreads(this->GetNumberOfThreads());
  progress->RegisterInternalFilter(toLabelMap, .3);


  typedef itk::ShapeLabelMapFilter< LabelMapType > BaseLabelMapFilterType;
  typedef itk::OrientedBoundingBoxLabelMapFilter<LabelMapType, BaseLabelMapFilterType> DerivedLabelMapFilterType;
  typedef DerivedLabelMapFilterType LabelMapFilterType;

  typename LabelMapFilterType::Pointer filter = LabelMapFilterType::New();
  filter->SetInput(toLabelMap->GetOutput());
  filter->SetComputeFeretDiameter( this->m_ComputeFeretDiameter );
  filter->SetComputePerimeter( this->m_ComputePerimeter );

  filter->SetNumberOfThreads(this->GetNumberOfThreads());
  progress->RegisterInternalFilter(filter, .7);

  filter->Update();

  m_LabelMap = filter->GetOutput();

}


} // end namespace itk


#endif
