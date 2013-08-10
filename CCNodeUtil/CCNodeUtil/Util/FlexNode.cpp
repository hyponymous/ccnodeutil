/*
 *   FILE: FlexNode.cpp
 * AUTHOR: Michael David Plotz
 *   DATE: Tue May 7 14:25:45 PDT 2013
 */

#include "FlexNode.h"

USING_NS_CC;

NS_CC_UTIL_BEGIN

// ========================================================================== //
// static interface                                                           //

FlexNode *FlexNode::create()
{
    FlexNode *obj = new FlexNode();
    if (obj && obj->initFlexNode())
    {
        obj->autorelease();
        return obj;
    }
    CC_SAFE_DELETE(obj);
    return NULL;
}

// ========================================================================== //
// public interface                                                           //

FlexNode::FlexNode()
: m_weight(1.0f)
{
}

FlexNode::~FlexNode()
{
}

bool FlexNode::initFlexNode()
{
    return true;
}

// ========================================================================== //
// private interface                                                          //

NS_CC_UTIL_END

