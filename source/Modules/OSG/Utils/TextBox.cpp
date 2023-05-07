#include "TextBox.h"
#include "Modules/OSG/OSGNodeMasks.h"

TextBox::TextBox(): matrixTransform(new osg::MatrixTransform()),
    projectionMatrix(new osg::Projection),
    textGeode(new osg::Geode()),
    text(new osgText::Text())
{
  // Matrix Transform - We use a matrix transform to nullify any 
  // transforms that may be applied to parents, which would then 
  // apply to this text box.
  // Now we set this matrix transform's reference frame to be absolute, 
  // meaning any transformations performed on parent nodes will 
  // not be performed on this projection.
  matrixTransform->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    
  //Add the projection matrix to this transform
  matrixTransform->addChild(projectionMatrix);
    
// Projection Matrix - We use a projection matrix because we need to 
// create a new "layer" upon which to draw the text. We create a
// 2D orthographic projection.
// Set our projection matrix to an orthographic 2D matrix with the
// dimensions of (x_min, x_max, y_min, y_max) resolution.
        
// osg::Matrix::ortho2D() is a static function that returns
// a matrix with the properties described above.
    projectionMatrix->setMatrix(osg::Matrix::ortho2D(0,640,0,480));
    
    // Now add the text geometry to this new projection matrix surface
    projectionMatrix->addChild(textGeode);

	textGeode->setNodeMask(~GASS::NM_RECEIVE_SHADOWS & textGeode->getNodeMask());
	textGeode->setNodeMask(~GASS::NM_CAST_SHADOWS & textGeode->getNodeMask());

	osg::ref_ptr<osg::StateSet> nodess = textGeode->getOrCreateStateSet();

#ifdef OSG_GL_FIXED_FUNCTION_AVAILABLE
    nodess->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
#endif


	osg::Program* program = new osg::Program;
	nodess->setAttribute(program);



// Geode - Since osgText::Text is a derived class from drawable, we 
// must add it to an osg::Geode before we can add it to our ScenGraph.
    textGeode->addDrawable(text);
    
    //Set the screen alignment - always face the screen
    text->setAxisAlignment(osgText::Text::SCREEN);
    
    //Set the text to our default text string
    text->setText("Default Text");
	text->setBackdropType(osgText::Text::OUTLINE);


	
}

void TextBox::setText(const string& t)
{
    text->setText(t);
}

void TextBox::setFont(const string& font)
{
    text->setFont(font);
}

void TextBox::setColor(osg::Vec4d color)
{
    text->setColor(color);
}

void TextBox::setPosition(osg::Vec3d position)
{
    text->setPosition(position);
}

void TextBox::setTextSize(float size)
{
    text->setCharacterSize(size);
}

osg::ref_ptr <osg::Group> TextBox::getGroup() const
{
    return matrixTransform;
}

string TextBox::getText() const
{
    return text->getText().createUTF8EncodedString();
}
