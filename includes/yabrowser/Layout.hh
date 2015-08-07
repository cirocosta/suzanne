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

  inline Rect(float x_ = 0.0, float y_ = 0.0, float w = 0.0, float h = 0.0)
      : x(x_), y(y_), width(w), height(h)
  {
  }

  inline Rect expanded_by(const EdgeSizes& edge) const
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

  inline Rect padding_box() const { return content.expanded_by(padding); }

  inline Rect border_box() const
  {
    return padding_box().expanded_by(border);
  }

  inline Rect margin_box() const { return border_box().expanded_by(margin); }
};

enum class BoxType { BlockNode, InlineNode, AnonymousBlock };

class LayoutBox
{
public:
  BoxType type;
  Dimensions dimensions;
  LayoutBoxContainer children;
  style::StyledChild styled_node;
  LayoutBox* parent;

public:
  LayoutBox(const style::StyledChild&, Dimensions dim = Dimensions());
  LayoutBox(const style::StyledChild&, BoxType bt, LayoutBox* lbp);
  ~LayoutBox();

  LayoutBox(const LayoutBox&) = delete;
  const LayoutBox& operator=(const LayoutBox&) = delete;

  void calculate();

  /* // block */
  void calculate_block_layout();
  void calculate_block_width();
  void calculate_block_position();
  /* void calculate_block_height(); */
private:
  void _init_tree();
};
}  // ! ns yabrowser
}; // ! ns layout

#endif
