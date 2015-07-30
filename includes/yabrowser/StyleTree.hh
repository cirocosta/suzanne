#ifndef YABROWSER__STYLE__STYLETREE_HH
#define YABROWSER__STYLE__STYLETREE_HH

#include "yacss/CSS.hh"
#include "yahtml/DOM.hh"

#include <vector>
#include <memory>
#include <utility>
#include <algorithm>


namespace yabrowser { namespace style {

typedef std::pair<yacss::RulePtr, unsigned> MatchedRule;
typedef std::map<std::string, std::string> SpecifiedValues;

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

