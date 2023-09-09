/*
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "RNYGNodePrint.h"
#include <stdarg.h>
#include "RNYGEnums.h"
#include "RNYGNode.h"
#include "Yoga-internal.h"

namespace facebook {
namespace yoga {
typedef std::string string;

static void indent(string* base, uint32_t level) {
  for (uint32_t i = 0; i < level; ++i) {
    base->append("  ");
  }
}

static bool areFourValuesEqual(const std::array<RNYGValue, RNYGEdgeCount>& four) {
  return RNYGValueEqual(four[0], four[1]) && RNYGValueEqual(four[0], four[2]) &&
    RNYGValueEqual(four[0], four[3]);
}

static void appendFormatedString(string* str, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  va_list argsCopy;
  va_copy(argsCopy, args);
  std::vector<char> buf(1 + vsnprintf(NULL, 0, fmt, args));
  va_end(args);
  vsnprintf(buf.data(), buf.size(), fmt, argsCopy);
  va_end(argsCopy);
  string result = string(buf.begin(), buf.end() - 1);
  str->append(result);
}

static void
appendFloatIfNotUndefined(string* base, const string key, const float num) {
  if (!RNYGFloatIsUndefined(num)) {
    appendFormatedString(base, "%s: %g; ", key.c_str(), num);
  }
}

static void appendNumberIfNotUndefined(
    string* base,
    const string key,
    const RNYGValue number) {
  if (number.unit != RNYGUnitUndefined) {
    if (number.unit == RNYGUnitAuto) {
      base->append(key + ": auto; ");
    } else {
      string unit = number.unit == RNYGUnitPoint ? "px" : "%%";
      appendFormatedString(
          base, "%s: %g%s; ", key.c_str(), number.value, unit.c_str());
    }
  }
}

static void
appendNumberIfNotAuto(string* base, const string& key, const RNYGValue number) {
  if (number.unit != RNYGUnitAuto) {
    appendNumberIfNotUndefined(base, key, number);
  }
}

static void
appendNumberIfNotZero(string* base, const string& str, const RNYGValue number) {
  if (!RNYGFloatsEqual(number.value, 0)) {
    appendNumberIfNotUndefined(base, str, number);
  }
}

static void appendEdges(
    string* base,
    const string& key,
    const std::array<RNYGValue, RNYGEdgeCount>& edges) {
  if (areFourValuesEqual(edges)) {
    appendNumberIfNotZero(base, key, edges[RNYGEdgeLeft]);
  } else {
    for (int edge = RNYGEdgeLeft; edge != RNYGEdgeAll; ++edge) {
      string str = key + "-" + RNYGEdgeToString(static_cast<RNYGEdge>(edge));
      appendNumberIfNotZero(base, str, edges[edge]);
    }
  }
}

static void appendEdgeIfNotUndefined(
    string* base,
    const string& str,
    const std::array<RNYGValue, RNYGEdgeCount>& edges,
    const RNYGEdge edge) {
  appendNumberIfNotUndefined(
      base, str, *RNYGComputedEdgeValue(edges, edge, &RNYGValueUndefined));
}

void RNYGNodeToString(
    std::string* str,
                      RNYGNodeRef node,
                      RNYGPrintOptions options,
    uint32_t level) {
  indent(str, level);
  appendFormatedString(str, "<div ");
  if (node->getPrintFunc() != nullptr) {
    node->getPrintFunc()(node);
  }

  if (options & RNYGPrintOptionsLayout) {
    appendFormatedString(str, "layout=\"");
    appendFormatedString(
        str, "width: %g; ", node->getLayout().dimensions[RNYGDimensionWidth]);
    appendFormatedString(
        str, "height: %g; ", node->getLayout().dimensions[RNYGDimensionHeight]);
    appendFormatedString(
        str, "top: %g; ", node->getLayout().position[RNYGEdgeTop]);
    appendFormatedString(
        str, "left: %g;", node->getLayout().position[RNYGEdgeLeft]);
    appendFormatedString(str, "\" ");
  }

  if (options & RNYGPrintOptionsStyle) {
    appendFormatedString(str, "style=\"");
    if (node->getStyle().flexDirection != RNYGNode().getStyle().flexDirection) {
      appendFormatedString(
          str,
          "flex-direction: %s; ",
                           RNYGFlexDirectionToString(node->getStyle().flexDirection));
    }
    if (node->getStyle().justifyContent != RNYGNode().getStyle().justifyContent) {
      appendFormatedString(
          str,
          "justify-content: %s; ",
                           RNYGJustifyToString(node->getStyle().justifyContent));
    }
    if (node->getStyle().alignItems != RNYGNode().getStyle().alignItems) {
      appendFormatedString(
          str,
          "align-items: %s; ",
                           RNYGAlignToString(node->getStyle().alignItems));
    }
    if (node->getStyle().alignContent != RNYGNode().getStyle().alignContent) {
      appendFormatedString(
          str,
          "align-content: %s; ",
                           RNYGAlignToString(node->getStyle().alignContent));
    }
    if (node->getStyle().alignSelf != RNYGNode().getStyle().alignSelf) {
      appendFormatedString(
          str, "align-self: %s; ", RNYGAlignToString(node->getStyle().alignSelf));
    }
    appendFloatIfNotUndefined(str, "flex-grow", node->getStyle().flexGrow);
    appendFloatIfNotUndefined(str, "flex-shrink", node->getStyle().flexShrink);
    appendNumberIfNotAuto(str, "flex-basis", node->getStyle().flexBasis);
    appendFloatIfNotUndefined(str, "flex", node->getStyle().flex);

    if (node->getStyle().flexWrap != RNYGNode().getStyle().flexWrap) {
      appendFormatedString(
          str, "flexWrap: %s; ", RNYGWrapToString(node->getStyle().flexWrap));
    }

    if (node->getStyle().overflow != RNYGNode().getStyle().overflow) {
      appendFormatedString(
          str, "overflow: %s; ", RNYGOverflowToString(node->getStyle().overflow));
    }

    if (node->getStyle().display != RNYGNode().getStyle().display) {
      appendFormatedString(
          str, "display: %s; ", RNYGDisplayToString(node->getStyle().display));
    }
    appendEdges(str, "margin", node->getStyle().margin);
    appendEdges(str, "padding", node->getStyle().padding);
    appendEdges(str, "border", node->getStyle().border);

    appendNumberIfNotAuto(
        str, "width", node->getStyle().dimensions[RNYGDimensionWidth]);
    appendNumberIfNotAuto(
        str, "height", node->getStyle().dimensions[RNYGDimensionHeight]);
    appendNumberIfNotAuto(
        str, "max-width", node->getStyle().maxDimensions[RNYGDimensionWidth]);
    appendNumberIfNotAuto(
        str, "max-height", node->getStyle().maxDimensions[RNYGDimensionHeight]);
    appendNumberIfNotAuto(
        str, "min-width", node->getStyle().minDimensions[RNYGDimensionWidth]);
    appendNumberIfNotAuto(
        str, "min-height", node->getStyle().minDimensions[RNYGDimensionHeight]);

    if (node->getStyle().positionType != RNYGNode().getStyle().positionType) {
      appendFormatedString(
          str,
          "position: %s; ",
                           RNYGPositionTypeToString(node->getStyle().positionType));
    }

    appendEdgeIfNotUndefined(
        str, "left", node->getStyle().position, RNYGEdgeLeft);
    appendEdgeIfNotUndefined(
        str, "right", node->getStyle().position, RNYGEdgeRight);
    appendEdgeIfNotUndefined(str, "top", node->getStyle().position, RNYGEdgeTop);
    appendEdgeIfNotUndefined(
        str, "bottom", node->getStyle().position, RNYGEdgeBottom);
    appendFormatedString(str, "\" ");

    if (node->getMeasure() != nullptr) {
      appendFormatedString(str, "has-custom-measure=\"true\"");
    }
  }
  appendFormatedString(str, ">");

  const uint32_t childCount = static_cast<uint32_t>(node->getChildren().size());
  if (options & RNYGPrintOptionsChildren && childCount > 0) {
    for (uint32_t i = 0; i < childCount; i++) {
      appendFormatedString(str, "\n");
        RNYGNodeToString(str, RNYGNodeGetChild(node, i), options, level + 1);
    }
    appendFormatedString(str, "\n");
    indent(str, level);
  }
  appendFormatedString(str, "</div>");
}
} // namespace yoga
} // namespace facebook
