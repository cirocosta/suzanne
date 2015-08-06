#include "gtest/gtest.h"
#include "yahtml/parser/driver.hh"
#include "yacss/parser/driver.hh"
#include "yabrowser/StyleTree.hh"

using namespace yabrowser::style;
using namespace yacss;
using namespace yahtml;

TEST(SelectorMatches, SimpleMath) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<h1>"
    "</h1>";

  const char* css_source =
    "h1 {"
      "color: black;"
    "}"
    ""
    "h2 {"
      "color: green;"
    "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(htmldriver.dom->type, yahtml::NodeType::Element);
  yahtml::Element* h1_elem = dynamic_cast<yahtml::Element*>(htmldriver.dom.get());

  ASSERT_EQ(cssdriver.result, 0);
  ASSERT_EQ(cssdriver.stylesheet.rules.size(), 2);
  ASSERT_EQ(cssdriver.stylesheet.rules[0]->selectors.size(), 1);
  yacss::Selector* h1_sel = &(cssdriver.stylesheet.rules[0]->selectors[0]);
  EXPECT_EQ(selector_matches(*h1_sel, *h1_elem), true);


  ASSERT_EQ(cssdriver.stylesheet.rules[1]->selectors.size(), 1);
  yacss::Selector* h2_sel = &(cssdriver.stylesheet.rules[1]->selectors[0]);
  EXPECT_EQ(selector_matches(*h2_sel, *h1_elem), false);
}


TEST(SelectorMatches, MultipleSelectors) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<body>"
      "<h1 class=\"header\">"
      "</h1>"
      "<h2 class=\"header\">"
      "</h2>"
    "</body>";

  const char* css_source =
    ".header {"
      "margin: auto;"
    "}"

    "h1.header {"
      "color: black;"
    "}"

    ""
    "h2.header {"
      "color: green;"
    "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  // html parsed
  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(htmldriver.dom->type, yahtml::NodeType::Element);
  yahtml::Element* body = dynamic_cast<yahtml::Element*>(htmldriver.dom.get());

  // grab the elements
  ASSERT_EQ(body->children.size(), 2);
  yahtml::Element* h1 = dynamic_cast<yahtml::Element*>(body->children[0].get());
  yahtml::Element* h2 = dynamic_cast<yahtml::Element*>(body->children[1].get());

  // css parsed
  ASSERT_EQ(cssdriver.result, 0);
  ASSERT_EQ(cssdriver.stylesheet.rules.size(), 3);

  // grab those selectors
  ASSERT_EQ(cssdriver.stylesheet.rules[0]->selectors.size(), 1);
  yacss::Selector* class_sel = &(cssdriver.stylesheet.rules[0]->selectors[0]);
  ASSERT_EQ(cssdriver.stylesheet.rules[1]->selectors.size(), 1);
  yacss::Selector* h1_sel = &(cssdriver.stylesheet.rules[1]->selectors[0]);
  ASSERT_EQ(cssdriver.stylesheet.rules[2]->selectors.size(), 1);
  yacss::Selector* h2_sel = &(cssdriver.stylesheet.rules[2]->selectors[0]);

  EXPECT_EQ(selector_matches(*h1_sel, *h1), true);
  EXPECT_EQ(selector_matches(*class_sel, *h1), true);

  EXPECT_EQ(selector_matches(*h2_sel, *h2), true);
  EXPECT_EQ(selector_matches(*class_sel, *h2), true);

  EXPECT_EQ(selector_matches(*h1_sel, *h2), false);
  EXPECT_EQ(selector_matches(*h2_sel, *h1), false);
}

TEST(RulesMatcher, OneRuleMultipleSelectors) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<body>"
      "<h1 class=\"header\">"
      "</h1>"
      "<h2 class=\"header\">"
      "</h2>"
    "</body>";

  const char* css_source =
    "h1, h2 {"
      "color: red;"
    "}"

    ".header {"
      "margin: green;"
    "}"

    "h1.header,"
    "h2.header {"
      "color: blue;"
    "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  // html parsed
  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(htmldriver.dom->type, yahtml::NodeType::Element);
  yahtml::Element* body = dynamic_cast<yahtml::Element*>(htmldriver.dom.get());

  // grab the elements
  ASSERT_EQ(body->children.size(), 2);
  yahtml::Element* h1 = dynamic_cast<yahtml::Element*>(body->children[0].get());
  yahtml::Element* h2 = dynamic_cast<yahtml::Element*>(body->children[1].get());

  // css parsed
  ASSERT_EQ(cssdriver.result, 0);
  ASSERT_EQ(cssdriver.stylesheet.rules.size(), 3);

  // grab those rules
  ASSERT_EQ(cssdriver.stylesheet.rules[0]->selectors.size(), 2);
  yacss::RulePtr tags_rule = cssdriver.stylesheet.rules[0];
  ASSERT_EQ(cssdriver.stylesheet.rules[1]->selectors.size(), 1);
  yacss::RulePtr class_rule = cssdriver.stylesheet.rules[1];
  ASSERT_EQ(cssdriver.stylesheet.rules[2]->selectors.size(), 2);
  yacss::RulePtr tag_w_class_rule = cssdriver.stylesheet.rules[2];

  // expect they all match (specificity changes only)
  EXPECT_NE(rule_matches(tags_rule, *h1).first, nullptr);
  EXPECT_NE(rule_matches(tags_rule, *h2).first, nullptr);

  EXPECT_NE(rule_matches(class_rule, *h1).first, nullptr);
  EXPECT_NE(rule_matches(class_rule, *h2).first, nullptr);

  EXPECT_NE(rule_matches(tag_w_class_rule, *h1).first, nullptr);
  EXPECT_NE(rule_matches(tag_w_class_rule, *h2).first, nullptr);
}

