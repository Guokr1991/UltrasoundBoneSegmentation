#include "itkImageFileWriter.h"

#include "itkPluginUtilities.h"

#include "BoneSegmentationEvaluatorCLP.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkFlatStructuringElement.h"
#include "itkImage.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkAndImageFilter.h"
#include "itkNotImageFilter.h"
#include "itkIndex.h"
#include "itkImageMaskSpatialObject.h"
#include <time.h>

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace
{

template <class T>
int DoIt( int argc, char * argv[], T )
{
  PARSE_ARGS;

  typedef itk::Image<T, 3> ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;

  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();
  ReaderType::Pointer reader3 = ReaderType::New();
  ReaderType::Pointer reader4 = ReaderType::New();

  // Timing
  time_t start,end;
  time(&start);

  // Get iterator for segmented meta image
  reader1->SetFileName(segmentedVolume.c_str());
  reader1->Update();
  ImageType::Pointer segmentedVolumeImage = reader1->GetOutput();
  IteratorType iterator1(segmentedVolumeImage, segmentedVolumeImage->GetRequestedRegion());

  // Get iterator for true positive verification meta image
  reader2->SetFileName(truePositiveGroundTruthVolume.c_str());
  reader2->Update();
  ImageType::Pointer truePositiveImage = reader2->GetOutput();
  IteratorType iterator2(truePositiveImage, truePositiveImage->GetRequestedRegion());

  // Get iterator for false negative verification meta image
  reader4->SetFileName(falseNegativeGroundTruthVolume.c_str());
  reader4->Update();
  ImageType::Pointer falseNegativeImage = reader4->GetOutput();
  IteratorType iterator4(falseNegativeImage, falseNegativeImage->GetRequestedRegion());
  
  // For checking the sizes of each image, need to be the same
  ImageType::RegionType segmentedRegion = segmentedVolumeImage->GetLargestPossibleRegion();
  ImageType::RegionType truePositiveRegion = truePositiveImage->GetLargestPossibleRegion();
  ImageType::RegionType falseNegativeRegion = falseNegativeImage->GetLargestPossibleRegion();
  ImageType::SizeType segmentedSize = segmentedRegion.GetSize();
  ImageType::SizeType truePositiveSize = truePositiveRegion.GetSize();
  ImageType::SizeType falseNegativeSize = falseNegativeRegion.GetSize();

  if (!(segmentedSize[0] == truePositiveSize[0] && segmentedSize[0] == falseNegativeSize[0] &&
      segmentedSize[1] == truePositiveSize[1] && segmentedSize[1] == falseNegativeSize[1] &&
      segmentedSize[2] == truePositiveSize[2] && segmentedSize[2] == falseNegativeSize[2]))
  {
    return EXIT_FAILURE;
  }
  
  // Iterator for segmented meta image dilation
  reader3->SetFileName(segmentedVolume.c_str());
  reader3->Update();

  // Use cross structure for dilation
  typedef itk::FlatStructuringElement<3> StructElementType;

  // Set cross size to dilation value specified
  StructElementType::SizeType crossSize;
  crossSize[0] = dilateValue;
  crossSize[1] = dilateValue;
  crossSize[2] = 0;
  StructElementType structuringElement = StructElementType::Cross(crossSize);

  // Dilate the algorithms segmentation
  typedef itk::BinaryDilateImageFilter<ImageType, ImageType, StructElementType>
    BinaryDilateImageFilterType;
  BinaryDilateImageFilterType::Pointer dilateFilter =
    BinaryDilateImageFilterType::New();
  dilateFilter->SetInput(reader3->GetOutput());
  dilateFilter->SetKernel(structuringElement);
  dilateFilter->SetDilateValue(1);
  dilateFilter->Update();

  ImageType::Pointer dilatedImage = ImageType::New();
  dilatedImage = dilateFilter->GetOutput();
  IteratorType iterator3(dilatedImage, dilatedImage->GetRequestedRegion());

  // Values for the entire volume
  int expectedPoints = 0;
  int falseNegative = 0;
  int segCount = 0;
  int truePositive = 0;

  // Values for the current frame
  int currentFrameTruePositiveCount = 0;
  int currentFrameFalseNegativeCount = 0;
  int currentFrameExpectedCount = 0;
  int currentFrameSegmentationCount = 0;

  // Percentages for the current frame / best & worst overall
  float currentFalseNegativePercentage = 0;
  float currentTruePositivePercentage = 0;
  float worstFalseNegativePercentage = 0;
  float bestTruePositivePercentage = 0;

  // For determining when a new frame is reached meaning per frame calculations have to be done
  itk::Index<3> currentIndex;
  currentIndex[0] = currentIndex[1] = currentIndex[2] = 0;
  int dilatedPixelCount = 0;
  while(!iterator1.IsAtEnd())
  {
    unsigned char segmentPixel = iterator1.Get();
    unsigned char truePositivePixel = iterator2.Get();
    unsigned char falseNegativePixel = iterator4.Get();
    unsigned char dilatedPixel = iterator3.Get();

    // New Frame, so calculate the statistics for the recently iterated frame
    if (iterator1.GetIndex()[2] != currentIndex[2])
    {
      // False negative & false positive calculation
      currentFalseNegativePercentage = (float)currentFrameFalseNegativeCount/(float)currentFrameExpectedCount * 100;
      currentTruePositivePercentage = currentFrameTruePositiveCount/currentFrameSegmentationCount * 100;

      // If the recently iterated frame has a higher false negative percentage, save the frame # and value
      if (currentFalseNegativePercentage > worstFalseNegativePercentage)
      {
	worstFalseNegativePercentage = currentFalseNegativePercentage;
	worstFalseNegativeFrameNumber = currentIndex[2];
      }

      // If the recently iterated frame has a higher true positive percentage, save the frame # and vaslue
      if (currentTruePositivePercentage > bestTruePositivePercentage)
      {
	bestTruePositivePercentage = currentTruePositivePercentage;
	bestTruePositiveFrameNumber = currentIndex[2];
      }

      // Reset values for next frame calculation
      currentFrameExpectedCount = 0;
      currentFrameFalseNegativeCount = 0;
      currentFrameTruePositiveCount = 0;
      currentFrameSegmentationCount = 0;
    }
      
    // If the current pixel was set in the false negative test line
    if (falseNegativePixel != 0)
    {
      // Increment total/frame expected counts
      expectedPoints++;
      currentFrameExpectedCount++;
      // If the dilated segmentation was not set then it is a false negative
      if (dilatedPixel == 0)
      {
	// Increment total/frame false negative counts
        falseNegative++;
	currentFrameFalseNegativeCount++;
      }
    }

    // If the current pixel was set in the segmentation
    if (segmentPixel != 0)
    {
      // Increment the total/frame segmentation counts
      segCount++;
      currentFrameSegmentationCount++;
      // If the true positive test region was set then it is a true positive
      if (truePositivePixel != 0)
      {
	// Increment the total/frame true positive counts
        truePositive++;
	currentFrameTruePositiveCount++;
      }
    }

    if (dilatedPixel != 0)
    {
      dilatedPixelCount++;
    }
    currentIndex = iterator1.GetIndex(); // Set the index for the pixel iterated (for checking if the next pixel is in a new frame)
    // Iterate to next pixel for each image
    ++iterator1;
    ++iterator2;
    ++iterator3;
    ++iterator4;
  }
  std::cout << "Dilated pixel count: " << dilatedPixelCount << std::endl;
  // Calculate metrics for total volume
  falseNegativePercentage = (float)falseNegative/(float)expectedPoints * 100;
  truePositivePercentage = truePositive/(float)segCount * 100;

  // Write module output values
  std::ofstream rts;
  rts.open(returnParameterFile.c_str());
  rts << "falseNegativePercentage = " << falseNegativePercentage << std::endl;
  rts << "truePositivePercentage = " << truePositivePercentage << std::endl;
  rts << "bestTruePositiveFrameNumber = " << bestTruePositiveFrameNumber << std::endl;
  rts << "worstFalseNegativeFrameNumber = " << worstFalseNegativeFrameNumber << std::endl;
  rts.close();
  
  time(&end);
  double dif = difftime(end,start);
  std::cout << "Time to run: " << dif << std::endl;
  
  return EXIT_SUCCESS;
}

} // end of anonymous namespace

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  itk::ImageIOBase::IOPixelType     pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  try
    {
    itk::GetImageType(segmentedVolume, pixelType, componentType);

    // This filter handles all types on input, but only produces
    // signed types
    switch( componentType )
      {
      case itk::ImageIOBase::UCHAR:
        return DoIt( argc, argv, static_cast<unsigned char>(0) );
        break;
      case itk::ImageIOBase::CHAR:
        return DoIt( argc, argv, static_cast<char>(0) );
        break;
      case itk::ImageIOBase::USHORT:
        return DoIt( argc, argv, static_cast<unsigned short>(0) );
        break;
      case itk::ImageIOBase::SHORT:
        return DoIt( argc, argv, static_cast<short>(0) );
        break;
      case itk::ImageIOBase::UINT:
        return DoIt( argc, argv, static_cast<unsigned int>(0) );
        break;
      case itk::ImageIOBase::INT:
        return DoIt( argc, argv, static_cast<int>(0) );
        break;
      case itk::ImageIOBase::ULONG:
        return DoIt( argc, argv, static_cast<unsigned long>(0) );
        break;
      case itk::ImageIOBase::LONG:
        return DoIt( argc, argv, static_cast<long>(0) );
        break;
      case itk::ImageIOBase::FLOAT:
        return DoIt( argc, argv, static_cast<float>(0) );
        break;
      case itk::ImageIOBase::DOUBLE:
        return DoIt( argc, argv, static_cast<double>(0) );
        break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }

  catch( itk::ExceptionObject & excep )
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
