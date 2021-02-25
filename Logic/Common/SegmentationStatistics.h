/*=========================================================================

  Program:   ITK-SNAP
  Module:    $RCSfile: SegmentationStatistics.h,v $
  Language:  C++
  Date:      $Date: 2010/06/15 16:54:35 $
  Version:   $Revision: 1.2 $
  Copyright (c) 2007 Paul A. Yushkevich
  
  This file is part of ITK-SNAP 

  ITK-SNAP is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  -----

  Copyright (c) 2003 Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information. 

=========================================================================*/
#ifndef __SegmentationStatistics_h_
#define __SegmentationStatistics_h_

#include "SNAPCommon.h"
#include <vector>
#include <string>
#include <iostream>
#include <map>

class GenericImageData;
class ColorLabelTable;
class ScalarImageWrapperBase;
class IRISApplication;

namespace itk {
  template <unsigned int VDim> class ImageRegion;
  template <unsigned int VDim> class Index;
}

class SegmentationStatistics
{
public:

  /* Data structure cooresponding to a gray overlay image */
  struct GrayStats { 
    std::string layer_id;
    double sum, sumsq, mean, sd, cubicsum, fourthsum, skewness, kurtosis, median,
           max, min, two_point_five, five, ten, twenty_five, seventy_five, ninety, ninety_five, ninetyseven_point_five,
           volume_ratio, tissue_frac, tissue_vmm, air_frac, air_vmm, HAA_ratio, HAA_Vmm, HAA_count, 
           below950, below920, below910, below856, below601, below250, above0;
    GrayStats() : sum(0), sumsq(0), mean(0), sd(), cubicsum(0), fourthsum(0), 
                  skewness(0), kurtosis(0), median(0), max(0), min(0), tissue_frac(0), 
                  tissue_vmm(0), air_frac(0), air_vmm(0), HAA_ratio(0), HAA_Vmm(0), HAA_count(0), 
                  below950(0), below920(0), below910(0), below856(0), below601(0), below250(0), 
                  above0(0) {}
  };

  /* Data structure corresponding to a row in the statistics table */
  struct Entry {
    unsigned long int count;
    double volume_mm3;
    std::vector<float> median_vec;
    vnl_vector<double> sum, sumsq, mean, stdev, cubicsum, fourthsum, skewness, kurtosis, median, max, min, two_point_five, five, ten,
                       twenty_five, seventy_five, ninety, ninety_five, ninetyseven_point_five, volume_ratio, tissue_frac, tissue_vmm,
                       air_frac, air_vmm, HAA_ratio, HAA_Vmm, HAA_count, below950, below920, below910, below856, below601, below250, above0;
    Entry() : count(0),volume_mm3(0), median_vec() {}
    void resize(int n) {
      sum.set_size(n); sum.fill(0);
      sumsq.set_size(n); sumsq.fill(0);
      mean.set_size(n); mean.fill(0);
      stdev.set_size(n); stdev.fill(0);

      volume_ratio.set_size(n); volume_ratio.fill(0);

      cubicsum.set_size(n); cubicsum.fill(0);
      fourthsum.set_size(n); fourthsum.fill(0);
      skewness.set_size(n); skewness.fill(0);
      kurtosis.set_size(n); kurtosis.fill(0);

      min.set_size(n); min.fill(0);
      two_point_five.set_size(n); two_point_five.fill(0);
      five.set_size(n); five.fill(0);
      ten.set_size(n); ten.fill(0);
      twenty_five.set_size(n); twenty_five.fill(0);
      median.set_size(n); median.fill(0);
      seventy_five.set_size(n); seventy_five.fill(0);
      ninety.set_size(n); ninety.fill(0);
      ninety_five.set_size(n); ninety_five.fill(n);
      ninetyseven_point_five.set_size(n); ninetyseven_point_five.fill(n);
      max.set_size(n); max.fill(0);

      tissue_frac.set_size(n); tissue_frac.fill(0);
      tissue_vmm.set_size(n); tissue_vmm.fill(0);
      air_frac.set_size(n); air_frac.fill(0);
      air_vmm.set_size(n); air_vmm.fill(0);
      HAA_ratio.set_size(n); HAA_ratio.fill(0);
      HAA_Vmm.set_size(n); HAA_Vmm.fill(0);
      HAA_count.set_size(n); HAA_count.fill(0);

      below950.set_size(n); below950.fill(0);
      below920.set_size(n); below920.fill(0);
      below910.set_size(n); below910.fill(0);
      below856.set_size(n); below856.fill(0);
      below601.set_size(n); below601.fill(0);
      below250.set_size(n); below250.fill(0);
      above0.set_size(n);   above0.fill(0);
    }
  };

  typedef std::map<LabelType, Entry> EntryMap;

  /* Compute statistics from a segmentation image */
  void Compute(IRISApplication *app);
  
  /* Export to a text file using legacy format */
  void ExportLegacy(std::ostream &oss, const ColorLabelTable &clt);

  /* Export to a CSV or text file */
  void Export(std::ostream &oss, const std::string &colsep, const ColorLabelTable &clt);

  const EntryMap &GetStats() const
    { return m_Stats; }

  const std::vector<std::string> &GetImageStatisticsColumns() const
    { return m_ImageStatisticsColumnNames; }

private:

  // Label statistics
  EntryMap m_Stats;

  // Column information
  std::vector<std::string> m_ImageStatisticsColumnNames;
  
  void RecordRunLength(
      size_t ngray,
      std::vector<ScalarImageWrapperBase *> &layers,
      itk::ImageRegion<3> &region,
      itk::Index<3> &runStart,
      long runLength,
      Entry *cachedEntry);
};

#endif
