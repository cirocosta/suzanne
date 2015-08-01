#include "gtest/gtest.h"
#include "yabrowser/StyleTree.hh"
#include "yahtml/parser/driver.hh"
#include "yacss/parser/driver.hh"

using namespace yabrowser;
using namespace yabrowser::style;
using namespace yacss;

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

TEST(StyledTree, TreeWithClassSelector) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<body>"
      "<h1 class=\"header\">"
        "cool header 1 text"
      "</h1>"
      "<h2 class=\"header\">"
        "cool header 2 text"
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

  // OK w/ parsing
  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(cssdriver.result, 0);

  ASSERT_EQ(htmldriver.dom->type, yahtml::NodeType::Element);
  yahtml::DOMChild body = htmldriver.dom;
  yacss::Stylesheet* ss = &cssdriver.stylesheet;

  ::StyledNode sn (body, *ss);
  EXPECT_EQ(sn.specified_values.size(), 0); // no specified values for body
  ASSERT_EQ(sn.children.size(), 2);

  // h1
  StyledChild& h1_style = sn.children.at(0);
  ASSERT_EQ(h1_style->specified_values.size(), 2);
  EXPECT_EQ(h1_style->specified_values["color"].get<KeywordValue>().val,
            "green");
  EXPECT_EQ(h1_style->specified_values["width"].get<LengthValue>().val,
            300);

  // h1's text
  ASSERT_EQ(h1_style->children.size(), 1);
  StyledChild& h1_text_style = h1_style->children.at(0);
  EXPECT_EQ(h1_text_style->specified_values.size(), 0);


  // h2
  StyledChild& h2_style = sn.children.at(1);
  ASSERT_EQ(h2_style->specified_values.size(), 1);
  EXPECT_EQ(h2_style->specified_values["color"].get<KeywordValue>().val,
            "green");

  // h2's text
  ASSERT_EQ(h2_style->children.size(), 1);
  StyledChild& h2_text_style = h2_style->children.at(0);
  EXPECT_EQ(h2_text_style->specified_values.size(), 0);
}

