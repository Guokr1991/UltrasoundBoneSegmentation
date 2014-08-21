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
#include <QtPlugin>

// UltrasoundBoneSegmenter Logic includes
#include <vtkSlicerUltrasoundBoneSegmenterLogic.h>

// UltrasoundBoneSegmenter includes
#include "qSlicerUltrasoundBoneSegmenterModule.h"
#include "qSlicerUltrasoundBoneSegmenterModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerUltrasoundBoneSegmenterModule, qSlicerUltrasoundBoneSegmenterModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerUltrasoundBoneSegmenterModulePrivate
{
public:
  qSlicerUltrasoundBoneSegmenterModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerUltrasoundBoneSegmenterModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterModulePrivate::qSlicerUltrasoundBoneSegmenterModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerUltrasoundBoneSegmenterModule methods

//-----------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterModule::qSlicerUltrasoundBoneSegmenterModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerUltrasoundBoneSegmenterModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterModule::~qSlicerUltrasoundBoneSegmenterModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerUltrasoundBoneSegmenterModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerUltrasoundBoneSegmenterModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerUltrasoundBoneSegmenterModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerUltrasoundBoneSegmenterModule::icon() const
{
  return QIcon(":/Icons/UltrasoundBoneSegmenter.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerUltrasoundBoneSegmenterModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerUltrasoundBoneSegmenterModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerUltrasoundBoneSegmenterModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerUltrasoundBoneSegmenterModule
::createWidgetRepresentation()
{
  return new qSlicerUltrasoundBoneSegmenterModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerUltrasoundBoneSegmenterModule::createLogic()
{
  return vtkSlicerUltrasoundBoneSegmenterLogic::New();
}
