#include "yabrowser/Layout.hh"

namespace yabrowser
{
namespace layout
{

inline float to_px(const yacss::CSSBaseValue& value)
{
  if (value.type == yacss::ValueType::Length) {
    return value.get<yacss::LengthValue>().val;
  }

  return 0.0;
}

using namespace style;
using namespace yacss;

LayoutBox::LayoutBox(const StyledChild& sn, const BoxType bt)
    : type(bt), dimensions(Dimensions()), children(LayoutBoxContainer())
{
  if (type == BoxType::AnonymousBlock)
    return;

  bool has_block = false;
  bool constructed_anon = false;
  LayoutBoxContainer::const_iterator last_anon_it;
  styled_node = sn;

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
              std::make_shared<LayoutBox>(child, BoxType::InlineNode));

          continue;
        }

        if (!constructed_anon) {
          children.push_back(
              std::make_shared<LayoutBox>(child, BoxType::AnonymousBlock));
          last_anon_it = children.end() - 1;
          constructed_anon = true;
        }

        (*last_anon_it)
            ->children.push_back(
                std::make_shared<LayoutBox>(child, BoxType::InlineNode));
        break;

      case DISPLAY_BLOCK:
        children.push_back(
            std::make_shared<LayoutBox>(child, BoxType::BlockNode));
        constructed_anon = false;
        break;

      case DISPLAY_NONE:
        continue;
    }
  }
}

LayoutBox::~LayoutBox() {}

void LayoutBox::calculate(const Dimensions& containing_block)
{
  if (this->type == BoxType::BlockNode) {
    calculate_block_layout(containing_block);
  } else if (this->type == BoxType::InlineNode) {
    throw std::runtime_error("LayoutBox::calculate - inlinenode unsupported");
  } else if (this->type == BoxType::AnonymousBlock) {
    throw std::runtime_error("LayoutBox::calculate - anonblock unsupported");
  } else {
    throw std::runtime_error("LayoutBox::calculate - unsupported");
  }
}

void LayoutBox::calculate_block_layout(const Dimensions& containing_block)
{
  calculate_block_width(containing_block);
  /* calculate_block_position(containing_block); */
  /* for (const auto& child : children) */
  /*   calculate(this->dimensions); */
  /* calculate_block_height(); */
}

void LayoutBox::calculate_block_width(const Dimensions& parent)
{
  const CSSBaseValue zero_length = LengthValue(0, yacss::UNIT_PX);
  const CSSBaseValue auto_keyword = KeywordValue("auto");

  CSSBaseValue width = styled_node->decl_lookup({ "width" }, auto_keyword);
  CSSBaseValue margin_left =
      styled_node->decl_lookup({ "margin-left", "margin" }, zero_length);
  CSSBaseValue margin_right =
      styled_node->decl_lookup({ "margin-right", "margin" }, zero_length);

  const CSSBaseValue& border_left =
      styled_node->decl_lookup({ "border-left", "border" }, zero_length);
  const CSSBaseValue& border_right =
      styled_node->decl_lookup({ "border-right", "border" }, zero_length);

  const CSSBaseValue& padding_left =
      styled_node->decl_lookup({ "padding-left", "padding" }, zero_length);
  const CSSBaseValue& padding_right =
      styled_node->decl_lookup({ "padding-right", "padding" }, zero_length);

  float total = to_px(width) + to_px(margin_left) + to_px(margin_right) +
                to_px(border_left) + to_px(border_right) + to_px(padding_left) +
                to_px(border_right);

  bool auto_width = width == auto_keyword;
  bool auto_margin_left = margin_left == auto_keyword;
  bool auto_margin_right = margin_right == auto_keyword;


  // if width is set to auto and there's an overflow,
  // any other 'auto' values become 0px
  if (auto_width && total > parent.content.width) {
    if (margin_left.type == ValueType::Keyword)
      margin_left = zero_length;

    if (margin_right.type == ValueType::Keyword)
      margin_right = zero_length;
  }

  float underflow = parent.content.width - total;

  // =auto: (w : false, mr: false, ml: false)
  if (!auto_width && !auto_margin_right && !auto_margin_left) {
    margin_right = LengthValue(to_px(margin_right) + underflow, yacss::UNIT_PX);
  }

  // =auto: (w : false, mr: false, ml: true)
  else if (!auto_width && !auto_margin_right && auto_margin_left) {
    margin_left = LengthValue(underflow, yacss::UNIT_PX);
  }

  // =auto: (w : false, mr: true, ml: false)
  else if (!auto_width && auto_margin_right && !auto_margin_left) {
    margin_right = LengthValue(underflow, yacss::UNIT_PX);
  }

  // =auto: (w: true, mr: any, ml: any)
  if (auto_width) {
    if (auto_margin_left)
      margin_left = zero_length;
    if (auto_margin_right)
      margin_right = zero_length;

    if (underflow >= 0) {
      width = LengthValue(underflow, yacss::UNIT_PX);
    } else {
      width = zero_length;
      margin_right =
          LengthValue(to_px(margin_right) + underflow, yacss::UNIT_PX);
    }
  }

  // =auto: (w : false, mr: true, ml: true)
  if (!auto_width && auto_margin_left && auto_margin_right) {
    margin_left = LengthValue(underflow / 2, yacss::UNIT_PX);
    margin_right = LengthValue(underflow / 2, yacss::UNIT_PX);
  }

  this->dimensions.content.width = to_px(width);
  this->dimensions.padding.left = to_px(padding_left);
  this->dimensions.padding.right = to_px(padding_right);

  this->dimensions.border.left = to_px(border_left);
  this->dimensions.border.right = to_px(border_right);

  this->dimensions.margin.left = to_px(margin_left);
  this->dimensions.margin.right = to_px(margin_right);
}

} // ! ns yabrowser
}; // ! ns layout
