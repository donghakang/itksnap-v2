/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ScalarImageWrapper.txx,v $
  Language:  C++
  Date:      $Date: 2009/01/24 01:50:21 $
  Version:   $Revision: 1.4 $
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "ScalarImageWrapper.h"
#include "ImageWrapperTraits.h"
#include "RLEImageRegionIterator.h"
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkNumericTraits.h"
#include "RLERegionOfInterestImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkVTKImageExport.h"
#include "itkStreamingImageFilter.h"

#include "AdaptiveSlicingPipeline.h"
#include "SNAPSegmentationROISettings.h"
#include "itkCommand.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkVectorImageToImageAdaptor.h"
#include "itkCastImageFilter.h"
#include "IRISException.h"
#include "VectorImageWrapper.h"
#include "ScalarImageHistogram.h"
#include "ThreadedHistogramImageFilter.h"
#include "GuidedNativeImageIO.h"
#include "itkImageFileWriter.h"

#include "vtkImageImport.h"

#include <iostream>

template<class TTraits, class TBase>
ScalarImageWrapper<TTraits,TBase>
::ScalarImageWrapper()
{
  m_MinMaxFilter = MinMaxFilter::New();
  m_HistogramFilter = HistogramFilterType::New();

  // Set up VTK export pipeline
  //this->SetupVTKImportExport();
}

template<class TTraits, class TBase>
ScalarImageWrapper<TTraits,TBase>
::ScalarImageWrapper(const Self &copy)
{
  Superclass::CommonInitialization();

  // If the source contains an image, make a copy of that image
  if (copy.IsInitialized() && copy.GetImage())
    {
    // Create and allocate the image
    ImagePointer newImage = ImageType::New();
    newImage->SetRegions(copy.GetImage()->GetBufferedRegion());
    newImage->Allocate();

    // Copy the image contents
    typedef typename ImageType::InternalPixelType InternalPixelType;
    InternalPixelType *ptrTarget = newImage->GetBufferPointer();
    InternalPixelType *ptrSource = copy.GetImage()->GetBufferPointer();
    memcpy(ptrTarget,ptrSource,
           sizeof(InternalPixelType) * newImage->GetBufferedRegion().GetNumberOfPixels());
    
    UpdateImagePointer(newImage);
    }
}

template<>
ScalarImageWrapper<LabelImageWrapperTraits, ScalarImageWrapperBase>
::ScalarImageWrapper(const Self &copy)
{
    CommonInitialization();

    // If the source contains an image, make a copy of that image
    if (copy.IsInitialized() && copy.GetImage())
    {
        typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> roiType;
        roiType::Pointer roi = roiType::New();
        roi->SetInput(copy.GetImage());
        roi->Update();
        ImagePointer newImage = roi->GetOutput();
        UpdateImagePointer(newImage);
    }
}

template<class TTraits, class TBase>
ScalarImageWrapper<TTraits,TBase>
::~ScalarImageWrapper()
{

}


template<class TTraits, class TBase>
void 
ScalarImageWrapper<TTraits,TBase>
::UpdateImagePointer(ImageType *newImage, ImageBaseType *referenceSpace, ITKTransformType *transform)
{
  // std::cout << "(ScalarImageWrapper.cxx) - UpdateImagePointer starts" << std::endl;
  // Call the parent
  Superclass::UpdateImagePointer(newImage, referenceSpace, transform);

  // Update the max-min pipeline once we have one setup
  m_MinMaxFilter->SetInput(newImage);

  m_MinMaxFilter->GetMaximumOutput()->Update();
  // std::cout << "ScalarImageWrapper.cxx -- MAX: " << static_cast<double>(m_MinMaxFilter->GetMaximumOutput()->Get());
  // std::cout << "    MIN: " <<  static_cast<double>(m_MinMaxFilter->GetMinimumOutput()->Get()) << std::endl;

  // Update the histogram mini-pipeline
  m_HistogramFilter->SetInput(newImage);
  m_HistogramFilter->SetRangeInputs(m_MinMaxFilter->GetMinimumOutput(),
                                    m_MinMaxFilter->GetMaximumOutput());


  // Set the number of bins to default
  m_HistogramFilter->SetNumberOfBins(DEFAULT_HISTOGRAM_BINS);

  // Update the common representation policy
  m_CommonRepresentationPolicy.UpdateInputImage(newImage);

  // Update the VTK export pipeline
  // m_VTKExporter->SetInput(newImage);
}

