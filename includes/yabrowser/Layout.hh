#ifndef YABROWSER__LAYOUT__LAYOUT_HH
#define YABROWSER__LAYOUT__LAYOUT_HH

#include "StyleTree.hh"

namespace yabrowser { namespace layout {

struct Rect
{
  float x;
  float y;
  float width;
  float height;
};

struct EdgeSizes
{
  float top;
  float right;
  float bottom;
  float left;
};

struct Dimensions
{
  Rect content;
  EdgeSizes padding;
  EdgeSizes border;
  EdgeSizes margin;
};

// CSS
enum class Display
{
  Undefined, Block, Inline,
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
public:
  LayoutBox(const style::StyledNode&);
  ~LayoutBox();
};

}}; // ! ns yabrowser layout

#endif
