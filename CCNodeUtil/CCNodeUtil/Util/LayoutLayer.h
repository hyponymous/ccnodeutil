/*
 *   FILE: LayoutLayer.h
 * AUTHOR: Michael David Plotz
 *   DATE: Sun Jul 14 12:41:38 PDT 2013
 */

#ifndef __nback_LAYOUTLAYER_H__
#define __nback_LAYOUTLAYER_H__

#include "cocos2d.h"
#include "CCNodeUtil.h"

#define hll() (LayoutLayer::create()->setLayoutBehavior(LAYOUT_HORIZONTAL))
#define vll() (LayoutLayer::create()->setLayoutBehavior(LAYOUT_VERTICAL))
#define oll() (LayoutLayer::create()->setLayoutBehavior(LAYOUT_OVER))

USING_NS_CC;
using namespace std;

NS_CC_UTIL_BEGIN

enum LayoutBehaviorType
{
    LAYOUT_NONE,
    LAYOUT_HORIZONTAL,
    LAYOUT_VERTICAL,
    LAYOUT_OVER,
};

enum LayoutSizingPolicy
{
    LAYOUT_SIZING_NONE,
    LAYOUT_SIZING_EQUAL,
};

class FlexNode;
class HLayoutBehavior;
class LayoutBehavior;
class OverLayoutBehavior;
class VLayoutBehavior;

class LayoutLayer : public CCLayer, public CCRGBAProtocol
{
public:
    static LayoutLayer *create();

    LayoutLayer();
    virtual ~LayoutLayer();

    bool initLayoutLayer();

    Alignment getAlignment() const { return m_alignment; }
    LayoutLayer *setAlignment(Alignment value) { m_alignment = value; return this; }

    CCSize getMinSize();
    LayoutLayer *setMinSize(const CCSize& value);

    const CCSize& getPadding() const { return m_padding; }
    LayoutLayer *setPadding(const CCSize& value) { m_padding = value; return this; }

    const CCSize& getMargin() const { return m_margin; }
    LayoutLayer *setMargin(const CCSize& value) { m_margin = value; return this; }

    LayoutLayer *setLayoutBehavior(LayoutBehaviorType value);
    LayoutLayer *setWidthSizingPolicy(LayoutSizingPolicy value) { m_widthSizingPolicy = value; return this; }
    LayoutLayer *setHeightSizingPolicy(LayoutSizingPolicy value) { m_heightSizingPolicy = value; return this; }

    LayoutLayer *setFrozen(bool value) { m_frozen = value; return this; }
    bool getFrozen() { return m_frozen; }

    LayoutLayer *fitToContents();
    LayoutLayer *fitWidthToContents();
    LayoutLayer *fitHeightToContents();
    LayoutLayer *clear();

    LayoutLayer *doLayout(CCSize minSize = CCSizeZero);

    const CCNode *getBackground() const { return m_background; }
    LayoutLayer *setBackground(CCNode *newBackground);
    NodeList& getChildrenList() { return m_children; }
    list<FlexNode *>& getFlexNodes() { return m_flexNodes; }

    // ========================================================================== //
    // inherited from CCNode                                                      //

    virtual void setContentSize(const CCSize& size);
    virtual void addChild(CCNode *child);
    virtual void addChild(CCNode *child, int zOrder);
    virtual void addChild(CCNode *child, int zOrder, int tag);
    virtual void removeChild(CCNode *child, bool cleanup);
    virtual void removeChild(CCNode *child) { this->removeChild(child, true); }

    // ========================================================================== //
    // inherited from CCRGBAProtocol                                              //

    virtual void setColor(const ccColor3B& value);
    virtual const ccColor3B& getColor() { return m_color; }
    virtual GLubyte getOpacity(void) { return m_opacity; }
    virtual void setOpacity(GLubyte value);
    virtual void setOpacityModifyRGB(bool value);
    virtual bool isOpacityModifyRGB(void) { return m_opacityModifyRGB; }

protected:
private:
    LayoutLayer(const LayoutLayer& orig);
    LayoutLayer& operator=(const LayoutLayer& rhs);

    void fitChildrenSizes(bool fitWidth, bool fitHeight);
    CCSize getSizeOfLargestChild();

    LayoutBehavior *m_layoutBehavior;

    Alignment m_alignment;
    CCSize m_minSize;
    CCSize m_padding;
    CCSize m_margin;
    LayoutSizingPolicy m_widthSizingPolicy;
    LayoutSizingPolicy m_heightSizingPolicy;

    bool m_frozen;

    CCNode *m_background;
    NodeList m_children;
    list<FlexNode *> m_flexNodes;

    ccColor3B m_color;
    GLubyte m_opacity;
    bool m_opacityModifyRGB;

};

class LayoutBehavior
{
public:
    static LayoutBehavior* getLayoutBehavior(LayoutBehaviorType type);

    LayoutBehavior();
    virtual ~LayoutBehavior();

    virtual void doLayout(LayoutLayer *layer) { }
    virtual CCSize getMinSize(LayoutLayer *layer) { return CCSizeZero; }

protected:
private:
    LayoutBehavior(const LayoutBehavior& orig);
    LayoutBehavior& operator=(const LayoutBehavior& rhs);
};

class HLayoutBehavior : public LayoutBehavior
{
public:
    HLayoutBehavior();
    virtual ~HLayoutBehavior();

    virtual void doLayout(LayoutLayer *layer);
    virtual CCSize getMinSize(LayoutLayer *layer);

protected:
private:
    HLayoutBehavior(const HLayoutBehavior& orig);
    HLayoutBehavior& operator=(const HLayoutBehavior& rhs);
};

class VLayoutBehavior : public LayoutBehavior
{
public:
    VLayoutBehavior();
    virtual ~VLayoutBehavior();

    virtual void doLayout(LayoutLayer *layer);
    virtual CCSize getMinSize(LayoutLayer *layer);

protected:
private:
    VLayoutBehavior(const VLayoutBehavior& orig);
    VLayoutBehavior& operator=(const VLayoutBehavior& rhs);
};

class OverLayoutBehavior : public LayoutBehavior
{
public:
    OverLayoutBehavior();
    virtual ~OverLayoutBehavior();

    virtual void doLayout(LayoutLayer *layer);
    virtual CCSize getMinSize(LayoutLayer *layer);

protected:
private:
    OverLayoutBehavior(const OverLayoutBehavior& orig);
    OverLayoutBehavior& operator=(const OverLayoutBehavior& rhs);
};

NS_CC_UTIL_END

#endif /* __nback_LAYOUTLAYER_H__ */