template <class TTraits, class TBase>
void
ScalarImageWrapper<TTraits,TBase>
::SetNativeMapping(NativeIntensityMapping mapping)
{
  Superclass::SetNativeMapping(mapping);
  // Propagate the mapping to the histogram
  m_HistogramFilter->SetIntensityTransform(mapping.GetScale(), mapping.GetShift());
}


template<class TTraits, class TBase>
void 
ScalarImageWrapper<TTraits,TBase>
::CheckImageIntensityRange() 
{
  // Image should be loaded
  assert(this->m_Image);

  // Check if the image has been updated since the last time that
  // the min/max has been computed
  m_MinMaxFilter->Update();
  m_ImageScaleFactor = 1.0 / (m_MinMaxFilter->GetMaximum() - m_MinMaxFilter->GetMinimum());
  // std::cout << "Current m_ImageScaleFactor: " << m_ImageScaleFactor << "          - ScalarImageWrapper.cxx " << std::endl;
}

template<class TTraits, class TBase>
typename ScalarImageWrapper<TTraits,TBase>::ComponentTypeObject *
ScalarImageWrapper<TTraits,TBase>
::GetImageMinObject() const
{
  return m_MinMaxFilter->GetMinimumOutput();
}

template<class TTraits, class TBase>
typename ScalarImageWrapper<TTraits,TBase>::ComponentTypeObject *
ScalarImageWrapper<TTraits,TBase>
::GetImageMaxObject() const
{
  return m_MinMaxFilter->GetMaximumOutput();
}

template<class TTraits, class TBase>
double
ScalarImageWrapper<TTraits,TBase>
::GetImageGradientMagnitudeUpperLimit()
{
  // Paul - 12/1/2014: I have modified this code to not compute the gradient magnitude
  // on the whole image, but instead, to compute just the difference between the max and
  // min values. I think this is much more efficient, and this removes some problems with
  // wrappers that wrap around ImageAdapter objects.
  //
  // I hope this does not cause too much trouble...
  return m_MinMaxFilter->GetMaximum() - m_MinMaxFilter->GetMinimum();
}

template<class TTraits, class TBase>
double
ScalarImageWrapper<TTraits,TBase>
::GetImageGradientMagnitudeUpperLimitNative()
{
  return this->m_NativeMapping.MapGradientMagnitudeToNative(
        this->GetImageGradientMagnitudeUpperLimit());
}


template<class TTraits, class TBase>
SmartPtr<typename ScalarImageWrapper<TTraits, TBase>::FloatImageSource>
ScalarImageWrapper<TTraits, TBase>::CreateCastToFloatPipeline() const
{
  // The kind of mini-pipeline that is created here depends on whether the
  // internal image is a floating point image or not, and whether the native
  // to intensity mapping is identity or not. We use template specialization to
  // select the right behavior
  typedef itk::UnaryFunctorImageFilter<ImageType, FloatImageType, NativeIntensityMapping> FilterType;
  SmartPtr<FilterType> filter = FilterType::New();
  filter->SetInput(this->m_Image);
  filter->SetFunctor(this->m_NativeMapping);

  SmartPtr<FloatImageSource> output = filter.GetPointer();
  return output;
}

template<class TTraits, class TBase>
SmartPtr<typename ScalarImageWrapper<TTraits, TBase>::DoubleImageSource>
ScalarImageWrapper<TTraits, TBase>::CreateCastToDoublePipeline() const
{
  // The kind of mini-pipeline that is created here depends on whether the
  // internal image is a floating point image or not, and whether the native
  // to intensity mapping is identity or not. We use template specialization to
  // select the right behavior
  typedef itk::UnaryFunctorImageFilter<ImageType, DoubleImageType, NativeIntensityMapping> FilterType;
  SmartPtr<FilterType> filter = FilterType::New();
  filter->SetInput(this->m_Image);
  filter->SetFunctor(this->m_NativeMapping);

  SmartPtr<DoubleImageSource> output = filter.GetPointer();
  return output;
}

