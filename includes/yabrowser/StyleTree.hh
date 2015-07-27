#ifndef YABROWSER__STYLETREE_HH
#define YABROWSER__STYLETREE_HH

#include "yacss/CSS.hh"
#include "yahtml/DOM.hh"
#include <memory>
#include <algorithm>

namespace yabrowser {

class StyledNode;
class Value;

typedef std::shared_ptr<Value> ValuePtr;
typedef std::shared_ptr<StyledNode> StyledNodePtr;

bool selector_matches(const yacss::Selector& sel, yahtml::Element& elem);

class StyledNode
{
public:
  std::shared_ptr<yahtml::Node> node;
  std::map<std::string, ValuePtr> specified_values;
  std::vector<StyledNodePtr> children;
};

}; // ! ns yabrowser

#endif

