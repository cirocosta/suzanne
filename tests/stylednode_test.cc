#include "gtest/gtest.h"
#include "yabrowser/StyleTree.hh"
#include "yahtml/parser/driver.hh"
#include "yacss/parser/driver.hh"

using namespace yabrowser;
using namespace yabrowser::style;

TEST(StyledTree, SingleNodeStyleTree) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<h1>"
    "</h1>";

  const char* css_source =
    "h1 {"
      "color: black;"
    "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(cssdriver.result, 0);

  ASSERT_EQ(htmldriver.dom->type, yahtml::NodeType::Element);
  yahtml::DOMChild h1 = htmldriver.dom;
  yacss::Stylesheet* ss = &cssdriver.stylesheet;

  ::StyledNode sn (h1, *ss);

  ASSERT_EQ(sn.node.get(), h1.get());
  ASSERT_EQ(sn.specified_values.size(), 1);

  yacss::DeclarationContainer* decls = &sn.specified_values;
  EXPECT_EQ(decls->at("color").get<yacss::KeywordValue>().val, "black");

  ASSERT_EQ(sn.children.size(), 1);
  ASSERT_EQ(sn.children[0]->node->type, yahtml::NodeType::Text);
}

