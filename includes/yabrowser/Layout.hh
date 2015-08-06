#ifndef YABROWSER__LAYOUT__LAYOUT_HH
#define YABROWSER__LAYOUT__LAYOUT_HH

#include "StyleTree.hh"
#include <memory>
#include <vector>

namespace yabrowser { namespace layout {

class LayoutBox;
typedef std::shared_ptr<LayoutBox> LayoutBoxPtr;
typedef std::vector<LayoutBoxPtr> LayoutBoxContainer;

struct Rect
{
  float x;
  float y;
  float width;
  float height;

  inline Rect (float x_ = 0.0, float y_ = 0.0, float w = 0.0, float h = 0.0)
    : x(x_), y(y_), width(w), height(h)
  {}
};

struct EdgeSizes
{
  float top;
  float right;
  float bottom;
  float left;

  inline EdgeSizes (float t = 0.0, float r = 0.0, float b = 0.0, float l = 0.0)
    : top(t), right(r), bottom(b), left(l)
  {}
};

struct Dimensions
{
  Rect content;
  EdgeSizes padding;
  EdgeSizes border;
  EdgeSizes margin;

  inline Dimensions (Rect c = Rect(), EdgeSizes p = EdgeSizes(),
              EdgeSizes b = EdgeSizes(), EdgeSizes m = EdgeSizes())
    : content(c), padding(p), border(b), margin(m)
  {}
};

enum class BoxType
{
  BlockNode, InlineNode, AnonymousBlock
};

class LayoutBox
{
public:
  BoxType type;
  Dimensions dimensions;
  LayoutBoxContainer children;
  style::StyledChild styled_node;
public:
  LayoutBox(const style::StyledChild&, BoxType bt = BoxType::BlockNode);
  ~LayoutBox();

  void calculate (const Dimensions& parent);

  /* // block */
  void calculate_block_layout(const Dimensions& parent);
  void calculate_block_width(const Dimensions& parent);
  /* void calculate_block_position(const LayoutBox& parent); */
  /* void calculate_block_height(); */
};

}}; // ! ns yabrowser layout

#endif
