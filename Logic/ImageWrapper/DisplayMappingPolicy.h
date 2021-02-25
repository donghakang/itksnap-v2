#ifndef DISPLAYMAPPINGPOLICY_H
#define DISPLAYMAPPINGPOLICY_H

#include "ImageWrapperBase.h"
#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "IntensityToColorLookupTableImageFilter.h"
#include "MultiChannelDisplayMode.h"

class ColorLabelTable;
class LabelToRGBAFilter;
class IntensityCurveVTK;
class Registry;
template <class TEnum> class RegistryEnumMap;
template <class T, class U> class LookupTableIntensityMappingFilter;
template <class T> class RGBALookupTableIntensityMappingFilter;
template <class T, typename U> class InputSelectionImageFilter;

/**
 * @brief An abstract class describing intensity mapping between image
 * intensities stored internally in an itk image and RGBA intensities
 * displayed to the user. This class is a parent in a hierarchy of
 * policies used to customize ImageWrapper behavior.
 */
class AbstractDisplayMappingPolicy : public itk::DataObject
{
public:

  irisITKAbstractObjectMacro(AbstractDisplayMappingPolicy, itk::DataObject)

  typedef ImageWrapperBase::DisplaySliceType DisplaySliceType;
  typedef ImageWrapperBase::DisplaySlicePointer DisplaySlicePointer;

  virtual IntensityCurveInterface *GetIntensityCurve() const = 0;
  virtual ColorMap *GetColorMap() const = 0;

  virtual DisplaySlicePointer GetDisplaySlice(unsigned int slice) = 0;

  virtual void Save(Registry &folder) = 0;
  virtual void Restore(Registry &folder) = 0;
};

class AbstractColorLabelTableDisplayMappingPolicy : public AbstractDisplayMappingPolicy
{
public:

  irisITKAbstractObjectMacro(AbstractColorLabelTableDisplayMappingPolicy,
                             AbstractDisplayMappingPolicy)

  /**
   * Set the table of color labels used to produce color slice images
   */
  virtual void SetLabelColorTable(ColorLabelTable *labels) = 0;

  /**
   * Get the color label table
   */
  virtual ColorLabelTable *GetLabelColorTable() const = 0;
};

template <class TWrapperTraits>
class ColorLabelTableDisplayMappingPolicy
    : public AbstractColorLabelTableDisplayMappingPolicy
{
public:

  irisITKObjectMacro(ColorLabelTableDisplayMappingPolicy<TWrapperTraits>,
                     AbstractColorLabelTableDisplayMappingPolicy)

  typedef typename TWrapperTraits::WrapperType WrapperType;
  typedef typename TWrapperTraits::ImageType ImageType;
  typedef itk::Image<LabelType, 2> InputSliceType;
  typedef ImageWrapperBase::DisplaySliceType DisplaySliceType;
  typedef ImageWrapperBase::DisplaySlicePointer DisplaySlicePointer;

  typedef InputSliceType::PixelType InputPixelType;
  typedef DisplaySliceType::PixelType DisplayPixelType;

  /**
   * Set the table of color labels used to produce color slice images
   */
  void SetLabelColorTable(ColorLabelTable *labels) ITK_OVERRIDE;

  /**
   * Get the color label table
   */
  ColorLabelTable *GetLabelColorTable() const ITK_OVERRIDE;

  void Initialize(WrapperType *wrapper);
  void UpdateImagePointer(ImageType *image);

  DisplaySlicePointer GetDisplaySlice(unsigned int slice) ITK_OVERRIDE;

  virtual IntensityCurveInterface *GetIntensityCurve() const ITK_OVERRIDE { return NULL; }
  virtual ColorMap *GetColorMap() const ITK_OVERRIDE { return NULL; }

  virtual void Save(Registry &folder) ITK_OVERRIDE {}
  virtual void Restore(Registry &folder) ITK_OVERRIDE {}

  virtual DisplayPixelType MapPixel(const InputPixelType &val);

protected:

  ColorLabelTableDisplayMappingPolicy();
  ~ColorLabelTableDisplayMappingPolicy();

  typedef LabelToRGBAFilter RGBAFilterType;
  typedef SmartPtr<RGBAFilterType> RGBAFilterPointer;

  RGBAFilterPointer m_RGBAFilter[3];
  WrapperType *m_Wrapper;
};

/**
 * @brief The parent class for the policies that involve curve-based mappings,
 * for both scalar and vector images.
 */
