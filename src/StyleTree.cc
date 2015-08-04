#include "yabrowser/StyleTree.hh"

namespace yabrowser { namespace style {

using namespace yacss;
using namespace yahtml;

StyledNode::StyledNode (const DOMChild root, const Stylesheet& ss)
{
  node = root;

  if (root->type == NodeType::Element) {
    Element* elem = static_cast<yahtml::Element*>(root.get());
    specified_values = compute_specified_values(ss, *elem);
    const auto it = specified_values.find("display");

    if (it == specified_values.end()) {
      display = DISPLAY_INLINE;
    } else {
      std::string disp = it->second.get<KeywordValue>().val;

      if (disp == "inline") {
        display = DISPLAY_INLINE;
      } else if (disp == "block") {
        display = DISPLAY_BLOCK;
      } else if (disp == "none") {
        display = DISPLAY_NONE;
      } else {
        throw std::runtime_error("Unknown Display value.");
      }
    }
  } else {
    specified_values = DeclarationContainer {};
    display = DISPLAY_INLINE;
  }

  // recursively construct the styled tree
  for (const auto& child : root->children) {
    StyledNode sn (child, ss);
    children.push_back(std::make_shared<StyledNode>(sn));
  }
}

StyledNode::~StyledNode ()
{ }

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

MatchedRule rule_matches (const yacss::RulePtr& rule, const yahtml::Element& elem)
{
  for (const auto& selector : rule->selectors)
    if (selector_matches(selector, elem))
      return MatchedRule {rule, selector.specificity};

  return MatchedRule {nullptr, 0};
}

std::vector<MatchedRule> matching_rules (const Stylesheet& ss,
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

DeclarationContainer compute_specified_values (const Stylesheet& ss,
                                               const Element& elem)
{
  DeclarationContainer spec_values;

  for (const auto& matched_rule : matching_rules(ss, elem))
    for (const auto& decl : matched_rule.first->declarations)
      spec_values[decl.first] = decl.second;

  return spec_values;
}

CSSBaseValue StyledNode::decl_lookup (
const std::initializer_list<std::string> keys, const CSSBaseValue& def) const
{
  DeclarationContainer::const_iterator it;

  for (const auto& key : keys) {
    it = specified_values.find(key);
    if (it != specified_values.end())
      return it->second;
  }

  return CSSBaseValue(def);
}

}}; // ! ns yabrowser style

