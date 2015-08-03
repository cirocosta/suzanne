#include "gtest/gtest.h"
#include "yabrowser/Layout.hh"
#include "yacss/parser/driver.hh"
#include "yahtml/parser/driver.hh"

using namespace yabrowser;
using namespace yabrowser::style;
using namespace yacss;
using namespace yabrowser::layout;

TEST(Layout, SingleBlockElement) {
  /**
   * As any element contains at least an empty text we expect to have
   * a block box w/ an inline box inside
   */

  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char * html_source =
    "<h1></h1>";
  const char * css_source =
    "h1 { display: block; }";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  ::StyledNode sn (htmldriver.dom, cssdriver.stylesheet);
  LayoutBox layout (sn);

  EXPECT_EQ(layout.type, BoxType::BlockNode);
  EXPECT_EQ(layout.children.size(), 1);
  EXPECT_EQ(layout.children.at(0)->type, BoxType::InlineNode);
}

TEST(Layout, BlockWithInlineList) {
  /**
   *    """
   *      A block container box either contains only block-level boxes or
   *      establishes an inline formatting context and thus contains only
   *      inline-level boxes.
   *    """
   */
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char * html_source =
    "<ul>"
      "<li></li>"
      "<li></li>"
    "</ul>";
  const char * css_source =
    "ul { display: block; }"
    "li { display: inline; }";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  ::StyledNode sn (htmldriver.dom, cssdriver.stylesheet);
  LayoutBox ul_layout (sn);

  EXPECT_EQ(ul_layout.type, BoxType::BlockNode);
  EXPECT_EQ(ul_layout.children.size(), 2);
  EXPECT_EQ(ul_layout.children.at(0)->type, BoxType::InlineNode);
  EXPECT_EQ(ul_layout.children.at(1)->type, BoxType::InlineNode);

  LayoutBoxPtr& li0_layout = ul_layout.children.at(0);
  LayoutBoxPtr& li1_layout = ul_layout.children.at(1);

  EXPECT_EQ(li0_layout->type, BoxType::InlineNode);
  EXPECT_EQ(li1_layout->type, BoxType::InlineNode);
  EXPECT_EQ(li0_layout->children.size(), 1);
  EXPECT_EQ(li1_layout->children.size(), 1);

  EXPECT_EQ(li0_layout->children.at(0)->type, BoxType::InlineNode);
  EXPECT_EQ(li1_layout->children.at(0)->type, BoxType::InlineNode);
}

TEST(Layout, SimpleAnonymous) {
  /*
   * If a block container box has a block-level box inside it, then force
   * it to have only block-level boxes inside it (use anonymous boxes).
   */
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<body>"
      "<span></span>"
      "<h1></h1>"
    "</body>";


  const char* css_source =
    "body, h1 { display: block; }"
    "span { display: inline; }";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  ::StyledNode sn (htmldriver.dom, cssdriver.stylesheet);
  LayoutBox body_layout (sn);

  ASSERT_EQ(body_layout.type, BoxType::BlockNode);
  ASSERT_EQ(body_layout.children.size(), 2);

  LayoutBoxPtr& span_layout = body_layout.children.at(0);
  LayoutBoxPtr& h1_layout = body_layout.children.at(1);

  // span
  EXPECT_EQ(span_layout->type, BoxType::AnonymousBlock);
  EXPECT_EQ(span_layout->children.size(), 1);
  EXPECT_EQ(span_layout->children.at(0)->type, BoxType::InlineNode);

  // h1
  EXPECT_EQ(h1_layout->type, BoxType::BlockNode);
  EXPECT_EQ(h1_layout->children.size(), 1);
  EXPECT_EQ(h1_layout->children.at(0)->type, BoxType::InlineNode);
}


TEST(Layout, AnonymousWithListInside) {
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source =
    "<body>"
      "<h1></h1>"
      "<h2></h2>"
      "huehue <strong>brbr</strong> huehue"
    "</body>";


  const char* css_source =
    "body, h1, h2 { display: block; }"
    "span { display: inline; }";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  ::StyledNode sn (htmldriver.dom, cssdriver.stylesheet);
  LayoutBox body_layout (sn);

  // body will have 2 blocks and 1 anonymous
  ASSERT_EQ(body_layout.children.size(), 3);
  LayoutBoxPtr& h1_layout = body_layout.children.at(0);
  LayoutBoxPtr& h2_layout = body_layout.children.at(1);
  LayoutBoxPtr& text_layout = body_layout.children.at(2);

  EXPECT_EQ(h1_layout->type, BoxType::BlockNode);
  EXPECT_EQ(h2_layout->type, BoxType::BlockNode);
  EXPECT_EQ(text_layout->type, BoxType::AnonymousBlock);

  ASSERT_EQ(text_layout->children.size(), 3);
  LayoutBoxPtr& txt1_layout = text_layout->children.at(0);
  LayoutBoxPtr& strong_layout = text_layout->children.at(1);
  LayoutBoxPtr& txt2_layout = text_layout->children.at(2);

  EXPECT_EQ(txt1_layout->type, BoxType::InlineNode);
  EXPECT_EQ(strong_layout->type, BoxType::InlineNode);
  EXPECT_EQ(txt2_layout->type, BoxType::InlineNode);
}

