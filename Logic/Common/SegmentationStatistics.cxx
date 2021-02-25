/*=========================================================================

  Program:   ITK-SNAP
  Module:    $RCSfile: SegmentationStatistics.cxx,v $
  Language:  C++
  Date:      $Date: 2010/05/31 19:52:37 $
  Version:   $Revision: 1.1 $
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
#include "SegmentationStatistics.h"
#include "GenericImageData.h"
#include "IRISApplication.h"
#include "ImageCollectionToImageFilter.h"

#include <iostream>
#include <iomanip>


using namespace std;


// TODO: improve efficiency by using filters to integrate label intensities
void
SegmentationStatistics
::Compute(IRISApplication *app)
{
  // Get the current image data
  GenericImageData *id = app->GetCurrentImageData();

  // Get the selected segmentation layer
  LabelImageWrapper *seg = app->GetSelectedSegmentationLayer();

  // A list of image sources
  vector<ScalarImageWrapperBase *> layers;

  // Clear the list of column names
  m_ImageStatisticsColumnNames.clear();

  // Find all the images available for statistics computation
  for(LayerIterator it(id, MAIN_ROLE | OVERLAY_ROLE); !it.IsAtEnd(); ++it)
    {
    ScalarImageWrapperBase *lscalar = it.GetLayerAsScalar();
    if(lscalar)
      {
      m_ImageStatisticsColumnNames.push_back(lscalar->GetNickname());
      layers.push_back(lscalar);
      }
    else
      {
      VectorImageWrapperBase *lvector = it.GetLayerAsVector();
      for(int j = 0; j < lvector->GetNumberOfComponents(); j++)
        {
        std::ostringstream oss;
        oss << lvector->GetNickname();
        if(lvector->GetNumberOfComponents() > 1)
          oss << " [" << j << "]";
        m_ImageStatisticsColumnNames.push_back(oss.str());
        layers.push_back(lvector->GetScalarRepresentation(
              SCALAR_REP_COMPONENT, j));
        }
      }
    }

  // Get the number of gray image layers
  size_t ngray = layers.size();

  // Clear and initialize the statistics table
  m_Stats.clear();

  // Start the label image iteration
  LabelImageWrapper::ConstIterator itLabel = seg->GetImageConstIterator();
  itk::ImageRegion<3> region = itLabel.GetRegion();

  // Cache the entry to avoid many calls to std::map
  LabelType runLabel = 0;
  Entry *cachedEntry = &m_Stats[runLabel];
  cachedEntry->resize(ngray);
  itk::Index<3> runStart = itLabel.GetIndex();
  long runLength = 0;

  // Aggregate the statistical data
  for( ; !itLabel.IsAtEnd(); ++itLabel, ++runLength)
    {
    // Get the label and the corresponding entry (use cache to reduce time wasted in std::map)
    LabelType label = itLabel.Value();
    if(label != runLabel)
      {
      // Record the statistics from the last run
      this->RecordRunLength(ngray, layers, region, runStart, runLength, cachedEntry);

      // Change the cached entry
      runLabel = label;
      cachedEntry = &m_Stats[runLabel];
      if(cachedEntry->count == 0)
        cachedEntry->resize(ngray);

      runStart = itLabel.GetIndex();
      runLength = 0;
      }
    }

  // Record the statistics from the last run
  this->RecordRunLength(ngray, layers, region, runStart, runLength, cachedEntry);

  // Compute the size of a voxel, in mm^3
  const double *spacing = 
    id->GetMain()->GetImageBase()->GetSpacing().GetDataPointer();
  double volVoxel = spacing[0] * spacing[1] * spacing[2];
  double total_count = 0;

  // Obtain Total Count
  for (EntryMap::iterator it = ++(m_Stats.begin()); it != m_Stats.end(); ++it) // sum the total voxel except label 0
	{
		Entry &entry = it->second;
		total_count += entry.count;
	}


  // Compute the mean and standard deviation
  for(EntryMap::iterator it = m_Stats.begin(); it != m_Stats.end(); ++it)
    {
    Entry &entry = it->second;
    for(size_t j = 0; j < ngray; j++)
      {
      // Map to native format
      double mean = entry.sum[j] / entry.count;
      double stdev = sqrt((entry.sumsq[j] - entry.sum[j] * mean) / (entry.count - 1));

      float vol_ratio = (100 * entry.count / total_count);
      double skewness = ((entry.cubicsum[j] - (3 * entry.count * mean * stdev * stdev) - (entry.count * mean * mean * mean)) / (entry.count * stdev * stdev * stdev));
			double kurtosis = (((entry.fourthsum[j] / entry.count) - (4 * mean * entry.cubicsum[j] / entry.count) + (6 * mean * mean * entry.sumsq[j] / entry.count) - (4 * mean * mean * mean * entry.sum[j] / entry.count) + (mean * mean * mean * mean)) / (stdev * stdev * stdev * stdev)) - 3;

      std::cout << "MEDIAN_VEC SIZE = " << entry.median_vec.size() << std::endl;
      sort(entry.median_vec.begin(), entry.median_vec.end(), greater<float>());
      float min = entry.median_vec.back();
			float array_size = (entry.median_vec.size() - 1);
			int two_point_five_num = round(97.5 * array_size / 100);
			int five_num = round(95 * array_size / 100);
			int ten_num = round(9 * array_size / 10);
			int twenty_five_num = round(3 * array_size / 4);
			int median_num = round(array_size / 2);
			int seventy_five_num = round(array_size / 4);
			int ninety_num = round(array_size / 10);
			int ninety_five_num = round(array_size / 20);
			int ninetyseven_point_five_num = round(array_size / 40);
			float two_point_five = entry.median_vec[two_point_five_num];
			float five = entry.median_vec[five_num];
			float ten = entry.median_vec[ten_num];
			float twenty_five = entry.median_vec[twenty_five_num];
			float median = entry.median_vec[median_num];
			float seventy_five = entry.median_vec[seventy_five_num];
			float ninety = entry.median_vec[ninety_num];
			float ninety_five = entry.median_vec[ninety_five_num];
			float ninetyseven_point_five = entry.median_vec[ninetyseven_point_five_num];
			float max = entry.median_vec.front();

      float tissue_frac = 100 * ((static_cast<float>(mean) + 1000) / 1055);
			float air_frac = 100 * ((55 - static_cast<float>(mean)) / 1055);
			double tissue_vmm3 = entry.count * volVoxel *  (tissue_frac / 100);
			double air_vmm3 = entry.count * volVoxel *  (air_frac / 100);
			double haa_ratio_stat = 100 * (entry.HAA_count[j] / entry.count);
			double haa_vmm_stat = entry.HAA_count[j] * volVoxel;
			double below950percentile = 100 * (entry.below950[j] / entry.count);
			double below920percentile = 100 * (entry.below920[j] / entry.count);
			double below910percentile = 100 * (entry.below910[j] / entry.count);
			double below856percentile = 100 * (entry.below856[j] / entry.count);
			double below601percentile = 100 * (entry.below601[j] / entry.count);
			double below250percentile = 100 * (entry.below250[j] / entry.count);
			double above0percentile = 100 * (entry.above0[j] / entry.count);

      entry.volume_ratio[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(vol_ratio);
      entry.mean[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(mean);
      entry.stdev[j] = layers[j]->GetNativeIntensityMapping()->MapGradientMagnitudeToNative(stdev);

      entry.skewness[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(skewness);
			entry.kurtosis[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(kurtosis);
			
      entry.min[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(min);
			entry.two_point_five[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(two_point_five);
			entry.five[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(five);
			entry.ten[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(ten);
			entry.twenty_five[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(twenty_five);
			entry.median[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(median);
			entry.seventy_five[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(seventy_five);
			entry.ninety[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(ninety);
			entry.ninety_five[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(ninety_five);
			entry.ninetyseven_point_five[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(ninetyseven_point_five);
			entry.max[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(max);

      entry.tissue_vmm[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(tissue_vmm3);
			entry.tissue_frac[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(tissue_frac);
			entry.air_vmm[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(air_vmm3);
			entry.air_frac[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(air_frac);
			entry.HAA_ratio[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(haa_ratio_stat);
			entry.HAA_Vmm[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(haa_vmm_stat);
			entry.below950[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(below950percentile);
			entry.below920[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(below920percentile);
			entry.below910[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(below910percentile);
			entry.below856[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(below856percentile);
			entry.below601[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(below601percentile);
			entry.below250[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(below250percentile);
			entry.above0[j] = layers[j]->GetNativeIntensityMapping()->MapInternalToNative(above0percentile);
      }
    entry.volume_mm3 = entry.count * volVoxel;
    }
}

void SegmentationStatistics
::RecordRunLength(size_t ngray, vector<ScalarImageWrapperBase *> &layers,
                  itk::ImageRegion<3> &region, itk::Index<3> &runStart,
                  long runLength, Entry *cachedEntry)
{
  std::vector<float> temp;

  // Record the statistics from the last run
  for(size_t j = 0; j < ngray; j++)
  {
  temp = layers[j]->GetRunLengthIntensityStatistics(
          region, runStart, runLength,
          cachedEntry->sum.data_block() + j,
          cachedEntry->sumsq.data_block() + j,
          cachedEntry->cubicsum.data_block() + j,
          cachedEntry->fourthsum.data_block() + j,
          cachedEntry->HAA_count.data_block() + j,
          cachedEntry->below950.data_block() + j,
          cachedEntry->below920.data_block() + j,
          cachedEntry->below910.data_block() + j,
          cachedEntry->below856.data_block() + j,
          cachedEntry->below601.data_block() + j,
          cachedEntry->below250.data_block() + j,
          cachedEntry->above0.data_block() + j); 
  }
  (cachedEntry->median_vec).insert((cachedEntry->median_vec).end(), temp.begin(), temp.end());
	cachedEntry->count += runLength;
}

void 
SegmentationStatistics
::ExportLegacy(ostream &fout, const ColorLabelTable &clt)
{
  // Write voxel volumes to the file
  fout << "##########################################################" << std::endl;
  fout << "# SNAP Voxel Count File" << std::endl;
  fout << "# File format:" << std::endl;
  fout << "# LABEL: ID / NUMBER / VOLUME / MEAN / SD" << std::endl;
  fout << "# Fields:" << std::endl;
  fout << "#    LABEL         Label description" << std::endl;
  fout << "#    ID            The numerical id of the label" << std::endl;
  fout << "#    NUMBER        Number of voxels that have that label " << std::endl;
  fout << "#    VOLUME        Volume of those voxels in cubic mm " << std::endl;
  fout << "#    MEAN          Mean intensity of those voxels " << std::endl;
  fout << "#    SD            Standard deviation of those voxels " << std::endl;
  fout << "##########################################################" << std::endl;

  for (ColorLabelTable::ValidLabelConstIterator it = clt.begin();
       it != clt.end(); it++)
    {
    LabelType i = it->first;
    ColorLabel cl = clt.GetColorLabel(i);
    if(m_Stats[i].count > 0)
      {
      fout << std::left << std::setw(40) << cl.GetLabel() << ": ";
      fout << std::right << std::setw(4) << i << " / ";
      fout << std::right << std::setw(10) << m_Stats[i].count << " / ";
      fout << std::setw(10) << (m_Stats[i].volume_mm3);
     
      for(size_t j = 0; j < m_Stats[i].mean.size(); j++)
        {
        fout << " / " << std::internal << std::setw(10) 
          << m_Stats[i].mean[j] << " / " << std::setw(10)
          << m_Stats[i].stdev[j];
        }

      fout << endl;
      }      
    }
}

#include "itksys/SystemTools.hxx"

void SegmentationStatistics
::Export(ostream &oss, const string &colsep, const ColorLabelTable &clt)
{
  // Write out the header
  oss << "Label Id" << colsep;
  oss << "Label Name" << colsep;
  oss << "Number Of Voxels" << colsep;
  oss << "Volume (mm^3)" << colsep;
  oss << "Volume (%)";

  // Print the list of column names
  for(int i = 0; i < m_ImageStatisticsColumnNames.size(); i++)
  {
    std::string colname = m_ImageStatisticsColumnNames[i];
    itksys::SystemTools::ReplaceString(colname, colsep.c_str(), " ");

    oss << colsep << "Image mean (" << colname << ")";
    oss << colsep << "Image stdev (" << colname << ")";
  }

	oss << "Skewness" << colsep;
	oss << "Kurtosis" << colsep;
	oss << "Intensity 0%" << colsep;
	oss << "Intensity 2.5%" << colsep;
	oss << "Intensity 5%" << colsep;
	oss << "Intensity 10%" << colsep;
	oss << "Intensity 25%" << colsep;
	oss << "Intensity 50%" << colsep;
	oss << "Intensity 75%" << colsep;
	oss << "Intensity 90%" << colsep;
	oss << "Intensity 95%" << colsep;
	oss << "Intensity 97.5%" << colsep;
	oss << "Intensity 100%" << colsep;
	oss << "Tissue (%)" << colsep;
	oss << "Tissue (mm3)" << colsep;
	oss << "Air (%)" << colsep;
	oss << "Air (mm3)" << colsep;
	oss << "HAA (%)" << colsep;
	oss << "Below -950 (%)" << colsep;
	oss << "Below -920 (%)" << colsep;
	oss << "Below -910 (%)" << colsep;
	oss << "Below -856 (%)" << colsep;
	oss << "Below -601 (%)" << colsep;
	oss << "Below -250 (%)" << colsep;
	oss << "Above 0 (%)";
  // Endline
  oss << std::endl;

  // Write each row
  for(EntryMap::iterator it = m_Stats.begin(); it != m_Stats.end(); ++it)
    {
    LabelType i = it->first;
    Entry &entry = it->second;
    oss << i << colsep;

    std::string label(clt.GetColorLabel(i).GetLabel());
    itksys::SystemTools::ReplaceString(label, colsep.c_str(), " ");
    oss << label << colsep;

    oss << entry.count << colsep;
    oss << entry.volume_mm3;

    for(int j = 0; j < entry.mean.size(); j++)
      {
        oss << colsep << entry.volume_ratio[j];
        oss << colsep << entry.mean[j];
        oss << colsep << entry.stdev[j];
        oss << colsep << entry.skewness[j];
        oss << colsep << entry.kurtosis[j];
        oss << colsep << entry.min[j];
        oss << colsep << entry.two_point_five[j];
        oss << colsep << entry.five[j];
        oss << colsep << entry.ten[j];
        oss << colsep << entry.twenty_five[j];
        oss << colsep << entry.median[j];
        oss << colsep << entry.seventy_five[j];
        oss << colsep << entry.ninety[j];
        oss << colsep << entry.ninety_five[j];
        oss << colsep << entry.ninetyseven_point_five[j];
        oss << colsep << entry.max[j];
        oss << colsep << entry.tissue_frac[j];
        oss << colsep << entry.tissue_vmm[j];
        oss << colsep << entry.air_frac[j];
        oss << colsep << entry.air_vmm[j];
        oss << colsep << entry.HAA_ratio[j];
        oss << colsep << entry.below950[j];
        oss << colsep << entry.below920[j];
        oss << colsep << entry.below910[j];
        oss << colsep << entry.below856[j];
        oss << colsep << entry.below601[j];
        oss << colsep << entry.below250[j];
        oss << colsep << entry.above0[j];
      }

    oss << std::endl;
    }
}

