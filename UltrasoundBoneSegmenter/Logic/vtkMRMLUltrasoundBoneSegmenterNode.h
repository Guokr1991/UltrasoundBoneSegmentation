#ifndef __vtkMRMLUltrasoundBoneSegmenterNode_h
#define __vtkMRMLUltrasoundBoneSegmenterNode_h

#include "vtkSlicerUltrasoundBoneSegmenterModuleLogicExport.h"
#include <vtkMRMLSequenceNode.h>
#include <vtkMRMLScene.h>

class vtkMRMLUltrasoundBoneSegmenterNode;

class VTK_SLICER_ULTRASOUNDBONESEGMENTER_MODULE_LOGIC_EXPORT vtkMRMLUltrasoundBoneSegmenterNode : public vtkMRMLNode
{
public:
  static vtkMRMLUltrasoundBoneSegmenterNode *New();
  vtkTypeMacro(vtkMRMLUltrasoundBoneSegmenterNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  virtual void ReadXMLAttributes(const char** attributes);

  virtual void WriteXML(ostream& of, int indent);

  virtual void Copy(vtkMRMLNode *node);

  virtual const char* GetNodeTagName() { return "UltrasoundBoneSegmenter"; };

  void SetUltrasoundSequenceNodeID(char*);
  char* GetUltrasoundSequenceNodeID();


/*
  vtkSetStringMacro(UltrasoundSequenceNodeID);
  vtkGetStringMacro(UltrasoundSequenceNodeID);

  // Get reference sequence node
  vtkMRMLSequenceNode* GetReferenceSequenceNode();
  // Set and observe reference sequence node
  void SetAndObserveReferenceSequenceNode(vtkMRMLSequenceNode* node);
*/
protected:
  vtkMRMLUltrasoundBoneSegmenterNode();
  ~vtkMRMLUltrasoundBoneSegmenterNode();
  vtkMRMLUltrasoundBoneSegmenterNode(const vtkMRMLUltrasoundBoneSegmenterNode&);
  void operator=(const vtkMRMLUltrasoundBoneSegmenterNode&);

  char* UltrasoundSequenceNodeID;
};

#endif
