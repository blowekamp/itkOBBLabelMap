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
#ifndef __itkOrientedBoundingBoxLabelMapFilter_hxx
#define __itkOrientedBoundingBoxLabelMapFilter_hxx

#include "itkOrientedBoundingBoxLabelMapFilter.h"

namespace itk
{


template< class TImage, class TLabelImage >
void
OrientedBoundingBoxLabelMapFilter< TImage, TLabelImage >
::ThreadedProcessLabelObject(LabelObjectType *labelObject)
{
  Superclass::ThreadedProcessLabelObject(labelObject);


  typedef vnl_matrix<double> VNLMatrixType;
  typedef vnl_vector<double> VNLVectorType;

  // this method is slow :(
  const ImageType *            output = this->GetOutput();

  VNLMatrixType rotationMatrix = labelObject->GetPrincipalAxes().GetVnlMatrix();
  const typename LabelObjectType::CentroidType centroid = labelObject->GetCentroid();
  const unsigned int numLines = labelObject->GetNumberOfLines();

  // Create a matrix where the columns are the physical points of the
  // start and end of each RLE line from the label map, relative to
  // the centroid
  VNLMatrixType pixelLocations( ImageDimension, labelObject->GetNumberOfLines()*2 );
  for( unsigned int l = 0; l < numLines; ++l )
    {
    typename LabelObjectType::LineType line = labelObject->GetLine(l);

    // add start index of line as physical point relative to centroid
    IndexType idx = line.GetIndex();
    typename ImageType::PointType pt;
    output->TransformIndexToPhysicalPoint(idx, pt);
    for(unsigned int j = 0; j < ImageDimension; ++j)
      {
      pixelLocations(j,l*2) = pt[j] - centroid[j];
      }

    // add end index of line as physical point relative to centroid
    idx[0] +=  line.GetLength()-1;
    output->TransformIndexToPhysicalPoint(idx, pt);
    for(unsigned int j = 0; j < ImageDimension; ++j)
      {
      pixelLocations(j,l*2+1) = pt[j] - centroid[j];
      }
    }


  // Project the physical points onto principal axes
  VNLMatrixType transformedPixelLocations = rotationMatrix * pixelLocations;

  // find the bounds in the projected domain
  assert( transformedPixelLocations.columns() != 0 );
  VNLVectorType proj_min = transformedPixelLocations.get_column(0);
  VNLVectorType proj_max = transformedPixelLocations.get_column(0);

  for ( unsigned int column = 1; column < transformedPixelLocations.columns(); column++ )
    {
    for ( unsigned int i = 0; i < ImageDimension; ++i )
      {
      const double value = transformedPixelLocations(i, column);
      proj_min[i] = std::min(proj_min[i], value);
      proj_max[i] = std::max(proj_max[i], value);
      }
    }

  // The proj_min/max is from center of pixel to center of pixel. The
  // full extent of the pixels needs to include the offset bits to the
  // corners, projected onto the principle axis basis (rotationMatrix).
  const Vector<double, ImageDimension> gridOffset = 0.5*output->GetSpacing();
  Vector<double, ImageDimension> physicalOffset;
  output->TransformLocalVectorToPhysicalVector(gridOffset, physicalOffset);
  VNLVectorType proj_offset = rotationMatrix * physicalOffset.GetVnlVector();

  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    proj_min[i] -= proj_offset[i];
    proj_max[i] += proj_offset[i];
    }

  Vector<double, ImageDimension> rsize; // real physical size
  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    rsize[i] = vnl_math_abs(proj_max[i]-proj_min[i]);
    }


  //
  // Invert rotation matrix, we will now convert points from the
  // projected space back to the physical one
  //
  rotationMatrix.inplace_transpose();

  VNLVectorType min = rotationMatrix*proj_min;
  VNLVectorType max = rotationMatrix*proj_max;

  for ( unsigned int i = 0; i < ImageDimension; ++i )
    {
    min[i] += centroid[i];
    max[i] += centroid[i];
    }

  typename LabelObjectType::OBBVerticesType vertices;

  // Use binary index to map the vertices of the OBB to an array. For
  // example, in 2D, binary  counting will give[0,0], [0,1], [1,0],
  // [1,1], which corresponds to [minX,minY], [minX,maxY],
  // [maxX,minY], [maxX,maxY].
  for (unsigned int i = 0; i <  LabelObjectType::OBBVerticesType::Length; ++i)
    {
    const unsigned int msb = 1 << (ImageDimension-1);
     for ( unsigned int j = 0; j < ImageDimension; j++ )
      {
      if (i & msb>>j)
        {
        vertices[i][j] = max[j];
        }
      else
        {
        vertices[i][j] = min[j];
        }
      }
    }


  labelObject->SetOrientedBoundingBoxVertices(vertices);
  labelObject->SetOrientedBoundingBoxSize(rsize);
  labelObject->SetOrientedBoundingBoxDirection( rotationMatrix );

}

template< class TImage, class TLabelImage >
void
OrientedBoundingBoxLabelMapFilter< TImage, TLabelImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);


}

} // end namespace itk
#endif
