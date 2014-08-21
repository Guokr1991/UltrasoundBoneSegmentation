/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerUltrasoundBoneSegmenterFooBarWidget.h"
#include "ui_qSlicerUltrasoundBoneSegmenterFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_UltrasoundBoneSegmenter
class qSlicerUltrasoundBoneSegmenterFooBarWidgetPrivate
  : public Ui_qSlicerUltrasoundBoneSegmenterFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerUltrasoundBoneSegmenterFooBarWidget);
protected:
  qSlicerUltrasoundBoneSegmenterFooBarWidget* const q_ptr;

public:
  qSlicerUltrasoundBoneSegmenterFooBarWidgetPrivate(
    qSlicerUltrasoundBoneSegmenterFooBarWidget& object);
  virtual void setupUi(qSlicerUltrasoundBoneSegmenterFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterFooBarWidgetPrivate
::qSlicerUltrasoundBoneSegmenterFooBarWidgetPrivate(
  qSlicerUltrasoundBoneSegmenterFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerUltrasoundBoneSegmenterFooBarWidgetPrivate
::setupUi(qSlicerUltrasoundBoneSegmenterFooBarWidget* widget)
{
  this->Ui_qSlicerUltrasoundBoneSegmenterFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerUltrasoundBoneSegmenterFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterFooBarWidget
::qSlicerUltrasoundBoneSegmenterFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerUltrasoundBoneSegmenterFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerUltrasoundBoneSegmenterFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerUltrasoundBoneSegmenterFooBarWidget
::~qSlicerUltrasoundBoneSegmenterFooBarWidget()
{
}
