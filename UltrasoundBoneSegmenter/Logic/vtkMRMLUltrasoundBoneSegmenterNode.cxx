#include <vtkMRMLUltrasoundBoneSegmenterNode.h>

#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <sstream>

//------------------------------------------------------------------------------
static const char* ULTRASOUND_SEQUENCE_REFERENCE_ROLE = "ultrasoundSequenceRef";

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLUltrasoundBoneSegmenterNode);

//------------------------------------------------------------------------------
vtkMRMLUltrasoundBoneSegmenterNode::vtkMRMLUltrasoundBoneSegmenterNode()
{

}

//------------------------------------------------------------------------------
vtkMRMLUltrasoundBoneSegmenterNode::~vtkMRMLUltrasoundBoneSegmenterNode()
{
}

//------------------------------------------------------------------------------
void vtkMRMLUltrasoundBoneSegmenterNode::PrintSelf(ostream& os, vtkIndent indent)
{

}

//------------------------------------------------------------------------------
void vtkMRMLUltrasoundBoneSegmenterNode::WriteXML(ostream& of, int nIndent)
{
}

//------------------------------------------------------------------------------
void vtkMRMLUltrasoundBoneSegmenterNode::ReadXMLAttributes(const char** attributes)
{
}

void vtkMRMLUltrasoundBoneSegmenterNode::Copy(vtkMRMLNode *node)
{
}

char* vtkMRMLUltrasoundBoneSegmenterNode::GetUltrasoundSequenceNodeID()
{
  return this->UltrasoundSequenceNodeID;
}

void vtkMRMLUltrasoundBoneSegmenterNode::SetUltrasoundSequenceNodeID(char* newID)
{
  this->UltrasoundSequenceNodeID = newID;
}



/*
vtkMRMLSequenceNode* vtkMRMLUltrasoundBoneSegmenterNode::GetReferenceSequenceNode()
{
  return vtkMRMLSequenceNode::SafeDownCast(this->GetNodeReference(ULTRASOUND_SEQUENCE_REFERENCE_ROLE));
}

void vtkMRMLUltrasoundBoneSegmenterNode::SetAndObserveReferenceSequenceNode(vtkMRMLSequenceNode* node)
{
  std::cout << "In setandobserve" << std::endl;
  std::cout << "Node ID: " << node->GetID() << std::endl;
  this->DisableModifiedEventOn();
  this->SetNodeReferenceID(ULTRASOUND_SEQUENCE_REFERENCE_ROLE, (node ? node->GetID() : NULL));
  this->DisableModifiedEventOff();
}
*/
