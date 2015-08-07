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


// ROOT
LayoutBox::LayoutBox(const StyledChild& sn, Dimensions initial_dimensions)
    : type(BoxType::BlockNode),
      dimensions(initial_dimensions),
      children(LayoutBoxContainer()),
      styled_node(sn),
      parent(this)
{
  dimensions.content.height = 0.0;

  _init_tree();
}

// NODE or LEAF
LayoutBox::LayoutBox(const StyledChild& sn, const BoxType bt, LayoutBox* lbp)
    : type(bt),
      dimensions(Dimensions()),
      children(LayoutBoxContainer()),
      parent(lbp)
{
  if (type == BoxType::AnonymousBlock)
    return;

  styled_node = sn;
  _init_tree();
}

// BUILD
void LayoutBox::_init_tree()
{
  bool has_block = false;
  bool constructed_anon = false;
  LayoutBoxContainer::const_iterator last_anon_it;

  for (const auto& child : styled_node->children) {
    if (child->display == DISPLAY_BLOCK) {
      has_block = true;
      break;
    }
  }

  for (const auto& child : styled_node->children) {
    switch (child->display) {
      case DISPLAY_INLINE:
        if (!has_block) {
          children.push_back(
              std::make_shared<LayoutBox>(child, BoxType::InlineNode, this));

          continue;
        }

        if (!constructed_anon) {
          children.push_back(std::make_shared<LayoutBox>(
              child, BoxType::AnonymousBlock, this));
          last_anon_it = children.end() - 1;
          constructed_anon = true;
        }

        (*last_anon_it)
            ->children.push_back(
                std::make_shared<LayoutBox>(child, BoxType::InlineNode, this));
        break;

      case DISPLAY_BLOCK:
        children.push_back(
            std::make_shared<LayoutBox>(child, BoxType::BlockNode, this));
        constructed_anon = false;
        break;

      case DISPLAY_NONE:
        continue;
    }
  }
}

LayoutBox::~LayoutBox() {}

void LayoutBox::calculate()
{
  if (this->type == BoxType::BlockNode) {
    calculate_block_layout();
  } else if (this->type == BoxType::InlineNode) {
    std::cerr << "LayoutBox::calculate - inlinenode unsupported" << std::endl;
  } else if (this->type == BoxType::AnonymousBlock) {
    std::cerr << "LayoutBox::calculate - anonblock unsupported" << std::endl;
  } else {
    std::cerr << "LayoutBox::calculate - unsupported" << std::endl;
  }
}

void LayoutBox::calculate_block_layout()
{
  calculate_block_width();
  calculate_block_position();

  for (const auto& child : children) {
    child->calculate();
    this->dimensions.content.height += child->dimensions.margin_box().height;
  }

  /* calculate_block_height(); */
}

void LayoutBox::calculate_block_width()
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
  if (auto_width && total > parent->dimensions.content.width) {
    if (margin_left.type == ValueType::Keyword)
      margin_left = zero_length;

    if (margin_right.type == ValueType::Keyword)
      margin_right = zero_length;
  }

  float underflow = parent->dimensions.content.width - total;

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

  dimensions.content.width = to_px(width);
  dimensions.padding.left = to_px(padding_left);
  dimensions.padding.right = to_px(padding_right);

  dimensions.border.left = to_px(border_left);
  dimensions.border.right = to_px(border_right);

  dimensions.margin.left = to_px(margin_left);
  dimensions.margin.right = to_px(margin_right);
}

void LayoutBox::calculate_block_position()
{
  const CSSBaseValue zero_length = LengthValue(0, yacss::UNIT_PX);

  dimensions.margin.top =
      to_px(styled_node->decl_lookup({ "margin-top", "margin" }, zero_length));

  dimensions.margin.bottom = to_px(
      styled_node->decl_lookup({ "margin-bottom", "margin" }, zero_length));

  dimensions.border.top = to_px(styled_node->decl_lookup(
      { "border-top-width", "border-width" }, zero_length));
  dimensions.border.top = to_px(styled_node->decl_lookup(
      { "border-bottom-width", "border-width" }, zero_length));

  dimensions.border.top = to_px(
      styled_node->decl_lookup({ "padding-top", "padding" }, zero_length));
  dimensions.border.top = to_px(
      styled_node->decl_lookup({ "padding-bottom", "padding" }, zero_length));

  dimensions.content.x = parent->dimensions.content.x + dimensions.margin.left +
                         dimensions.border.left + dimensions.padding.left;

  // Position the box below all the previous boxes in the container.
  dimensions.content.y = parent->dimensions.content.height +
                         parent->dimensions.content.y + dimensions.margin.top +
                         dimensions.border.top + dimensions.padding.top;
}

} // ! ns yabrowser
}; // ! ns layout
