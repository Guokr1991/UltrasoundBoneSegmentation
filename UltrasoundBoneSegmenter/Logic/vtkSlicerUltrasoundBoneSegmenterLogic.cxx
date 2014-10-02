/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// UltrasoundBoneSegmenter Logic includes
#include "vtkSlicerUltrasoundBoneSegmenterLogic.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageLaplacian.h>
#include <vtkImageExport.h>
#include <vtkMRMLSequenceNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkImageData.h>

// ITK includes
#include <itkLaplacianRecursiveGaussianImageFilter.h>
#include <itkGradientRecursiveGaussianImageFilter.h>
#include <itkRecursiveGaussianImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkImage.h>

// STD includes
#include <cassert>
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerUltrasoundBoneSegmenterLogic);

//----------------------------------------------------------------------------
vtkSlicerUltrasoundBoneSegmenterLogic::vtkSlicerUltrasoundBoneSegmenterLogic()
{
  UltrasoundBoneSegmenterNode = 0;
}

//----------------------------------------------------------------------------
vtkSlicerUltrasoundBoneSegmenterLogic::~vtkSlicerUltrasoundBoneSegmenterLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerUltrasoundBoneSegmenterLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
int vtkSlicerUltrasoundBoneSegmenterLogic::Apply(vtkMRMLUltrasoundBoneSegmenterNode* paramNode, QProgressBar* progressBar)
{
  typedef itk::Image<unsigned char, 2> ImageType;
  typedef itk::LaplacianRecursiveGaussianImageFilter<ImageType, ImageType> filterType;
  //typedef itk::GradientRecursiveGaussianImageFilter<ImageType, ImageType> filterType;
  //typedef itk::RecursiveGaussianImageFilter<ImageType, ImageType> filterType;
  typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType> gaussianFilterType;
  typedef itk::ThresholdImageFilter<ImageType> thresholdFilterType;

  vtkMRMLScene* scene = this->GetMRMLScene();
  if (!scene)
  {
    vtkErrorMacro("Apply: Invalid scene.");
    return 1;
  }
  vtkMRMLSequenceNode* sequenceNode = vtkMRMLSequenceNode::SafeDownCast(scene->GetNodeByID(paramNode->GetUltrasoundSequenceNodeID()));
  int numberOfFrames = sequenceNode->GetNumberOfDataNodes();
  std::cout << "Number of frames: " << numberOfFrames << std::endl;
  int currentProgress = 0;
  progressBar->reset();
  progressBar->setTextVisible(true);

  for (int i = 0; i < numberOfFrames; i++)
  {

    vtkMRMLScalarVolumeNode* currentFrame = vtkMRMLScalarVolumeNode::SafeDownCast(sequenceNode->GetNthDataNode(i));
    vtkImageData* inputVolume = currentFrame->GetImageData();
    vtkSmartPointer<vtkImageExport> imageExport = vtkSmartPointer<vtkImageExport>::New();
    # if (VTK_MAJOR_VERSION <= 5)
      imageExport->SetInput(inputVolume);
    #else
      imageExport->SetInputData(inputVolume);
    #endif
    imageExport->Update();

    int inputExtent[6] = {0,0,0,0,0,0};
    inputVolume->GetExtent(inputExtent);
    ImageType::SizeType inputSize;
    inputSize[0] = inputExtent[1] - inputExtent[0] + 1;
    inputSize[1] = inputExtent[3] - inputExtent[2] + 1;
    //inputSize[2] = inputExtent[5] - inputExtent[4] + 1;

    ImageType::IndexType start;
    //start[0] = start[1] = start[2] = 0.0;
    start[0] = start[1] = 0.0;

    ImageType::RegionType region;
    region.SetSize(inputSize);
    region.SetIndex(start);

    ImageType::Pointer inputItkImage = ImageType::New();
    inputItkImage->SetRegions(region);
    try
    {
      inputItkImage->Allocate();
    }
    catch(itk::ExceptionObject &err)
    {
      std::cerr << "Error allocating memory for ITK image!" << std::endl;
    }
    imageExport->Export(inputItkImage->GetBufferPointer());

    /*
    filterType::Pointer gradientFilter = filterType::New();
    gradientFilter->SetInput(inputItkImage);
    gradientFilter->SetSigma(100);
    gradientFilter->Update();
    */
    /*
    filterType::Pointer gaussianFilter = filterType::New();
    gaussianFilter->SetInput(inputItkImage);
    gaussianFilter->SetDirection(1); // y-axis
    gaussianFilter->SetSecondOrder();
    gaussianFilter->Update();
    */
    thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
    thresholdFilter->SetInput(inputItkImage);
    thresholdFilter->ThresholdBelow(180);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->Update();
    gaussianFilterType::Pointer gaussianFilter = gaussianFilterType::New();
    gaussianFilter->SetInput(thresholdFilter->GetOutput());
    gaussianFilter->SetVariance(4.0);
    gaussianFilter->Update();

    filterType::Pointer laplacianFilter = filterType::New();
    laplacianFilter->SetInput(gaussianFilter->GetOutput());
    laplacianFilter->Update();

    unsigned char* vtkImageDataPtr = (unsigned char*)inputVolume->GetScalarPointer();
    itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(laplacianFilter->GetOutput(),region);
    for (imageIterator.GoToBegin(); !imageIterator.IsAtEnd(); ++imageIterator)
    {
      ImageType::IndexType i = imageIterator.GetIndex();
      (*vtkImageDataPtr) = laplacianFilter->GetOutput()->GetPixel(i);
      vtkImageDataPtr++;
    }
    /*
    vtkSmartPointer<vtkImageGaussianSmooth> gaussianSmoothFilter = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    gaussianSmoothFilter->SetInputConnection(currentFrame->GetImageDataConnection());
    gaussianSmoothFilter->Update();
    vtkSmartPointer<vtkImageLaplacian> laplacianFilter = vtkSmartPointer<vtkImageLaplacian>::New();
    laplacianFilter->SetInputConnection(gaussianSmoothFilter->GetOutputPort());
    laplacianFilter->Update();
    */
    //currentFrame->SetAndObserveImageData(laplacianFilter->GetOutput());
    currentFrame->SetAndObserveImageData(inputVolume);
    currentProgress = (i+1.0)/numberOfFrames*100;
    progressBar->setValue(currentProgress);
  }

  return 0;
}

//---------------------------------------------------------------------------
void vtkSlicerUltrasoundBoneSegmenterLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerUltrasoundBoneSegmenterLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerUltrasoundBoneSegmenterLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerUltrasoundBoneSegmenterLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerUltrasoundBoneSegmenterLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}