class AbstractContinuousImageDisplayMappingPolicy : public AbstractDisplayMappingPolicy
{
public:
  irisITKAbstractObjectMacro(AbstractContinuousImageDisplayMappingPolicy,
                             AbstractDisplayMappingPolicy)

  /**
   * @brief Get the intensity range relative to which the contrast mapping
   * curve is constructed. This is primarily used when displaying the curve
   * to the user.
   * @return Vector containing min/max of the curve range (in native units)
   */
  virtual Vector2d GetNativeImageRangeForCurve() = 0;

  /**
   * @brief Get the histogram associated with the current state of the display
   * policy. For single-component layers, this method just returns the
   * component's histogram. For multi-component layers, it may return the
   * pooled histogram, e.g., when the display is in RGB mode
   * @param nBins Number of bins desired in the histogram
   */
  virtual const ScalarImageHistogram *GetHistogram(int nBins) = 0;

  virtual void SetColorMap(ColorMap *map) = 0;

  /**
   * Automatically fit the contrast mapping based on the percentiles of
   * the image histogram.
   * TODO: the accuracy of this is currently limited by the bin size in
   * the histogram. At some point, it may make sense to have separate
   * histograms for display and for auto-fitting.
   */
  virtual void AutoFitContrast();


  /*
   * Written By 강동하
   * This function is to set level and window as specific value
  */
  virtual void DefaultFitContrast(); 


  /**
   * Has the intensity curve been adjusted from its default (reset) state?
   */
  virtual bool IsContrastInDefaultState();

  /**
   * Get the intensity min/max in native image intensity units
   */
  Vector2d GetCurveMinMaxNative();

};

class AbstractCachingAndColorMapDisplayMappingPolicy
    : public AbstractContinuousImageDisplayMappingPolicy
{
public:
  irisITKAbstractObjectMacro(AbstractCachingAndColorMapDisplayMappingPolicy,
                             AbstractContinuousImageDisplayMappingPolicy)

};








template<class TWrapperTraits>
class CachingCurveAndColorMapDisplayMappingPolicy
    : public AbstractCachingAndColorMapDisplayMappingPolicy
{
public:

  irisITKObjectMacro(CachingCurveAndColorMapDisplayMappingPolicy<TWrapperTraits>,
                     AbstractCachingAndColorMapDisplayMappingPolicy)

  typedef typename TWrapperTraits::WrapperType WrapperType;
  typedef typename TWrapperTraits::ImageType ImageType;
  typedef typename ImageType::PixelType PixelType;
  typedef typename TWrapperTraits::ComponentType ComponentType;
  typedef itk::Image<PixelType, 2> InputSliceType;

  typedef ImageWrapperBase::DisplaySliceType DisplaySliceType;
  typedef ImageWrapperBase::DisplaySlicePointer DisplaySlicePointer;
  typedef ImageWrapperBase::DisplayPixelType DisplayPixelType;

  // Lookup table
  typedef itk::Image<DisplayPixelType, 1>                     LookupTableType;

  // Min/Max inputs
  typedef itk::SimpleDataObjectDecorator<ComponentType>   ComponentObjectType;


  /**
   * @brief Copy the LUT, intensity curve, and color map from another display
   * mapping. This allows memory savings by sharing the LUT among multiple
   * wrappers. This is used with components on a multi-component image
   */
  void CopyDisplayPipeline(const Self *reference);

  void DeepCopyIntensityMap(WrapperType *srcWrapper);

  /**
   * Set the reference intensity range for the lookup table and intensity curve.
   * This means that the LUT is constructed not between the image min and image
   * max, but between some other pair of values. This is useful when the image
   * in the imagewrapper is a subregion of another image, or when the image is
   * a component of a multi-component image.
   */
  void SetReferenceIntensityRange(ComponentObjectType *refMin,
                                  ComponentObjectType *refMax);

  void ClearReferenceIntensityRange();

  Vector2d GetNativeImageRangeForCurve() ITK_OVERRIDE;

  virtual const ScalarImageHistogram *GetHistogram(int nBins) ITK_OVERRIDE;


  /**
   * Get the display slice in a given direction.  To change the
   * display slice, call parent's MoveToSlice() method
   */
  DisplaySlicePointer GetDisplaySlice(unsigned int dim) ITK_OVERRIDE;

  /**
    Get a pointer to the colormap
    */
  ColorMap *GetColorMap () const ITK_OVERRIDE;

  void SetColorMap(ColorMap *map) ITK_OVERRIDE;

  /**
   * Get a pointer to the intensity curve
   */
  virtual IntensityCurveInterface *GetIntensityCurve() const ITK_OVERRIDE;

  /**
   * Set a new intensity curve
   */
  void SetIntensityCurve(IntensityCurveInterface *curve);

  void Initialize(WrapperType *wrapper);
  void UpdateImagePointer(ImageType *image);

  virtual void Save(Registry &folder) ITK_OVERRIDE;
  virtual void Restore(Registry &folder) ITK_OVERRIDE;

  virtual DisplayPixelType MapPixel(const PixelType &val);


protected:

  CachingCurveAndColorMapDisplayMappingPolicy();
  ~CachingCurveAndColorMapDisplayMappingPolicy();


  // Filter that generates the lookup table
  typedef IntensityToColorLookupTableImageFilter<
              ImageType, LookupTableType>               LookupTableFilterType;

  // Filter that applies the lookup table to slices
  typedef LookupTableIntensityMappingFilter<
                InputSliceType, DisplaySliceType>         IntensityFilterType;


  // LUT generator
  SmartPtr<LookupTableFilterType> m_LookupTableFilter;

  // Filters for the three slice directions
  SmartPtr<IntensityFilterType> m_IntensityFilter[3];

  /**
   * Implementation of the intensity curve funcitonality. The intensity map
   * transforms input intensity values into the range [0 1], which serves as
   * the input into the color map transform.
   */
  SmartPtr<IntensityCurveVTK> m_IntensityCurveVTK;

  /**
    * Color map, which maps intensity values normalized to the range [0 1]
    * to output RGB values
    */
  SmartPtr<ColorMap> m_ColorMap;

  WrapperType *m_Wrapper;

};


