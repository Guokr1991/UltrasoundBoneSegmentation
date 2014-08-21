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

#include <vtkMRMLSequenceNode.h>
#include <vtkMRMLScalarVolumeNode.h>

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
    vtkSmartPointer<vtkImageGaussianSmooth> gaussianSmoothFilter = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    gaussianSmoothFilter->SetInputConnection(currentFrame->GetImageDataConnection());
    gaussianSmoothFilter->Update();
    currentFrame->SetAndObserveImageData(gaussianSmoothFilter->GetOutput());
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
