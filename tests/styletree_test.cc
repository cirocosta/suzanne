#include "gtest/gtest.h"
#include "yahtml/parser/driver.hh"
#include "yacss/parser/driver.hh"
#include "yabrowser/StyleTree.hh"

using namespace yabrowser;

TEST(SelectorMatches, Simple) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<h1>"
    "</h1>";

  const char* css_source =
    "h1 {"
      "color: black;"
    "}"
    "";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(htmldriver.dom->type, yahtml::NodeType::Element);
  yahtml::Element* h1_elem = dynamic_cast<yahtml::Element*>(htmldriver.dom.get());

  ASSERT_EQ(cssdriver.result, 0);
  ASSERT_EQ(cssdriver.stylesheet.rules.size(), 1);
  ASSERT_EQ(cssdriver.stylesheet.rules[0].selectors.size(), 1);
  yacss::Selector* h1_sel = &(cssdriver.stylesheet.rules[0].selectors[0]);

  EXPECT_EQ(yabrowser::selector_matches(*h1_sel, *h1_elem), true);
}