/*
template<class TWrapperTraits>
class SpeedImageWrapperDisplayPolicy
    : public LinearColorMapDisplayMappingPolicy
{
public:
  irisITKObjectMacro(SpeedImageWrapperDisplayPolicy<TWrapperTraits>,
                     CachingCurveAndColorMapDisplayMappingPolicy<TWrapperTraits>)

  void UpdateImagePointer(ImageType *image);

protected:

  SpeedImageWrapperDisplayPolicy() {}
  virtual ~SpeedImageWrapperDisplayPolicy() {}

};
*/





/**
 * Display mapping policy for speed and level set images, i.e., images that
 * have a defined range of intensity and a colormap that takes them from
 * intensity values to display RGB values. This policy can work with both
 * integral and floating point types.
 */
class AbstractLinearColorMapDisplayMappingPolicy : public AbstractDisplayMappingPolicy
{
public:
  irisITKAbstractObjectMacro(AbstractLinearColorMapDisplayMappingPolicy,
                             AbstractContinuousImageDisplayMappingPolicy)

};

namespace itk {
  template <class TInput,class TOutput,class TFunctor>
    class UnaryFunctorImageFilter;
}

template <class TWrapperTraits>
class LinearColorMapDisplayMappingPolicy
    : public AbstractLinearColorMapDisplayMappingPolicy
{
public:

  irisITKObjectMacro(LinearColorMapDisplayMappingPolicy<TWrapperTraits>,
                     AbstractLinearColorMapDisplayMappingPolicy)

  typedef typename TWrapperTraits::WrapperType WrapperType;
  typedef typename TWrapperTraits::ImageType ImageType;
  typedef typename ImageType::PixelType PixelType;

  typedef itk::Image<PixelType, 2> InputSliceType;
  typedef ImageWrapperBase::DisplaySliceType DisplaySliceType;
  typedef ImageWrapperBase::DisplaySlicePointer DisplaySlicePointer;
  typedef ImageWrapperBase::DisplayPixelType DisplayPixelType;

  void Initialize(WrapperType *wrapper);
  void UpdateImagePointer(ImageType *image);

  virtual DisplaySlicePointer GetDisplaySlice(unsigned int slice) ITK_OVERRIDE;

  virtual IntensityCurveInterface *GetIntensityCurve() const ITK_OVERRIDE { return NULL; }

  virtual void Save(Registry &folder) ITK_OVERRIDE;
  virtual void Restore(Registry &folder) ITK_OVERRIDE;

  irisGetMacroWithOverride(ColorMap, ColorMap *)

  DisplayPixelType MapPixel(const PixelType &xin);

protected:

  LinearColorMapDisplayMappingPolicy();
  virtual ~LinearColorMapDisplayMappingPolicy();

  class MappingFunctor
  {
  public:
    DisplayPixelType operator()(PixelType in);
    double m_Scale, m_Shift;
    ColorMap *m_ColorMap;
    bool operator != (const MappingFunctor &f);
  };

  typedef itk::UnaryFunctorImageFilter
    <InputSliceType, DisplaySliceType, MappingFunctor> IntensityFilterType;
  typedef SmartPtr<IntensityFilterType> IntensityFilterPointer;

  IntensityFilterPointer m_Filter[3];
  MappingFunctor m_Functor;

  SmartPtr<ColorMap> m_ColorMap;
  WrapperType *m_Wrapper;
};


