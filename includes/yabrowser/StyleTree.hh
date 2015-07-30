#ifndef YABROWSER__STYLE__STYLETREE_HH
#define YABROWSER__STYLE__STYLETREE_HH

#include "yacss/CSS.hh"
#include "yahtml/DOM.hh"

#include <vector>
#include <memory>
#include <utility>
#include <algorithm>


namespace yabrowser { namespace style {

class StyledNode;
class Value;

typedef std::shared_ptr<Value> ValuePtr;
typedef std::shared_ptr<StyledNode> StyledNodePtr;
typedef std::shared_ptr<yacss::Rule> RulePtr;

typedef std::pair<RulePtr, unsigned> MatchedRule;

bool selector_matches(const yacss::Selector&, const yahtml::Element&);
MatchedRule rule_matches (const yacss::Rule&, const yahtml::Element&);
std::vector<MatchedRule> matching_rules (const yacss::Stylesheet&,
                                         const yahtml::Element&);
struct MatchedRuleLesser
{
  inline bool operator()(const MatchedRule& lhs, const MatchedRule& rhs) const
  {
    return lhs.second < rhs.second;
  }
};


class StyledNode
{
public:
  std::shared_ptr<yahtml::Node> node;
  std::map<std::string, ValuePtr> specified_values;
  std::vector<StyledNodePtr> children;
};

}}; // ! ns yabrowser style

#endif

