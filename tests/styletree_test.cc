#include "gtest/gtest.h"
#include "yahtml/parser/driver.hh"
#include "yacss/parser/driver.hh"
#include "yabrowser/StyleTree.hh"

using namespace yabrowser;

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
  ASSERT_EQ(cssdriver.stylesheet.rules[0].selectors.size(), 1);
  yacss::Selector* h1_sel = &(cssdriver.stylesheet.rules[0].selectors[0]);
  EXPECT_EQ(yabrowser::selector_matches(*h1_sel, *h1_elem), true);


  ASSERT_EQ(cssdriver.stylesheet.rules[1].selectors.size(), 1);
  yacss::Selector* h2_sel = &(cssdriver.stylesheet.rules[1].selectors[0]);
  EXPECT_EQ(yabrowser::selector_matches(*h2_sel, *h1_elem), false);
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
  ASSERT_EQ(cssdriver.stylesheet.rules[0].selectors.size(), 1);
  yacss::Selector* class_sel = &(cssdriver.stylesheet.rules[0].selectors[0]);
  ASSERT_EQ(cssdriver.stylesheet.rules[1].selectors.size(), 1);
  yacss::Selector* h1_sel = &(cssdriver.stylesheet.rules[1].selectors[0]);
  ASSERT_EQ(cssdriver.stylesheet.rules[2].selectors.size(), 1);
  yacss::Selector* h2_sel = &(cssdriver.stylesheet.rules[2].selectors[0]);

  EXPECT_EQ(yabrowser::selector_matches(*h1_sel, *h1), true);
  EXPECT_EQ(yabrowser::selector_matches(*class_sel, *h1), true);

  EXPECT_EQ(yabrowser::selector_matches(*h2_sel, *h2), true);
  EXPECT_EQ(yabrowser::selector_matches(*class_sel, *h2), true);

  EXPECT_EQ(yabrowser::selector_matches(*h1_sel, *h2), false);
  EXPECT_EQ(yabrowser::selector_matches(*h2_sel, *h1), false);
}