TEST(MatchingRules, OneRuleMultipleSelectors) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<body>"
      "<h1 class=\"header\">"
      "</h1>"
      "<h2 class=\"header\">"
      "</h2>"
    "</body>";

  const char* css_source =
    "h1, h2 {"
      "color: red;"
    "}"

    ".header {"
      "margin: green;"
    "}"

    "h1.header,"
    "h2.header {"
      "color: blue;"
    "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  // html parsed
  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(htmldriver.dom->type, yahtml::NodeType::Element);
  yahtml::Element* body = dynamic_cast<yahtml::Element*>(htmldriver.dom.get());

  // grab the elements
  ASSERT_EQ(body->children.size(), 2);
  yahtml::Element* h1 = dynamic_cast<yahtml::Element*>(body->children[0].get());
  yahtml::Element* h2 = dynamic_cast<yahtml::Element*>(body->children[1].get());

  // css parsed
  ASSERT_EQ(cssdriver.result, 0);
  ASSERT_EQ(cssdriver.stylesheet.rules.size(), 3);
  yacss::Stylesheet* stylesheet = &cssdriver.stylesheet;

  // grab some rules
  ASSERT_EQ(cssdriver.stylesheet.rules[0]->selectors.size(), 2);
  yacss::RulePtr tags_rule = cssdriver.stylesheet.rules[0];
  ASSERT_EQ(cssdriver.stylesheet.rules[1]->selectors.size(), 1);
  yacss::RulePtr class_rule = cssdriver.stylesheet.rules[1];
  ASSERT_EQ(cssdriver.stylesheet.rules[2]->selectors.size(), 2);
  yacss::RulePtr tag_w_class_rule = cssdriver.stylesheet.rules[2];

  // match rules with elements
  std::vector<MatchedRule> body_rules = matching_rules(*stylesheet, *body);
  std::vector<MatchedRule> h1_rules = matching_rules(*stylesheet, *h1);
  std::vector<MatchedRule> h2_rules = matching_rules(*stylesheet, *h2);

  EXPECT_EQ(body_rules.size(), 0);
  EXPECT_EQ(h1_rules.size(), 3);
  EXPECT_EQ(h2_rules.size(), 3);

  EXPECT_EQ(h1_rules[0].first, tags_rule);
  EXPECT_EQ(h1_rules[1].first, class_rule);
  EXPECT_EQ(h1_rules[2].first, tag_w_class_rule);

  EXPECT_EQ(h2_rules[0].first, tags_rule);
  EXPECT_EQ(h2_rules[1].first, class_rule);
  EXPECT_EQ(h2_rules[2].first, tag_w_class_rule);
}

TEST(SpecifiedValues, ClassSelector) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver (true, true);

  const char* html_source =
    "<body>"
      "<h1 class=\"header\">"
      "</h1>"
      "<h2 class=\"header\">"
      "</h2>"
    "</body>";

  const char* css_source =
    "h1 {"
    " color: black;"
    " width: 300px;"
    "}"

    ".header {"
      "color: green;"
    "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  // html parsed
  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(htmldriver.dom->type, yahtml::NodeType::Element);
  yahtml::Element* body = dynamic_cast<yahtml::Element*>(htmldriver.dom.get());

  // grab the elements
  ASSERT_EQ(body->children.size(), 2);
  yahtml::Element* h1 = dynamic_cast<yahtml::Element*>(body->children[0].get());
  yahtml::Element* h2 = dynamic_cast<yahtml::Element*>(body->children[1].get());

  // css parsed
  ASSERT_EQ(cssdriver.result, 0);
  ASSERT_EQ(cssdriver.stylesheet.rules.size(), 2);
  yacss::Stylesheet* stylesheet = &cssdriver.stylesheet;

  // grab some rules
  ASSERT_EQ(cssdriver.stylesheet.rules[0]->selectors.size(), 1);
  ASSERT_EQ(cssdriver.stylesheet.rules[1]->selectors.size(), 1);
  yacss::RulePtr tag_rule = cssdriver.stylesheet.rules[0];
  yacss::RulePtr class_rule = cssdriver.stylesheet.rules[1];
  ASSERT_EQ(tag_rule->selectors[0].tag, "h1");
  ASSERT_EQ(class_rule->selectors[0].classes[0], "header");

  // match rules with elements
  ASSERT_EQ(matching_rules(*stylesheet, *body).size(), 0);
  ASSERT_EQ(matching_rules(*stylesheet, *h1).size(), 2);
  ASSERT_EQ(matching_rules(*stylesheet, *h2).size(), 1);

  // compute specified values
  DeclarationContainer h1_decls = compute_specified_values(*stylesheet, *h1);
  DeclarationContainer h2_decls = compute_specified_values(*stylesheet, *h2);

  EXPECT_EQ(h1_decls.size(), 2);
  EXPECT_EQ(h1_decls["color"].get<KeywordValue>().val, "green");
  EXPECT_EQ(h1_decls["width"].get<LengthValue>().val, 300);

  EXPECT_EQ(h2_decls.size(), 1);
  EXPECT_EQ(h2_decls["color"].get<KeywordValue>().val, "green");
}