template <class TInputImage, class TOutputImage, class TFunctor>
class UnaryFunctorImageToSingleComponentVectorImageFilter
    : public itk::ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  typedef UnaryFunctorImageToSingleComponentVectorImageFilter<TInputImage, TOutputImage, TFunctor> Self;
  typedef itk::ImageToImageFilter<TInputImage, TOutputImage> Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer< const Self >  ConstPointer;

  typedef TInputImage InputImageType;
  typedef TOutputImage OutputImageType;
  typedef TFunctor FunctorType;

  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(UnaryFunctorImageToSingleComponentVectorImageFilter, ImageToImageFilter)
  itkNewMacro(Self)

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  void SetFunctor(const FunctorType &functor)
  {
    if(m_Functor != functor)
      {
      m_Functor = functor;
      this->Modified();
      }
  }

  itkGetConstReferenceMacro(Functor, FunctorType)

  void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread,
                            itk::ThreadIdType threadId) ITK_OVERRIDE;


protected:

  UnaryFunctorImageToSingleComponentVectorImageFilter() {}
  virtual ~UnaryFunctorImageToSingleComponentVectorImageFilter() {}

  FunctorType m_Functor;

};

#include "ImageRegionConstIteratorWithIndexOverride.h"

template <class TInputImage, class TOutputImage, class TFunctor>
void
UnaryFunctorImageToSingleComponentVectorImageFilter<TInputImage, TOutputImage, TFunctor>
::ThreadedGenerateData(const OutputImageRegionType &outputRegionForThread, itk::ThreadIdType threadId)
{
  // Use our fast iterators for vector images
  typedef itk::ImageLinearIteratorWithIndex<OutputImageType> IterBase;
  typedef IteratorExtender<IterBase> IterType;

  typedef typename OutputImageType::InternalPixelType OutputComponentType;
  typedef typename InputImageType::InternalPixelType InputComponentType;

  // Define the iterators
  IterType outputIt(this->GetOutput(), outputRegionForThread);
  int line_len = outputRegionForThread.GetSize(0);

  // Using a generic ITK iterator for the input because it supports RLE images and adaptors
  itk::ImageScanlineConstIterator< InputImageType > inputIt(this->GetInput(), outputRegionForThread);

  while ( !inputIt.IsAtEnd() )
    {
    // Get the pointer to the input and output pixel lines
    OutputComponentType *out = outputIt.GetPixelPointer(this->GetOutput());

    for(int i = 0; i < line_len; i++, ++inputIt)
      {
      out[i] = m_Functor(inputIt.Get());
      }

    outputIt.NextLine();
    inputIt.NextLine();
    }
}

template<class TTraits, class TBase>
SmartPtr<typename ScalarImageWrapper<TTraits, TBase>::FloatVectorImageSource>
ScalarImageWrapper<TTraits, TBase>::CreateCastToFloatVectorPipeline() const
{
  typedef UnaryFunctorImageToSingleComponentVectorImageFilter<
      ImageType, FloatVectorImageType, NativeIntensityMapping> FilterType;
  SmartPtr<FilterType> filter = FilterType::New();
  filter->SetInput(this->m_Image);
  filter->SetFunctor(this->m_NativeMapping);

  SmartPtr<FloatVectorImageSource> output = filter.GetPointer();
  return output;
}

template<class TTraits, class TBase>
SmartPtr<typename ScalarImageWrapper<TTraits, TBase>::DoubleVectorImageSource>
ScalarImageWrapper<TTraits, TBase>::CreateCastToDoubleVectorPipeline() const
{
  typedef UnaryFunctorImageToSingleComponentVectorImageFilter<
      ImageType, DoubleVectorImageType, NativeIntensityMapping> FilterType;
  SmartPtr<FilterType> filter = FilterType::New();
  filter->SetInput(this->m_Image);
  filter->SetFunctor(this->m_NativeMapping);

  SmartPtr<DoubleVectorImageSource> output = filter.GetPointer();
  return output;
}

