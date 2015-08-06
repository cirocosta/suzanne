#ifndef YABROWSER__LAYOUT__LAYOUT_HH
#define YABROWSER__LAYOUT__LAYOUT_HH

#include "StyleTree.hh"
#include <memory>
#include <vector>

namespace yabrowser
{
namespace layout
{

class LayoutBox;
struct EdgeSizes;
struct Rect;

typedef std::shared_ptr<LayoutBox> LayoutBoxPtr;
typedef std::vector<LayoutBoxPtr> LayoutBoxContainer;

struct EdgeSizes {
  float top;
  float right;
  float bottom;
  float left;

  inline EdgeSizes(float t = 0.0, float r = 0.0, float b = 0.0, float l = 0.0)
      : top(t), right(r), bottom(b), left(l)
  {
  }
};

struct Rect {
  float x;
  float y;
  float width;
  float height;

  constexpr Rect(float x_ = 0.0, float y_ = 0.0, float w = 0.0, float h = 0.0)
      : x(x_), y(y_), width(w), height(h)
  {
  }

  constexpr Rect expanded_by(const EdgeSizes& edge) const
  {
    return Rect(x - edge.left, y - edge.top, width + edge.left + edge.right,
                height + edge.top + edge.bottom);
  }
};


struct Dimensions {
  Rect content;
  EdgeSizes padding;
  EdgeSizes border;
  EdgeSizes margin;

  inline Dimensions(Rect c = Rect(), EdgeSizes p = EdgeSizes(),
                    EdgeSizes b = EdgeSizes(), EdgeSizes m = EdgeSizes())
      : content(c), padding(p), border(b), margin(m)
  {
  }

  constexpr Rect padding_box () const
  {
    return content.expanded_by(padding);
  }

  constexpr Rect border_box () const
  {
    return padding_box().expanded_by(border);
  }

  constexpr Rect margin_box () const
  {
    return border_box().expanded_by(margin);
  }
};

enum class BoxType { BlockNode, InlineNode, AnonymousBlock };

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

  void calculate(const Dimensions& parent);

  /* // block */
  void calculate_block_layout(const Dimensions& parent);
  void calculate_block_width(const Dimensions& parent);
  void calculate_block_position(const Dimensions& parent);
  /* void calculate_block_height(); */
};
}
}; // ! ns yabrowser layout

#endif
