/*
 *   FILE: LayoutLayer.cpp
 * AUTHOR: Michael David Plotz
 *   DATE: Sun Jul 14 12:41:38 PDT 2013
 */

#include "LayoutLayer.h"

#include "FlexNode.h"

USING_NS_CC;
using namespace std;

NS_CC_UTIL_BEGIN

// ========================================================================== //
// static                                                                     //

LayoutLayer *LayoutLayer::create()
{
    LayoutLayer *obj = new LayoutLayer();
    if (obj && obj->initLayoutLayer())
    {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return NULL;
}

// ========================================================================== //
// public                                                                     //

LayoutLayer::LayoutLayer()
: m_layoutBehavior(NULL)
, m_alignment(ALIGNMENT_CENTER)
, m_minSize(CCSizeZero)
, m_padding(CCSizeZero)
, m_margin(CCSizeZero)
, m_widthSizingPolicy(LAYOUT_SIZING_NONE)
, m_heightSizingPolicy(LAYOUT_SIZING_NONE)
, m_frozen(false)
, m_background(NULL)
, m_color(ccWHITE)
, m_opacity(0xff)
, m_opacityModifyRGB(true)
{
}

LayoutLayer::~LayoutLayer()
{
}

bool LayoutLayer::initLayoutLayer()
{
    if (!CCLayer::init())
    {
        return false;
    }

    this->ignoreAnchorPointForPosition(false);

    this->setAnchorPoint(CCPointZero);
    this->setContentSize(CCSizeZero);

    this->setLayoutBehavior(LAYOUT_NONE);

    return true;
}

CCSize LayoutLayer::getMinSize()
{
    CCSize minSize = m_layoutBehavior->getMinSize(this);
    minSize.width  = fmaxf(m_minSize.width,  minSize.width );
    minSize.height = fmaxf(m_minSize.height, minSize.height);
    return minSize;
}

LayoutLayer *LayoutLayer::setMinSize(const CCSize& value)
{
    m_minSize = value;
    return this;
}

LayoutLayer *LayoutLayer::setLayoutBehavior(LayoutBehaviorType value)
{
    m_layoutBehavior = LayoutBehavior::getLayoutBehavior(value);
    return this;
}

LayoutLayer *LayoutLayer::fitToContents()
{
    return this->setMinSize(CCSizeZero)->doLayout();
}

LayoutLayer *LayoutLayer::fitWidthToContents()
{
    return this->setMinSize(CCSizeMake(0.0f, m_minSize.height))->doLayout();
}

LayoutLayer *LayoutLayer::fitHeightToContents()
{
    return this->setMinSize(CCSizeMake(m_minSize.width, 0.0f))->doLayout();
}

LayoutLayer *LayoutLayer::clear()
{
    // have to make a copy since m_children will be modified in the middle of the loop
    NodeList childrenCopy = m_children;

    NodeList::iterator nodeIter;
    for (nodeIter = childrenCopy.begin();
         nodeIter != childrenCopy.end();
         ++nodeIter)
    {
        if ((*nodeIter) != m_background)
        {
            this->removeChild(*nodeIter);
        }
    }

    return this;
}

LayoutLayer *LayoutLayer::doLayout(CCSize minSize)
{
    if (m_frozen || !this->isVisible())
    {
        return this;
    }

    NodeList::iterator nodeIter;

    // First size and lay out children

    this->fitChildrenSizes(
        m_widthSizingPolicy == LAYOUT_SIZING_EQUAL,
        m_heightSizingPolicy == LAYOUT_SIZING_EQUAL
    );

    CCSize minChildSize = CCSizeZero;
    CCSize largest = this->getSizeOfLargestChild();
    if (m_widthSizingPolicy == LAYOUT_SIZING_EQUAL)
    {
        minChildSize.width = largest.width;
    }
    if (m_heightSizingPolicy == LAYOUT_SIZING_EQUAL)
    {
        minChildSize.height = largest.height;
    }

    for (nodeIter = m_children.begin();
         nodeIter != m_children.end();
         ++nodeIter)
    {
        LayoutLayer *child = dynamic_cast<LayoutLayer *>(*nodeIter);
        if (child)
        {
            child->doLayout(minChildSize);
        }
    }

    // Then lay out self

    CCSize size = this->getMinSize();
    size.width  = fmaxf(minSize.width , size.width );
    size.height = fmaxf(minSize.height, size.height);
    this->setContentSize(size);
    m_layoutBehavior->doLayout(this);

    return this;
}

LayoutLayer *LayoutLayer::setBackground(CCNode *newBackground)
{
    if (newBackground == m_background)
    {
        return this;
    }

    if (m_background)
    {
        CCAssert(m_background->getParent() == this, "old background had a different parent!");
        m_background->removeFromParent();
    }

    m_background = newBackground;
    if (m_background)
    {
        this->addChild(m_background);
        m_background->setAnchorPoint(CCPointZero);
        m_background->setContentSize(this->getContentSize());
    }

    return this;
}

// ========================================================================== //
// inherited from CCNode                                                      //

void LayoutLayer::setContentSize(const CCSize& size)
{
    CCLayer::setContentSize(size);
    if (m_background)
    {
        m_background->setContentSize(size);
    }
}

void LayoutLayer::addChild(CCNode *child)
{
    CCLayer::addChild(child);
}

void LayoutLayer::addChild(CCNode *child, int zOrder)
{
    CCLayer::addChild(child, zOrder);
}

void LayoutLayer::addChild(CCNode *child, int zOrder, int tag)
{
    CCLayer::addChild(child, zOrder, tag);
    m_children.push_back(child);
    FlexNode *flexNode = dynamic_cast<FlexNode *>(child);
    if (flexNode)
    {
        m_flexNodes.push_back(flexNode);
    }
}

void LayoutLayer::removeChild(CCNode *child, bool cleanup)
{
    m_children.remove(child);
    FlexNode *flexNode = dynamic_cast<FlexNode *>(child);
    if (flexNode)
    {
        m_flexNodes.remove(flexNode);
    }
    CCLayer::removeChild(child, cleanup);
}

// ========================================================================== //
// inherited from CCRGBAProtocol                                              //

void LayoutLayer::setColor(const ccColor3B& value)
{
    m_color = value;

    CCRGBAProtocol *colorable = NULL;

    colorable = dynamic_cast<CCRGBAProtocol *>(m_background);
    if (colorable)
    {
        colorable->setColor(m_color);
    }

    for (NodeList::iterator nodeIter = m_children.begin();
         nodeIter != m_children.end();
         ++nodeIter)
    {
        colorable = dynamic_cast<CCRGBAProtocol *>(*nodeIter);
        if (colorable)
        {
            colorable->setColor(m_color);
        }
    }
}

void LayoutLayer::setOpacity(GLubyte value)
{
    m_opacity = value;

    CCRGBAProtocol *colorable = NULL;

    colorable = dynamic_cast<CCRGBAProtocol *>(m_background);
    if (colorable)
    {
        colorable->setOpacity(m_opacity);
    }

    for (NodeList::iterator nodeIter = m_children.begin();
         nodeIter != m_children.end();
         ++nodeIter)
    {
        colorable = dynamic_cast<CCRGBAProtocol *>(*nodeIter);
        if (colorable)
        {
            colorable->setOpacity(m_opacity);
        }
    }
}

void LayoutLayer::setOpacityModifyRGB(bool value)
{
    m_opacityModifyRGB = value;

    CCRGBAProtocol *colorable = NULL;

    colorable = dynamic_cast<CCRGBAProtocol *>(m_background);
    if (colorable)
    {
        colorable->setOpacityModifyRGB(m_opacityModifyRGB);
    }

    for (NodeList::iterator nodeIter = m_children.begin();
         nodeIter != m_children.end();
         ++nodeIter)
    {
        colorable = dynamic_cast<CCRGBAProtocol *>(*nodeIter);
        if (colorable)
        {
            colorable->setOpacityModifyRGB(m_opacityModifyRGB);
        }
    }
}

// ========================================================================== //
// private                                                                    //

void LayoutLayer::fitChildrenSizes(bool fitWidth, bool fitHeight)
{
    if (!fitWidth && !fitHeight)
    {
        return;
    }

    for (NodeList::iterator nodeIter = m_children.begin();
         nodeIter != m_children.end();
         ++nodeIter)
    {
        LayoutLayer *child = dynamic_cast<LayoutLayer *>(*nodeIter);
        if (child && child->isVisible())
        {
            if (fitWidth)
            {
                child->fitWidthToContents();
            }
            if (fitHeight)
            {
                child->fitHeightToContents();
            }
        }
    }
}

CCSize LayoutLayer::getSizeOfLargestChild()
{
    CCSize largest = CCSizeZero;

    for (NodeList::iterator nodeIter = m_children.begin();
         nodeIter != m_children.end();
         ++nodeIter)
    {
        CCNode *child = *nodeIter;
        if (!child->isVisible())
        {
            continue;
        }

        CCSize childSize = child->getContentSize();
        if (childSize.width > largest.width)
        {
            largest.width = childSize.width;
        }
        if (childSize.height > largest.height)
        {
            largest.height = childSize.height;
        }
    }

    return largest;
}

// ========================================================================== //
// LayoutBehavior                                                             //

static map<LayoutBehaviorType, LayoutBehavior *> s_layoutBehaviors;

LayoutBehavior* LayoutBehavior::getLayoutBehavior(LayoutBehaviorType type)
{
    if (s_layoutBehaviors.find(type) == s_layoutBehaviors.end())
    {
        switch (type)
        {
        default:
        case LAYOUT_NONE:
            s_layoutBehaviors[type] = new LayoutBehavior();
            break;
        case LAYOUT_HORIZONTAL:
            s_layoutBehaviors[type] = new HLayoutBehavior();
            break;
        case LAYOUT_VERTICAL:
            s_layoutBehaviors[type] = new VLayoutBehavior();
            break;
        case LAYOUT_OVER:
            s_layoutBehaviors[type] = new OverLayoutBehavior();
            break;
        }
    }

    return s_layoutBehaviors[type];
}

LayoutBehavior::LayoutBehavior()
{
}

LayoutBehavior::~LayoutBehavior()
{
}

// ========================================================================== //
// HLayoutBehavior                                                            //

HLayoutBehavior::HLayoutBehavior()
{
}

HLayoutBehavior::~HLayoutBehavior()
{
}

void HLayoutBehavior::doLayout(LayoutLayer *layer)
{
    NodeList::iterator nodeIter;
    CCNode *node;

    const CCNode *background    = layer->getBackground();
    NodeList& children          = layer->getChildrenList();
    list<FlexNode *>& flexNodes = layer->getFlexNodes();
    const CCSize& padding       = layer->getPadding();
    const CCSize& margin        = layer->getMargin();
    Alignment alignment         = layer->getAlignment();
    CCSize contentSize          = layer->getContentSize();

    float totalNodeWidth = 0.0f;

    bool skipPad = true;

    // add up the visible (non-FlexNode) node widths
    for (nodeIter = children.begin();
         nodeIter != children.end();
         ++nodeIter)
    {
        node = *nodeIter;
        if (node == background ||
            !node->isVisible() ||
            dynamic_cast<FlexNode *>(node))
        {
            continue;
        }

        if (skipPad)
        {
            skipPad = false;
        }
        else
        {
            totalNodeWidth += padding.width;
        }
        totalNodeWidth += getScaledContentSize(node).width;
    }

    totalNodeWidth += 2.0f * margin.width;

    // divide any excess width among the FlexNodes

    list<FlexNode *>::iterator flexNodeIter;
    FlexNode *flexNode;

    // get total flex weight first
    float totalFlexWeight = 0.0f;

    for (flexNodeIter = flexNodes.begin();
         flexNodeIter != flexNodes.end();
         ++flexNodeIter)
    {
        flexNode = *flexNodeIter;
        if (flexNode->isVisible())
        {
            totalFlexWeight += flexNode->getWeight();
        }
    }

    for (flexNodeIter = flexNodes.begin();
         flexNodeIter != flexNodes.end();
         ++flexNodeIter)
    {
        flexNode = *flexNodeIter;
        if (!(flexNode->isVisible()))
        {
            continue;
        }

        CCSize size = flexNode->getContentSize();
        size.width = fmaxf(0.0f, (contentSize.width - totalNodeWidth) * flexNode->getWeight() / totalFlexWeight);
        flexNode->setContentSize(size);
    }

    // Do the positioning

    CCPoint anchor = CCPointZero;
    CCPoint loc = ccp(margin.width, margin.height);
    if (alignment & ALIGNMENT_TOP)
    {
        anchor.y = 1.0f;
        loc.y = contentSize.height - margin.height;
    }
    else if (alignment & ALIGNMENT_CENTER_Y)
    {
        anchor.y = 0.5f;
        loc.y = 0.5f * contentSize.height;
    }

    for (nodeIter = children.begin();
         nodeIter != children.end();
         ++nodeIter)
    {
        node = *nodeIter;
        if (node == background || !(node->isVisible()))
        {
            continue;
        }

        node->setAnchorPoint(anchor);
        node->setPosition(loc);
        loc.x += getScaledContentSize(node).width;
        if (!dynamic_cast<FlexNode *>(node))
        {
            loc.x += padding.width;
        }
    }
}

CCSize HLayoutBehavior::getMinSize(LayoutLayer *layer)
{
    CCSize size = CCSizeZero;

    const CCNode *background    = layer->getBackground();
    NodeList& children          = layer->getChildrenList();
    const CCSize& padding       = layer->getPadding();
    const CCSize& margin        = layer->getMargin();

    // don't add padding for the first visible child
    bool skipPad = true;

    NodeList::iterator nodeIter;
    for (nodeIter = children.begin();
         nodeIter != children.end();
         ++nodeIter)
    {
        CCNode *node = *nodeIter;
        if (node == background ||
            !node->isVisible() ||
            dynamic_cast<FlexNode *>(node))
        {
            continue;
        }

        CCSize nodeSize = getScaledContentSize(node);

        if (skipPad)
        {
            skipPad = false;
        }
        else
        {
            size.width += padding.width;
        }
        size.width += nodeSize.width;

        if (nodeSize.height > size.height)
        {
            size.height = nodeSize.height;
        }
    }

    size.width  += 2.0f * margin.width;
    size.height += 2.0f * margin.height;

    return size;
}

// ========================================================================== //
// VLayoutBehavior                                                            //

VLayoutBehavior::VLayoutBehavior()
{
}

VLayoutBehavior::~VLayoutBehavior()
{
}

void VLayoutBehavior::doLayout(LayoutLayer *layer)
{
    NodeList::iterator nodeIter;
    CCNode *node;

    const CCNode *background    = layer->getBackground();
    NodeList& children          = layer->getChildrenList();
    list<FlexNode *>& flexNodes = layer->getFlexNodes();
    const CCSize& padding       = layer->getPadding();
    const CCSize& margin        = layer->getMargin();
    Alignment alignment         = layer->getAlignment();
    CCSize contentSize          = layer->getContentSize();

    float totalNodeHeight = 0.0f;

    bool skipPad = true;

    // add up the visible (non-FlexNode) node widths
    for (nodeIter = children.begin();
         nodeIter != children.end();
         ++nodeIter)
    {
        node = *nodeIter;
        if (node == background ||
            !node->isVisible() ||
            dynamic_cast<FlexNode *>(node))
        {
            continue;
        }

        if (skipPad)
        {
            skipPad = false;
        }
        else
        {
            totalNodeHeight += padding.height;
        }
        totalNodeHeight += getScaledContentSize(node).height;
    }

    totalNodeHeight += 2.0f * margin.height;

    // divide any excess width among the FlexNodes

    list<FlexNode *>::iterator flexNodeIter;
    FlexNode *flexNode;

    // get total flex weight first
    float totalFlexWeight = 0.0f;

    for (flexNodeIter = flexNodes.begin();
         flexNodeIter != flexNodes.end();
         ++flexNodeIter)
    {
        flexNode = *flexNodeIter;
        if (flexNode->isVisible())
        {
            totalFlexWeight += flexNode->getWeight();
        }
    }

    for (flexNodeIter = flexNodes.begin();
         flexNodeIter != flexNodes.end();
         ++flexNodeIter)
    {
        flexNode = *flexNodeIter;
        if (!(flexNode->isVisible()))
        {
            continue;
        }

        CCSize size = flexNode->getContentSize();
        size.height = fmaxf(0.0f, (contentSize.height - totalNodeHeight) * flexNode->getWeight() / totalFlexWeight);
        flexNode->setContentSize(size);
    }

    // Do the positioning

    CCPoint anchor = ccp(0.0f, 1.0f);
    CCPoint loc = ccp(margin.width, contentSize.height - margin.height);
    if (alignment & ALIGNMENT_RIGHT)
    {
        anchor.x = 1.0f;
        loc.x = contentSize.width - margin.width;
    }
    else if (alignment & ALIGNMENT_CENTER_X)
    {
        anchor.x = 0.5f;
        loc.x = 0.5f * contentSize.width;
    }

    for (nodeIter = children.begin();
         nodeIter != children.end();
         ++nodeIter)
    {
        node = *nodeIter;
        if (node == background || !(node->isVisible()))
        {
            continue;
        }

        node->setAnchorPoint(anchor);
        node->setPosition(loc);
        loc.y -= getScaledContentSize(node).height;
        if (!dynamic_cast<FlexNode *>(node))
        {
            loc.y -= padding.height;
        }
    }
}

CCSize VLayoutBehavior::getMinSize(LayoutLayer *layer)
{
    CCSize size = CCSizeZero;

    const CCNode *background    = layer->getBackground();
    NodeList& children          = layer->getChildrenList();
    const CCSize& padding       = layer->getPadding();
    const CCSize& margin        = layer->getMargin();

    // don't add padding for the first visible child
    bool skipPad = true;

    NodeList::iterator nodeIter;
    for (nodeIter = children.begin();
         nodeIter != children.end();
         ++nodeIter)
    {
        CCNode *node = *nodeIter;
        if (node == background ||
            !node->isVisible() ||
            dynamic_cast<FlexNode *>(node))
        {
            continue;
        }

        CCSize nodeSize = getScaledContentSize(node);

        if (skipPad)
        {
            skipPad = false;
        }
        else
        {
            size.height += padding.height;
        }
        size.height += nodeSize.height;

        if (nodeSize.width > size.width)
        {
            size.width = nodeSize.width;
        }
    }

    size.width  += 2.0f * margin.width;
    size.height += 2.0f * margin.height;

    return size;
}

// ========================================================================== //
// OverLayoutBehavior                                                            //

OverLayoutBehavior::OverLayoutBehavior()
{
}

OverLayoutBehavior::~OverLayoutBehavior()
{
}

void OverLayoutBehavior::doLayout(LayoutLayer *layer)
{
    NodeList::iterator nodeIter;
    CCNode *node;

    const CCNode *background    = layer->getBackground();
    NodeList& children          = layer->getChildrenList();
    const CCSize& margin        = layer->getMargin();
    Alignment alignment         = layer->getAlignment();
    CCSize contentSize          = layer->getContentSize();

    // All child nodes will have the same location, based on alignment
    CCPoint anchor = ANCHOR_CC;
    CCPoint loc = centerOf(contentSize);

    if (alignment & ALIGNMENT_LEFT)
    {
        anchor.x = 0.0f;
        loc.x = margin.width;
    }
    else if (alignment & ALIGNMENT_RIGHT)
    {
        anchor.x = 1.0f;
        loc.x = contentSize.width - margin.width;
    }

    if (alignment & ALIGNMENT_BOTTOM)
    {
        anchor.y = 0.0f;
        loc.y = margin.height;
    }
    else if (alignment & ALIGNMENT_TOP)
    {
        anchor.y = 1.0f;
        loc.y = contentSize.height - margin.height;
    }

    for (nodeIter = children.begin();
         nodeIter != children.end();
         ++nodeIter)
    {
        node = *nodeIter;
        if (node == background || !(node->isVisible()))
        {
            continue;
        }

        node->setAnchorPoint(anchor);
        node->setPosition(loc);
    }
}

CCSize OverLayoutBehavior::getMinSize(LayoutLayer *layer)
{
    CCSize size = CCSizeZero;

    const CCNode *background    = layer->getBackground();
    NodeList& children          = layer->getChildrenList();
    const CCSize& margin        = layer->getMargin();

    // just find the largest dimensions

    NodeList::iterator nodeIter;
    for (nodeIter = children.begin();
         nodeIter != children.end();
         ++nodeIter)
    {
        CCNode *node = *nodeIter;
        if (node == background ||
            !node->isVisible() ||
            dynamic_cast<FlexNode *>(node))
        {
            continue;
        }

        CCSize nodeSize = getScaledContentSize(node);

        if (nodeSize.width > size.width)
        {
            size.width = nodeSize.width;
        }
        if (nodeSize.height > size.height)
        {
            size.height = nodeSize.height;
        }
    }

    size.width  += 2.0f * margin.width;
    size.height += 2.0f * margin.height;

    return size;
}

NS_CC_UTIL_END

