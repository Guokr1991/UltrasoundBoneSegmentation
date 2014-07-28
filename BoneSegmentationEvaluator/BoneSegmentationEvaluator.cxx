#include "itkImageFileWriter.h"

#include "itkPluginUtilities.h"

#include "BoneSegmentationEvaluatorCLP.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkFlatStructuringElement.h"
#include "itkImage.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"

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
  typedef itk::ImageRegionIterator<ImageType> IteratorType;

  ReaderType::Pointer reader1 = ReaderType::New();
  ReaderType::Pointer reader2 = ReaderType::New();
  ReaderType::Pointer reader3 = ReaderType::New();
  ReaderType::Pointer reader4 = ReaderType::New();


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

  // Get iterator for  false negative verification meta image
  reader4->SetFileName(falseNegativeGroundTruthVolume.c_str());
  reader4->Update();
  ImageType::Pointer falseNegativeImage = reader4->GetOutput();
  IteratorType iterator4(falseNegativeImage, falseNegativeImage->GetRequestedRegion());

  // *** ITK for segmented meta image dilation ***
  reader3->SetFileName(falseNegativeGroundTruthVolume.c_str());
  reader3->Update();

  // Use cross structure for dilation
  typedef itk::FlatStructuringElement<3> StructElementType;

  // Set cross size to dilation value specified
  StructElementType::SizeType crossSize;
  crossSize[0] = dilateValue;
  crossSize[1] = dilateValue;
  crossSize[2] = 0;
  StructElementType structuringElement = StructElementType::Cross(crossSize);


  typedef itk::GrayscaleDilateImageFilter<ImageType, ImageType, StructElementType>
    GrayscaleDilateImageFilterType;
  GrayscaleDilateImageFilterType::Pointer dilateFilter =
    GrayscaleDilateImageFilterType::New();
  dilateFilter->SetInput(reader3->GetOutput());
  dilateFilter->SetKernel(structuringElement);
  dilateFilter->Update();


  ImageType::Pointer image = ImageType::New();
  image = dilateFilter->GetOutput();
  IteratorType iterator3(image, image->GetRequestedRegion());


  typedef itk::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName("test_dilate.mha");
  writer->SetInput(dilateFilter->GetOutput());
  try
  {
    writer->Update();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e.GetDescription() << std::endl;
    return EXIT_FAILURE;
  }
  int expectedPoints = 0;
  int falseNegative = 0;
  int segCount = 0;
  int truePositive = 0;
  while(!iterator1.IsAtEnd())
  {
    unsigned char segmentPixel = iterator1.Get();
    unsigned char truePositivePixel = iterator2.Get();
    unsigned char falseNegativePixel = iterator4.Get();
    unsigned char dilatedPixel = iterator3.Get();

    if (falseNegativePixel != 0)
    {
      expectedPoints++;
      if (dilatedPixel == 0)
      {
        falseNegative++;
      }
    }

    if (segmentPixel != 0)
    {
      segCount++;
      if (truePositivePixel != 0)
      {
        truePositive++;
      }
    }

    ++iterator1;
    ++iterator2;
    ++iterator3;
    ++iterator4;
  }
  std::cout << "Expected Points: " << expectedPoints << std::endl;
  std::cout << "False Negative: " << falseNegative << std::endl;
  std::cout << "Segmentation Count: " << segCount << std::endl;
  std::cout << "True Positive: " << truePositive << std::endl;

  falseNegativePercentage = (float)falseNegative/(float)expectedPoints * 100;
  truePositivePercentage = truePositive/(float)segCount * 100;

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

  std::ofstream rts;
  rts.open(returnParameterFile.c_str());
  rts << "truePositivePercentage = " << truePositivePercentage << std::endl;
  rts << "falseNegativePercentage = " << falseNegativePercentage << std::endl;
  rts.close();

  return EXIT_SUCCESS;
}
