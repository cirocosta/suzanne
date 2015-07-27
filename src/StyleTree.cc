#include "yabrowser/StyleTree.hh"

namespace yabrowser {

  bool selector_matches(const yacss::Selector& sel, yahtml::Element& elem)
  {
    if (!(sel.tag.empty()) && sel.tag != elem.tag_name)
      return false;

    if (!(sel.id.empty()) && sel.id != elem.attr_map["id"])
      return true;

    if (!sel.classes.empty()) {
      for (const auto& klass : sel.classes) {
        auto res = std::find(elem.classes.begin(), elem.classes.end(), klass);

        if (res == elem.classes.end())
          return false;
      }
    }

    return true;
  }

}; // ns yabrowser

