// ####################################################################
//  File:       textBox.h
//  Purpose:    This class is a wrapper around OSG's text functionality.
//              It may be expanded to encompass more of osgText's options;
//              it currently demos the display of text in a SceneGraph.
// ####################################################################

#ifndef TEXT_BOX_H
#define TEXT_BOX_H
#include <string>
using std::string;



#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning(disable:4100)
#endif
#include <osgText/Text>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/MatrixTransform>

// ####################################################################

class TextBox
{
public:

  TextBox();
    
  ~TextBox() {}
  void setText(const string& text);
  void setFont(const string& font);

  //  Sets the text based on an osg::Vec4d color vector
  //  Properties:
  //      (red, green, blue, alpha)
  //  values range from 0.0 to 1.0: 0.0 is off, and 1.0 is complete.
  void setColor(osg::Vec4d color);

  //  Sets the text position as an offset from the lower left corner 
  //  of the wcreen; thus, any positive values for the osg::Vec3d 
  //  will offset the text to the right and/or up.
  void setPosition(osg::Vec3d position);
  void setTextSize(unsigned int size);
    
  osg::ref_ptr <osg::Group> getGroup() const;
    
  string getText() const;
    
private:

  //  Provides a buffer between any parent transformations and our 
  //  new matrix projection, ensures that we're working in our own 
  //  coordinate space.
  osg::ref_ptr<osg::MatrixTransform> matrixTransform;
    
  //  Projection that provides a surface on which to paint the text.
  osg::ref_ptr <osg::Projection>  projectionMatrix;
    
  //  The Geode textGeode is used to hold our text. 
  //  osgText is a drawable, so osg::Geode is
  //  needed to add the text to the SceneGraph
  osg::ref_ptr <osg::Geode>  textGeode;
    
  //  The actual osgText object that holds textual data
  osg::ref_ptr <osgText::Text> text;
};

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

#endif
