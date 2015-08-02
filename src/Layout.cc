#include "yabrowser/Layout.hh"

namespace yabrowser { namespace layout {

using namespace style;
using namespace yacss;

LayoutBox::LayoutBox (const StyledNode& sn, const BoxType bt)
  : type(bt), dimensions(Dimensions()), children(LayoutBoxContainer())
{
  if (type == BoxType::AnonymousBlock)
    return;

  bool has_block = false;
  bool constructed_anon = false;
  LayoutBoxContainer::const_iterator last_anon_it;

  for (const auto& child : sn.children) {
    if (child->display == DISPLAY_BLOCK) {
      has_block = true;
      break;
    }
  }

  for (const auto& child : sn.children) {
    switch (child->display) {
      case DISPLAY_INLINE:
      if (!has_block) {
        children.push_back(
            std::make_shared<LayoutBox>(*child, BoxType::InlineNode)
        );

        continue;
      }

      if (!constructed_anon) {
        children.push_back(
            std::make_shared<LayoutBox>(*child, BoxType::AnonymousBlock)
        );
        last_anon_it = children.end()-1;
        constructed_anon = true;
      }

      (*last_anon_it)->children.push_back(
          std::make_shared<LayoutBox>(*child, BoxType::InlineNode)
      );
      break;

      case DISPLAY_BLOCK:
      children.push_back(
          std::make_shared<LayoutBox>(*child, BoxType::BlockNode)
      );
      constructed_anon = false;
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

