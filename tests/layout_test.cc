#include "gtest/gtest.h"
#include "yabrowser/Layout.hh"
#include "yacss/parser/driver.hh"
#include "yahtml/parser/driver.hh"

using namespace yabrowser;
using namespace yabrowser::style;
using namespace yacss;
using namespace yabrowser::layout;

TEST(Layout, SingleBlockElement)
{
  /**
   * As any element contains at least an empty text we expect to have
   * a block box w/ an inline box inside
   */

  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<h1></h1>";
  const char* css_source = "h1 { display: block; }";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet));

  EXPECT_EQ(layout.type, BoxType::BlockNode);
  EXPECT_EQ(layout.children.size(), 1);
  EXPECT_EQ(layout.children.at(0)->type, BoxType::InlineNode);
}

TEST(Layout, BlockWithInlineList)
{
  /**
   *    """
   *      A block container box either contains only block-level boxes or
   *      establishes an inline formatting context and thus contains only
   *      inline-level boxes.
   *    """
   */
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<ul>"
                            "<li></li>"
                            "<li></li>"
                            "</ul>";
  const char* css_source = "ul { display: block; }"
                           "li { display: inline; }";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);

  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox ul_layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet));

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

TEST(Layout, SimpleAnonymous)
{
  /*
   * If a block container box has a block-level box inside it, then force
   * it to have only block-level boxes inside it (use anonymous boxes).
   */
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<body>"
                            "<span></span>"
                            "<h1></h1>"
                            "</body>";

  const char* css_source = "body, h1 { display: block; }"
                           "span { display: inline; }";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox body_layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet));

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

TEST(Layout, AnonymousWithListInside)
{
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<body>"
                            "<h1></h1>"
                            "<h2></h2>"
                            "huehue <strong>brbr</strong> huehue"
                            "</body>";

  const char* css_source = "body, h1, h2 { display: block; }"
                           "span { display: inline; }";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox body_layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet));

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

TEST(Layout, DisplayNone)
{
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<!DOCTYPE html>"
                            "<html>"
                            " <head>"
                            "  <title>My Title</title>"
                            " </head>"
                            " <body>"
                            "   huehue brbr"
                            " </body>"
                            "</html>";

  const char* css_source = "head { display: none; }"
                           "body { display: block; }";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox html_layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet));

  // DOM structure contains head elem
  ASSERT_EQ(htmldriver.dom->children.size(), 2);

  // head will get out of the layouttree
  ASSERT_EQ(html_layout.children.size(), 1);
  LayoutBoxPtr& body_layout = html_layout.children.at(0);
  EXPECT_EQ(body_layout->type, BoxType::BlockNode);
  ASSERT_EQ(body_layout->children.size(), 1);
  EXPECT_EQ(body_layout->children.at(0)->type, BoxType::InlineNode);
}

TEST(BlockLayout, SingleFlatBodyWidth)
{
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<body>"
                            "</body>";

  const char* css_source = "body {"
                           "  display: block "
                           "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox body_layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet),
      Dimensions(Rect(0.0, 0.0, 200.0, 200.0)));
  body_layout.calculate();

  EXPECT_EQ(body_layout.dimensions.content.width, 200);
  EXPECT_EQ(body_layout.dimensions.content.x, 0.0);
  EXPECT_EQ(body_layout.dimensions.content.y, 0.0);
}

TEST(BlockLayout, SingleFlatBodyFixedWidthAutoMargin)
{
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<body>"
                            "</body>";

  const char* css_source = "body {"
                           "  display: block;"
                           "  width: 100px;"
                           "  margin: auto"
                           "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox body_layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet),
      Dimensions(Rect(0.0, 0.0, 200.0, 0.0)));
  body_layout.calculate();

  EXPECT_EQ(body_layout.dimensions.content.width, 100);
  EXPECT_EQ(body_layout.dimensions.margin.left, 50);
  EXPECT_EQ(body_layout.dimensions.margin.right, 50);
  EXPECT_EQ(body_layout.dimensions.content.x, 50);
  EXPECT_EQ(body_layout.dimensions.content.y, 0);
}

TEST(BlockLayout, SingleFlatBodyFixedWidthAutoMarginLeft)
{
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<body>"
                            "</body>";

  const char* css_source = "body {"
                           "  display: block;"
                           "  width: 100px;"
                           "  margin-left: auto"
                           "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox body_layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet),
      Dimensions(Rect(0.0, 0.0, 200.0, 0.0)));
  body_layout.calculate();

  EXPECT_EQ(body_layout.dimensions.content.width, 100);
  EXPECT_EQ(body_layout.dimensions.margin.left, 100);
  EXPECT_EQ(body_layout.dimensions.margin.right, 0);
}

TEST(BlockLayout, SingleFlatBodyOverflows)
{
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<body>"
                            "</body>";

  const char* css_source = "body {"
                           "  display: block;"
                           "  width: 400px;"
                           "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox body_layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet),
      Dimensions(Rect(0.0, 0.0, 200.0, 0.0)));
  body_layout.calculate();

  EXPECT_EQ(body_layout.dimensions.content.width, 400);
  EXPECT_EQ(body_layout.dimensions.margin.left, 0);
  EXPECT_EQ(body_layout.dimensions.margin.right, -200);
}

TEST(BlockLayout, FlatBodyChildrenFixedWidthAutoMargin)
{
  yahtml::HTMLDriver htmldriver;
  yacss::CSSDriver cssdriver;

  const char* html_source = "<body>"
                            "  <h1></h1>"
                            "  <h1></h1>"
                            "  <h1></h1>"
                            "</body>";

  const char* css_source = "body, h1 { display: block; }"
                           "h1 { height: 10px }"
                           "body {"
                           "  display: block;"
                           "  width: 100px;"
                           "}";

  htmldriver.parse_source(html_source);
  cssdriver.parse_source(css_source);
  ASSERT_EQ(htmldriver.result + cssdriver.result, 0);

  LayoutBox body_layout(
      std::make_shared<StyledNode>(htmldriver.dom, cssdriver.stylesheet),
      Dimensions(Rect(0.0, 0.0, 200.0, 0.0)));
  body_layout.calculate();

  EXPECT_EQ(body_layout.dimensions.content.width, 100);
  EXPECT_EQ(body_layout.dimensions.margin.left, 0);
  EXPECT_EQ(body_layout.dimensions.margin.right, 100);
  EXPECT_EQ(body_layout.dimensions.content.x, 0);
  EXPECT_EQ(body_layout.dimensions.content.y, 0);

  ASSERT_EQ(body_layout.children.size(), 3);
  const LayoutBoxPtr& h1_1 = body_layout.children.at(0);
  const LayoutBoxPtr& h1_2 = body_layout.children.at(1);
  const LayoutBoxPtr& h1_3 = body_layout.children.at(2);

  ASSERT_EQ(h1_1->styled_node->specified_values.size(), 2);
  ASSERT_EQ(h1_1->styled_node->get_value<LengthValue>("height")->type,
            ValueType::Length);
  ASSERT_EQ(h1_1->styled_node->get_value<LengthValue>("height")->val, 10.0);
  EXPECT_EQ(h1_1->dimensions.content.width, 100);
  EXPECT_EQ(h1_1->dimensions.content.height, 10);

  EXPECT_EQ(body_layout.dimensions.content.height, 30);
}
