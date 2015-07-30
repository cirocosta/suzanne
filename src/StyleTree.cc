#include "yabrowser/StyleTree.hh"

namespace yabrowser { namespace style {

bool selector_matches (const yacss::Selector& sel, const yahtml::Element& elem)
{
  yahtml::AttrMap::const_iterator it;

  if (!(sel.tag.empty()) && sel.tag != elem.tag_name)
    return false;

  if (!(sel.id.empty())) {
    it = elem.attr_map.find("id");

    if (it == elem.attr_map.end() || it->second != sel.id)
      return false;
  }

  if (!sel.classes.empty()) {
    for (const auto& klass : sel.classes) {
      auto res = std::find(elem.classes.begin(), elem.classes.end(), klass);

      if (res == elem.classes.end())
        return false;
    }
  }

  return true;
}

MatchedRule rule_matches (const yacss::Rule& rule, const yahtml::Element& elem)
{
  for (const auto& selector : rule.selectors)
    if (selector_matches(selector, elem))
      return MatchedRule {std::make_shared<yacss::Rule>(rule), selector.specificity};

  return MatchedRule {nullptr, 0};
}

std::vector<MatchedRule> matching_rules (const yacss::Stylesheet& ss,
                                         const yahtml::Element& elem)
{
  std::vector<MatchedRule> rules_matched;

  for (const auto& rule : ss.rules) {
    MatchedRule matched_rule = rule_matches(rule, elem);

    if (!matched_rule.second)
      continue;

    rules_matched.push_back(matched_rule);
  }

  std::sort(rules_matched.begin(), rules_matched.end(), MatchedRuleLesser());

  return rules_matched;
}

}}; // ! ns yabrowser style