template<class TTraits, class TBase>
double 
ScalarImageWrapper<TTraits,TBase>
::GetImageScaleFactor()
{
  // Make sure min/max are up-to-date
  CheckImageIntensityRange();

  // Return the max or min
  return m_ImageScaleFactor;
}

/** Compute statistics over a run of voxels in the image starting at the index
 * startIdx. Appends the statistics to a running sum and sum of squared. The
 * statistics are returned in internal (not native mapped) format */
template<class TTraits, class TBase>
std::vector<float>
ScalarImageWrapper<TTraits, TBase>
::GetRunLengthIntensityStatistics(
    const itk::ImageRegion<3> &region,
    const itk::Index<3> &startIdx, long runlength,
    double *out_sum, double *out_sumsq, double *cubicsum, double *fourthsum, double *out_haa, double *below950, double *below920, double* below910,
    double *below856,double *below601, double *below250, double *above0) const
{
  if(this->IsSlicingOrthogonal())
    {
    ConstIterator it(this->m_Image, region);
    it.SetIndex(startIdx);

    std::vector<float> temp_vec;
    temp_vec.reserve(runlength);    // Reserve the space for time saving
    // Perform the integration
    for(long q = 0; q < runlength; q++, ++it)
      {
      double p = (double) it.Get();

      if (p >= -700 && p <= 0) ++(*out_haa);
      if (p <= -950) ++(*below950);
      if (p <= -920) ++(*below920);
      if (p <= -910) ++(*below910);
      if (p <= -856) ++(*below856);
      if (p <= -601) ++(*below601);
      if (p <= -250) ++(*below250);
      if (p >= 0)    ++(*above0);


      *out_sum += p;
      *out_sumsq += p * p;
      *cubicsum += p * p * p;
      *fourthsum += p * p * p * p;
      temp_vec.push_back(p);
      }

      return temp_vec;
    }
  else
    {
    // TODO: implement non-orthogonal statistics
    *out_sum += nan("");
    *out_sumsq += nan("");
    *cubicsum += nan("");
    *fourthsum += nan("");
    }
}

/**
  Get the RGBA apperance of the voxel at the intersection of the three
  display slices.
  */
template<class TTraits, class TBase>
void
ScalarImageWrapper<TTraits,TBase>
::GetVoxelUnderCursorDisplayedValueAndAppearance(
    vnl_vector<double> &out_value, DisplayPixelType &out_appearance)
{
  // Look up the actual intensity of the voxel from the slicer
  typename SlicerType::OutputPixelType pix_raw =
      this->m_Slicer[0]->LookupIntensityAtSliceIndex(this->m_ReferenceSpace);

  // The display value is mapped to native
  out_value.set_size(1);
  out_value[0] = this->m_NativeMapping(pix_raw);

  // Use the display mapping to map to display pixel
  out_appearance = this->m_DisplayMapping->MapPixel(pix_raw);
}

