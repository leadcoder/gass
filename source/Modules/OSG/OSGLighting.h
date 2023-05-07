#pragma once

#include "Modules/OSG/OSGCommon.h"
#include <osgEarth/Common>
#include <osgEarth/Threading>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/LightSource>
#include <osg/Light>
#include <osg/Material>
#include <set>

// Use this with StateSet::setDefine to toggle lighting in osgEarth lighting shaders.
#define OE_LIGHTING_DEFINE "OE_LIGHTING"

namespace GASS
{
    struct  Lighting
    {
        //! Sets the lighting mode on a stateset
        static void set(osg::StateSet* stateSet, osg::StateAttribute::OverrideValue value);

        //! Removed the lighting mode from a stateset
        static void remove(osg::StateSet* stateSet);

        //! Installs a default material on a stateset
        static void installDefaultMaterial(osg::StateSet* stateSet);
    };

    /**
     * Traverses a graph, looking for Lights and Materials, and generates either static
     * uniforms or dynamic cull callbacks for them so they will work with core profile
     * shaders. (This is necessary for GL3+ core, OSX, Mesa etc. that don't support
     * compatibility mode.)
     */
    class ExportOSG GenerateGL3LightingUniforms : public osg::NodeVisitor
    {
    public:
        GenerateGL3LightingUniforms();

    public: // osg::NodeVisitor
        virtual void apply(osg::Node& node);
        virtual void apply(osg::LightSource& node);

    protected:
        std::set<osg::StateSet*> _statesets;

        template<typename T> bool alreadyInstalled(osg::Callback* cb) const {
            return !cb ? false : dynamic_cast<T*>(cb)!=0L ? true : alreadyInstalled<T>(cb->getNestedCallback());
        }
    };

    /**
     * Material that will work in both FFP and non-FFP mode, by using the uniform
     * osg_FrontMaterial in place of gl_FrontMaterial.
     */
    class ExportOSG MaterialGL3 : public osg::Material
    {
    public:
        MaterialGL3() : osg::Material() { }
        MaterialGL3(const MaterialGL3& mat, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) : osg::Material(mat, copyop) {}
        MaterialGL3(const osg::Material& mat) : osg::Material(mat) { }

        META_StateAttribute(GASS, MaterialGL3, MATERIAL);

        virtual void apply(osg::State&) const;
    };

    /**
     * StateAttributeCallback that will update osg::Material properties as Uniforms
     */
    class ExportOSG MaterialCallback : public osg::StateAttributeCallback
    {
    public:
        virtual void operator() (osg::StateAttribute* attr, osg::NodeVisitor* nv);        
    };

    /**
     * Light that will work in both FFP and non-FFP mode.
     * To use this, find a LightSource and replace the Light with a LightGL3.
     * Then install the LightSourceGL3UniformGenerator cull callback on the LightSource.
     */
    class ExportOSG LightGL3 : public osg::Light
    {
    public:
        LightGL3() : osg::Light(), _enabled(true) { }
        LightGL3(int lightnum) : osg::Light(lightnum), _enabled(true) { }
        LightGL3(const LightGL3& light, const osg::CopyOp& copyop =osg::CopyOp::SHALLOW_COPY)
            : osg::Light(light, copyop), _enabled(light._enabled) {}
        LightGL3(const osg::Light& light)
            : osg::Light(light), _enabled(true) { }

        void setEnabled(bool value) { _enabled = value; }
        bool getEnabled() const { return _enabled; }

        virtual osg::Object* cloneType() const { return new LightGL3(_lightnum); }
        virtual osg::Object* clone(const osg::CopyOp& copyop) const { return new LightGL3(*this,copyop); }
        virtual bool isSameKindAs(const osg::Object* obj) const { return dynamic_cast<const LightGL3 *>(obj)!=NULL; }
        virtual const char* libraryName() const { return "osgEarth"; }
        virtual const char* className() const { return "LightGL3"; }
        virtual Type getType() const { return LIGHT; }

        virtual void apply(osg::State&) const;

    protected:
        bool _enabled;
    };

    /**
     * Cull callback that will install Light uniforms based on the Light in a LightSource.
     * Install this directly on the LightSource node. 
     */
    class ExportOSG LightSourceGL3UniformGenerator : public osg::NodeCallback
    {
    public:
        LightSourceGL3UniformGenerator();

        /**
         * Creates and installs Uniforms on the stateset for the Light components
         * of the Light that are non-positional (everything but the position and direction)
         */
        void generateNonPositionalData(osg::StateSet* ss, osg::Light* light);

    public: // osg::NodeCallback

        bool run(osg::Object* obj, osg::Object* data);

    public: // osg::Object

        void resizeGLBufferObjects(unsigned maxSize);
        void releaseGLObjects(osg::State* state) const;

        mutable std::vector<osg::ref_ptr<osg::StateSet> > _statesets;
        mutable std::mutex _statesetsMutex;
    };
}


