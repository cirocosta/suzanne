#include "gtest/gtest.h"
#include "yabrowser/StyleTree.hh"
#include "yahtml/parser/driver.hh"
#include "yacss/parser/driver.hh"

using namespace yabrowser;
using namespace yabrowser::style;
using namespace yacss;

TEST(StyledNode, SingleNodeStyleTree) {
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

TEST(StyledNode, TreeWithClassSelector) {
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

TEST(StyledNode, GetExistingValue) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<body>"
      "<h1 class=\"header\">"
        "cool header 1 text"
      "</h1>"
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
  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(cssdriver.result, 0);

  ::StyledNode sn (htmldriver.dom, cssdriver.stylesheet);

  // h1
  StyledChild& h1_style = sn.children.at(0);
  ASSERT_EQ(h1_style->specified_values.size(), 2);
  EXPECT_EQ(h1_style->get_value<KeywordValue>("color")->val, "green");
  EXPECT_EQ(h1_style->get_value<LengthValue>("width")->val, 300);
  EXPECT_EQ(h1_style->get_value<LengthValue>("lol"), nullptr);
}

TEST(StyledNode, Display) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<body>"
      "<h1 class=\"header\">"
        "cool header 1 text"
      "</h1>"
      "<div>"
      "</div>"
    "</body>";

  const char* css_source =
    "h1 {"
      "display: block;"
    "}"

    "div {"
      "display: none;"
    "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result, 0);
  ASSERT_EQ(cssdriver.result, 0);

  ::StyledNode sn (htmldriver.dom, cssdriver.stylesheet);

  // h1
  StyledChild& h1_style = sn.children.at(0);
  StyledChild& h1_text_style = h1_style->children.at(0);
  StyledChild& div_style = sn.children.at(1);

  EXPECT_EQ(h1_style->display, DISPLAY_BLOCK);
  EXPECT_EQ(h1_text_style->display, DISPLAY_INLINE);
  EXPECT_EQ(div_style->display, DISPLAY_NONE);
}

TEST(SpecifiedValues, DeclarationLookup) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<body>"
      "<h1></h1>"
      "<h2></h2>"
    "</body>";

  const char* css_source =
    "h1 {"
    " margin: 10px;"
    " border-left: 5px;"
    "}"

    "h2 {"
    " width: 10px;"
    "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  ::StyledNode sn (htmldriver.dom, cssdriver.stylesheet);

  StyledChild& h1_style = sn.children.at(0);
  StyledChild& h2_style = sn.children.at(1);

  // default values
  const CSSBaseValue zero_length = LengthValue(0, yacss::UNIT_PX);
  const CSSBaseValue auto_keyword = KeywordValue("auto");

  // margin - both specified by the shorthand
  const CSSBaseValue& margin_left =
    h1_style->decl_lookup({"margin", "margin-left"}, zero_length);

  const CSSBaseValue& margin_right =
    h1_style->decl_lookup({"margin", "margin-right"}, zero_length);

  EXPECT_EQ(margin_left.get<LengthValue>().val, 10);
  EXPECT_EQ(margin_right.get<LengthValue>().val, 10);

  //width - not specified in the first (keyword)
  //      - specified in the second (length)
  const CSSBaseValue& h1_width =
    h1_style->decl_lookup({"width"}, auto_keyword);
  const CSSBaseValue& h2_width =
    h2_style->decl_lookup({"width"}, auto_keyword);

  EXPECT_EQ(h1_width.type, yacss::ValueType::Keyword);
  EXPECT_EQ(h1_width.get<KeywordValue>().val, "auto");
  EXPECT_EQ(h2_width.type, yacss::ValueType::Length);

  // border - no shorthand specified. Only one (left) specified
  const CSSBaseValue& border_left =
    h1_style->decl_lookup({"border", "border-left"}, zero_length);
  const CSSBaseValue& border_right =
    h1_style->decl_lookup({"border", "border-right"}, zero_length);

  EXPECT_EQ(border_left.get<LengthValue>().val, 5);
  EXPECT_EQ(border_right.get<LengthValue>().val, 0);
}