//template<class TTraits, class TBase>
//void
//ScalarImageWrapper<TTraits,TBase>
//::SetupVTKImportExport()
//{
//  // Initialize the VTK Exporter
//  m_VTKExporter = VTKExporter::New();
//  m_VTKExporter->ReleaseDataFlagOn();
//
//  // Initialize the VTK Importer
//  m_VTKImporter = vtkImageImport::New();
//  m_VTKImporter->ReleaseDataFlagOn();
//
//  // Pipe the importer into the exporter (that's a lot of code)
//  m_VTKImporter->SetUpdateInformationCallback(
//        m_VTKExporter->GetUpdateInformationCallback());
//  m_VTKImporter->SetPipelineModifiedCallback(
//        m_VTKExporter->GetPipelineModifiedCallback());
//  m_VTKImporter->SetWholeExtentCallback(
//        m_VTKExporter->GetWholeExtentCallback());
//  m_VTKImporter->SetSpacingCallback(
//        m_VTKExporter->GetSpacingCallback());
//  m_VTKImporter->SetOriginCallback(
//        m_VTKExporter->GetOriginCallback());
//  m_VTKImporter->SetScalarTypeCallback(
//        m_VTKExporter->GetScalarTypeCallback());
//  m_VTKImporter->SetNumberOfComponentsCallback(
//        m_VTKExporter->GetNumberOfComponentsCallback());
//  m_VTKImporter->SetPropagateUpdateExtentCallback(
//        m_VTKExporter->GetPropagateUpdateExtentCallback());
//  m_VTKImporter->SetUpdateDataCallback(
//        m_VTKExporter->GetUpdateDataCallback());
//  m_VTKImporter->SetDataExtentCallback(
//        m_VTKExporter->GetDataExtentCallback());
//  m_VTKImporter->SetBufferPointerCallback(
//        m_VTKExporter->GetBufferPointerCallback());
//  m_VTKImporter->SetCallbackUserData(
//        m_VTKExporter->GetCallbackUserData());
//}

template<class TTraits, class TBase>
vtkImageImport *
ScalarImageWrapper<TTraits,TBase>
::GetVTKImporter()
{
  return m_VTKImporter;
}

template<class TTraits, class TBase>
typename ScalarImageWrapper<TTraits, TBase>::CommonFormatImageType *
ScalarImageWrapper<TTraits, TBase>
::GetCommonFormatImage(ExportChannel channel)
{
  return m_CommonRepresentationPolicy.GetOutput(channel);
}

template<class TTraits, class TBase>
IntensityCurveInterface *
ScalarImageWrapper<TTraits, TBase>
::GetIntensityCurve() const
{
  return this->m_DisplayMapping->GetIntensityCurve();
}

template<class TTraits, class TBase>
ColorMap *
ScalarImageWrapper<TTraits, TBase>
::GetColorMap() const
{
  return this->m_DisplayMapping->GetColorMap();
}

template<class TTraits, class TBase>
const ScalarImageHistogram *
ScalarImageWrapper<TTraits,TBase>
::GetHistogram(size_t nBins)
{
  // If the user passes in a non-zero number of bins, we pass that as a
  // parameter to the filter
  if(nBins > 0)
    m_HistogramFilter->SetNumberOfBins(nBins);

  m_HistogramFilter->Update();
  return m_HistogramFilter->GetHistogramOutput();
}

template<class TTraits, class TBase>
void
ScalarImageWrapper<TTraits, TBase>
::WriteToFileAsFloat(const char *fname, Registry &hints)
{
  SmartPtr<GuidedNativeImageIO> io = GuidedNativeImageIO::New();
  io->CreateImageIO(fname, hints, false);
  itk::ImageIOBase *base = io->GetIOBase();

  SmartPtr<FloatImageSource> pipeline = this->CreateCastToFloatPipeline();

  typedef itk::ImageFileWriter<FloatImageType> WriterType;
  SmartPtr<WriterType> writer = WriterType::New();
  writer->SetFileName(fname);
  if(base)
    writer->SetImageIO(base);
  writer->SetInput(pipeline->GetOutput());
  writer->Update();
}


template class ScalarImageWrapper<LabelImageWrapperTraits>;
template class ScalarImageWrapper<SpeedImageWrapperTraits>;
template class ScalarImageWrapper<LevelSetImageWrapperTraits>;
template class ScalarImageWrapper< ComponentImageWrapperTraits<GreyType> >;
template class ScalarImageWrapper< AnatomicScalarImageWrapperTraits<GreyType> >;

typedef VectorDerivedQuantityImageWrapperTraits<GreyVectorToScalarMagnitudeFunctor> MagTraits;
typedef VectorDerivedQuantityImageWrapperTraits<GreyVectorToScalarMaxFunctor> MaxTraits;
typedef VectorDerivedQuantityImageWrapperTraits<GreyVectorToScalarMeanFunctor> MeanTraits;
template class ScalarImageWrapper<MagTraits>;
template class ScalarImageWrapper<MaxTraits>;
template class ScalarImageWrapper<MeanTraits>;