class AbstractMultiChannelDisplayMappingPolicy
    : public AbstractContinuousImageDisplayMappingPolicy
{
public:
  irisITKAbstractObjectMacro(AbstractMultiChannelDisplayMappingPolicy,
                             AbstractContinuousImageDisplayMappingPolicy)

  irisVirtualGetMacro(DisplayMode, MultiChannelDisplayMode)
  irisVirtualSetMacro(DisplayMode, MultiChannelDisplayMode)

  irisVirtualGetMacro(Animate, bool)
  irisVirtualSetMacro(Animate, bool)

};

template <class TWrapperTraits>
class MultiChannelDisplayMappingPolicy
    : public AbstractMultiChannelDisplayMappingPolicy
{
public:

  irisITKObjectMacro(MultiChannelDisplayMappingPolicy<TWrapperTraits>,
                     AbstractMultiChannelDisplayMappingPolicy)


  typedef typename TWrapperTraits::WrapperType WrapperType;
  typedef typename TWrapperTraits::ImageType ImageType;
  typedef typename ImageType::PixelType PixelType;
  typedef typename ImageType::InternalPixelType InternalPixelType;

  typedef itk::Image<InternalPixelType, 2> InputSliceType;

  typedef ImageWrapperBase::DisplaySliceType DisplaySliceType;
  typedef ImageWrapperBase::DisplaySlicePointer DisplaySlicePointer;
  typedef ImageWrapperBase::DisplayPixelType DisplayPixelType;

  void Initialize(WrapperType *wrapper);
  void UpdateImagePointer(ImageType *image);

  /** Set the display mode */
  virtual void SetDisplayMode(MultiChannelDisplayMode mode) ITK_OVERRIDE;

  /** Get the display mode */
  irisGetMacroWithOverride(DisplayMode, MultiChannelDisplayMode)

  /** Get/Set the animation status */
  irisGetSetMacroWithOverride(Animate, bool)

  DisplaySlicePointer GetDisplaySlice(unsigned int slice) ITK_OVERRIDE;

  Vector2d GetNativeImageRangeForCurve() ITK_OVERRIDE;
  virtual const ScalarImageHistogram *GetHistogram(int nBins) ITK_OVERRIDE;

  /**
   * @brief Returns true when the display mode is such that the image min, max
   * and histogram used for contrast adjustment purposes use intensities pooled
   * from all of the components. This is the case when the components are being
   * animated or when display is in RGB mode.
   */
  bool IsContrastMultiComponent() const;

  virtual IntensityCurveInterface *GetIntensityCurve() const ITK_OVERRIDE;
  virtual ColorMap *GetColorMap() const ITK_OVERRIDE;
  virtual void SetColorMap(ColorMap *map) ITK_OVERRIDE;

  virtual void Save(Registry &folder) ITK_OVERRIDE;
  virtual void Restore(Registry &folder) ITK_OVERRIDE;

  virtual void AutoFitContrast() ITK_OVERRIDE;

  irisGetMacro(ScalarRepresentation, ScalarImageWrapperBase *)

  DisplayPixelType MapPixel(const PixelType &val);

protected:

  MultiChannelDisplayMappingPolicy();
  ~MultiChannelDisplayMappingPolicy();

  typedef RGBALookupTableIntensityMappingFilter<InputSliceType> ApplyLUTFilter;
  typedef itk::Image<unsigned char, 1>                         LookupTableType;

  typedef MultiComponentImageToScalarLookupTableImageFilter<ImageType, LookupTableType>
                                                             GenerateLUTFilter;

  MultiChannelDisplayMode m_DisplayMode;

  bool m_Animate;

  ScalarImageWrapperBase *m_ScalarRepresentation;
  WrapperType *m_Wrapper;

  SmartPtr<GenerateLUTFilter> m_LUTGenerator;
  SmartPtr<ApplyLUTFilter> m_RGBMapper[3];

  // Filters used to select the right pipeline for display
  typedef InputSelectionImageFilter<
    DisplaySliceType, MultiChannelDisplayMode> DisplaySliceSelector;
  SmartPtr<DisplaySliceSelector> m_DisplaySliceSelector[3];
};



#endif // DISPLAYMAPPINGPOLICY_H
