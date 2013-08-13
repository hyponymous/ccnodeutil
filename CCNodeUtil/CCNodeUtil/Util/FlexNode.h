/*
 *   FILE: FlexNode.h
 * AUTHOR: Michael David Plotz
 *   DATE: Tue May 7 14:25:45 PDT 2013
 */

#ifndef __CCNODEUTIL_FLEXNODE_H__
#define __CCNODEUTIL_FLEXNODE_H__

#include "cocos2d.h"
#include "CCNodeUtil.h"

USING_NS_CC;

NS_CC_UTIL_BEGIN

class FlexNode : public CCNode
{
public:
    static FlexNode *create();

    FlexNode();
    virtual ~FlexNode();

    bool initFlexNode();

    float getWeight() { return m_weight; }
    FlexNode *setWeight(float value)
    {
        if (value > 0)
        {
            m_weight = value;
        }
        return this;
    }

protected:
private:
    FlexNode(const FlexNode& orig);
    FlexNode& operator=(const FlexNode& rhs);

    float m_weight;
};

NS_CC_UTIL_END

#endif /* __CCNODEUTIL_FLEXNODE_H__ */

