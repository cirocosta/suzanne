#include "yabrowser/Layout.hh"

namespace yabrowser { namespace layout {

using namespace style;
using namespace yacss;

LayoutBox::LayoutBox (const StyledNode& sn, const BoxType bt)
  : type(bt), dimensions(Dimensions()), children(LayoutBoxContainer())
{
  const KeywordValue* display;

  // recursion
  for (const auto& child : sn.children) {

    switch (child->display) {
      case DISPLAY_INLINE:
        children.push_back(
            std::make_shared<LayoutBox>(*child, BoxType::InlineNode)
        );
        break;

      case DISPLAY_BLOCK:
        children.push_back(
            std::make_shared<LayoutBox>(*child, BoxType::BlockNode)
        );
        break;

      case DISPLAY_NONE:
        continue;
    }
  }
}

LayoutBox::~LayoutBox ()
{
}

}}; // ! ns yabrowser layout

