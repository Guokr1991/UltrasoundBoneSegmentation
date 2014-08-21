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

// Qt includes
#include <QDebug>


// SlicerQt includes
#include "qSlicerUltrasoundBoneSegmenterModuleWidget.h"
#include "ui_qSlicerUltrasoundBoneSegmenterModuleWidget.h"

// UltrasoundBoneSegmenter Logic includes
#include "vtkSlicerUltrasoundBoneSegmenterLogic.h"
#include "vtkMRMLUltrasoundBoneSegmenterNode.h"

#include <qSlicerAbstractCoreModule.h>
#include <qMRMLNodeFactory.h>
#include <vtkNew.h>

#include <iostream>
//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate: public Ui_qSlicerUltrasoundBoneSegmenterModuleWidget
{
  Q_DECLARE_PUBLIC(qSlicerUltrasoundBoneSegmenterModuleWidget);
protected:
  qSlicerUltrasoundBoneSegmenterModuleWidget* const q_ptr;
public:
  qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate(qSlicerUltrasoundBoneSegmenterModuleWidget& object);
  ~qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate();
  vtkSlicerUltrasoundBoneSegmenterLogic* logic() const;
};

//-----------------------------------------------------------------------------
// qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate::qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate(qSlicerUltrasoundBoneSegmenterModuleWidget& object) : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate::~qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate()
{
}

vtkSlicerUltrasoundBoneSegmenterLogic* qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerUltrasoundBoneSegmenterModuleWidget);
  return vtkSlicerUltrasoundBoneSegmenterLogic::SafeDownCast(q->logic());
}
//-----------------------------------------------------------------------------
// qSlicerUltrasoundBoneSegmenterModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterModuleWidget::qSlicerUltrasoundBoneSegmenterModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerUltrasoundBoneSegmenterModuleWidgetPrivate(*this) )
{
  paramNode = 0;
}

//-----------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterModuleWidget::~qSlicerUltrasoundBoneSegmenterModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerUltrasoundBoneSegmenterModuleWidget::setup()
{
  Q_D(qSlicerUltrasoundBoneSegmenterModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
  std::cout << "Edit Number 2" << std::endl;
  connect(d->pushButton_Segment, SIGNAL(clicked()), this, SLOT(onApply()));
  connect(d->MRMLNodeComboBox_UltrasoundSequence, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onUltrasoundSequenceChanged(vtkMRMLNode*)));
}

//-----------------------------------------------------------------------------
void qSlicerUltrasoundBoneSegmenterModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  this->Superclass::setMRMLScene(scene);
  if (scene == NULL)
  {
    return;
  }

  this->initializeUltrasoundBoneSegmenterNode(scene);
}

//-----------------------------------------------------------------------------
void qSlicerUltrasoundBoneSegmenterModuleWidget::initializeUltrasoundBoneSegmenterNode(vtkMRMLScene* scene)
{
  vtkCollection* paramNodes = scene->GetNodesByClass("vtkMRMLUltrasoundBoneSegmenterNode");

  if (paramNodes->GetNumberOfItems() > 0)
  {
    this->paramNode = vtkMRMLUltrasoundBoneSegmenterNode::SafeDownCast(paramNodes->GetItemAsObject(0));
    if (!this->paramNode)
    {
      qCritical() << "Error: Cannot instantiate paramNode";
    }
  }
  else
  {
    qDebug() << "No UltrasoundBoneSegmenter parameter nodes found.";
    this->paramNode = vtkMRMLUltrasoundBoneSegmenterNode::New();
    scene->AddNode(this->paramNode);
    this->paramNode->Delete();
  }
  paramNodes->Delete();
}

void qSlicerUltrasoundBoneSegmenterModuleWidget::onUltrasoundSequenceChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerUltrasoundBoneSegmenterModuleWidget);
  if (!node)
  {
    return;
  }

  if ((vtkMRMLSequenceNode::SafeDownCast(node)->GetDataNodeClassName()).compare("vtkMRMLScalarVolumeNode") == 0)
  {
    std::cout << "Ultrasound sequence node IS vtkMRMLScalarVolumeNode" << std::endl;
    d->pushButton_Segment->setEnabled(true);
  }
  else
  {
    std::cout << "Ultrasound sequence node IS NOT vtkMRMLScalarVolumeNode" << std::endl;
    d->pushButton_Segment->setEnabled(false);
  }
}

//-----------------------------------------------------------------------------
void qSlicerUltrasoundBoneSegmenterModuleWidget::onApply()
{
  Q_D(const qSlicerUltrasoundBoneSegmenterModuleWidget);

  vtkSlicerUltrasoundBoneSegmenterLogic *logic = d->logic();
  if (!this->paramNode || !d->MRMLNodeComboBox_UltrasoundSequence->currentNode())
  {
    return;
  }
  else
  {
    std::cout << "ID: " << d->MRMLNodeComboBox_UltrasoundSequence->currentNode()->GetID() << std::endl;
    this->paramNode->SetUltrasoundSequenceNodeID(d->MRMLNodeComboBox_UltrasoundSequence->currentNode()->GetID());
    logic->Apply(this->paramNode, d->progressBar);
  }
}
