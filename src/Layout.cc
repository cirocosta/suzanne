#include "yabrowser/Layout.hh"

namespace yabrowser { namespace layout {

using namespace style;
using namespace yacss;

LayoutBox::LayoutBox (const StyledChild& sn, const BoxType bt)
  : type(bt), dimensions(Dimensions()), children(LayoutBoxContainer())
{
  if (type == BoxType::AnonymousBlock)
    return;

  bool has_block = false;
  bool constructed_anon = false;
  LayoutBoxContainer::const_iterator last_anon_it;

  for (const auto& child : sn->children) {
    if (child->display == DISPLAY_BLOCK) {
      has_block = true;
      break;
    }
  }

  for (const auto& child : sn->children) {
    switch (child->display) {
      case DISPLAY_INLINE:
      if (!has_block) {
        children.push_back(
            std::make_shared<LayoutBox>(child, BoxType::InlineNode)
        );

        continue;
      }

      if (!constructed_anon) {
        children.push_back(
            std::make_shared<LayoutBox>(child, BoxType::AnonymousBlock)
        );
        last_anon_it = children.end()-1;
        constructed_anon = true;
      }

      (*last_anon_it)->children.push_back(
          std::make_shared<LayoutBox>(child, BoxType::InlineNode)
      );
      break;

      case DISPLAY_BLOCK:
      children.push_back(
          std::make_shared<LayoutBox>(child, BoxType::BlockNode)
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

void LayoutBox::calculate_block_width (const LayoutBox& parent)
{
  const CSSBaseValue zero_length = LengthValue(0, "px");
  const CSSBaseValue auto_keyword = KeywordValue("auto");

  const CSSBaseValue& width =
    parent.styled_node->decl_lookup({"width"}, auto_keyword);

  const CSSBaseValue& margin_left =
    parent.styled_node->decl_lookup({"margin-left", "margin"}, zero_length);
  const CSSBaseValue& margin_right =
    parent.styled_node->decl_lookup({"margin-right", "margin"}, zero_length);

  const CSSBaseValue& border_left =
    parent.styled_node->decl_lookup({"border-left", "border"}, zero_length);
  const CSSBaseValue& border_right =
    parent.styled_node->decl_lookup({"border-right", "border"}, zero_length);

  const CSSBaseValue& padding_left =
    parent.styled_node->decl_lookup({"padding-left", "padding"}, zero_length);
  const CSSBaseValue& padding_right =
    parent.styled_node->decl_lookup({"padding-right", "padding"}, zero_length);

  unsigned total_width =
    to_px(width) + to_px(margin_left) + to_px(margin_right) +
    to_px(border_left) + to_px(border_right) +
    to_px(padding_left) + to_px(border_right);

  // TODO continue here after improving CSSBaseValue comparison
  //      and decl_lookup as a DeclarationContainer method
  if (width.type != ValueType::Keyword && parent.dimensions.content.width) {
  }

}

}}; // ! ns yabrowser layout

