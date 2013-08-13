CCNodeUtil
==========

Layout and positioning utilities for [Cocos2d-x](http://www.cocos2d-x.org/ "Cocos2d-x").

# Example

See also the bottom of `HelloWorld::init()`. This example builds a table of 8 rows, each of which has an icon, an invisible `FlexNode`, and a label. Because of the width sizing policy `LAYOUT_SIZING_EQUAL`, the labels are positioned with their right edges aligned. Please note that `LayoutLayer`s will not lay themselves out unless you call `doLayout()` or `fitToContents()` explicitly.

```cpp
    CCSize padding = CCSizeMake(20, 12);

    LayoutLayer *outer = vll()
        ->setWidthSizingPolicy(LAYOUT_SIZING_EQUAL)
        ->setPadding(padding);

    for (int i = 0; i < 8; ++i)
    {
        LayoutLayer *row = hll()->setPadding(padding);

        row->addChild(CCSprite::create("Icon.png"));
        row->addChild(FlexNode::create());

        CCLabelTTF *label = CCLabelTTF::create(
            toString(2345 * i).c_str(),
            "Thonburi",
            34
        );
        row->addChild(label);

        outer->addChild(row);
    }

    outer->fitToContents();
    this->addChild(outer);
    alignNodeToParent(outer, ALIGNMENT_CENTER);
```

# Screenshots

![LayoutLayer in Action](http://hyponymo.us/wordpress/wp-content/uploads/2013/08/LayoutLayer_HelloWorld.png)
