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

typedef std::pair<yacss::RulePtr, unsigned> MatchedRule;
typedef std::shared_ptr<StyledNode> StyledChild;
typedef std::vector<StyledChild> StyledChildren;


class StyledNode
{
public:
  yahtml::DOMChild node;
  yacss::DeclarationContainer specified_values;
  StyledChildren children;
public:
  StyledNode(const yahtml::DOMChild root, const yacss::Stylesheet& ss);
  ~StyledNode();

  template<typename T>
  inline const T* get_value (const std::string& name) const
  {
    yacss::DeclarationContainer::const_iterator it = specified_values.find(name);
    if (it == specified_values.end())
      return nullptr;

    return &it->second.get<T>();
  }
};

bool selector_matches(const yacss::Selector&, const yahtml::Element&);

MatchedRule rule_matches (const yacss::RulePtr&, const yahtml::Element&);

std::vector<MatchedRule> matching_rules (const yacss::Stylesheet&,
                                         const yahtml::Element&);

yacss::DeclarationContainer compute_specified_values (
    const yacss::Stylesheet&, const yahtml::Element&
);

struct MatchedRuleLesser
{
  inline bool operator()(const MatchedRule& lhs, const MatchedRule& rhs) const
  {
    return lhs.second < rhs.second;
  }
};

}}; // ! ns yabrowser style

#endif

