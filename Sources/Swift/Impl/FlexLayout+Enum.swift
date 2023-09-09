// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Created by Luc Dion on 2017-07-17.

import UIKit

#if FLEXLAYOUT_SWIFT_PACKAGE
import FlexLayoutYoga

extension RNYGFlexDirection {
    static let column = RNYGFlexDirectionColumn
    static let columnReverse = RNYGFlexDirectionColumnReverse
    static let row = RNYGFlexDirectionRow
    static let rowReverse = RNYGFlexDirectionRowReverse
}

extension RNYGJustify {
    static let flexStart = RNYGJustifyFlexStart
    static let center = RNYGJustifyCenter
    static let flexEnd = RNYGJustifyFlexEnd
    static let spaceBetween = RNYGJustifySpaceBetween
    static let spaceAround = RNYGJustifySpaceAround
    static let spaceEvenly = RNYGJustifySpaceEvenly
}

extension RNYGAlign {
    static let auto = RNYGAlignAuto
    static let baseline = RNYGAlignBaseline
    static let stretch = RNYGAlignStretch
    static let flexStart = RNYGAlignFlexStart
    static let center = RNYGAlignCenter
    static let flexEnd = RNYGAlignFlexEnd
    static let spaceBetween = RNYGAlignSpaceBetween
    static let spaceAround = RNYGAlignSpaceAround
}

extension RNYGWrap {
    static let noWrap = RNYGWrapNoWrap
    static let wrap = RNYGWrapWrap
    static let wrapReverse = RNYGWrapWrapReverse
}

extension RNYGPositionType {
    static let relative = RNYGPositionTypeRelative
    static let absolute = RNYGPositionTypeAbsolute
}

extension RNYGDirection {
    static let LTR = RNYGDirectionLTR
    static let RTL = RNYGDirectionRTL
    static let inherit = RNYGDirectionInherit
}

extension RNYGDisplay {
    static let flex = RNYGDisplayFlex
    static let none = RNYGDisplayNone
}

extension RNYGUnit {
    static let percent = RNYGUnitPercent
    static let point = RNYGUnitPoint
}
#endif

extension Flex.Direction {
    var yogaValue: RNYGFlexDirection {
        switch self {
        case .column:        return RNYGFlexDirection.column
        case .columnReverse: return RNYGFlexDirection.columnReverse
        case .row:           return RNYGFlexDirection.row
        case .rowReverse:    return RNYGFlexDirection.rowReverse
        }
    }
}

extension Flex.JustifyContent {
    var yogaValue: RNYGJustify {
        switch self {
        case .start:        return RNYGJustify.flexStart
        case .center:       return RNYGJustify.center
        case .end:          return RNYGJustify.flexEnd
        case .spaceBetween: return RNYGJustify.spaceBetween
        case .spaceAround:  return RNYGJustify.spaceAround
        case .spaceEvenly:  return RNYGJustify.spaceEvenly
        }
    }
}

extension Flex.AlignContent {
    var yogaValue: RNYGAlign {
        switch self {
        case .stretch:      return RNYGAlign.stretch
        case .start:        return RNYGAlign.flexStart
        case .center:       return RNYGAlign.center
        case .end:          return RNYGAlign.flexEnd
        case .spaceBetween: return RNYGAlign.spaceBetween
        case .spaceAround:  return RNYGAlign.spaceAround
        }
    }
}

extension Flex.AlignItems {
    var yogaValue: RNYGAlign {
        switch self {
        case .stretch:      return RNYGAlign.stretch
        case .start:        return RNYGAlign.flexStart
        case .center:       return RNYGAlign.center
        case .end:          return RNYGAlign.flexEnd
        case .baseline:     return RNYGAlign.baseline
        }
    }
}

extension Flex.AlignSelf {
    var yogaValue: RNYGAlign {
        switch self {
        case .auto:         return RNYGAlign.auto
        case .stretch:      return RNYGAlign.stretch
        case .start:        return RNYGAlign.flexStart
        case .center:       return RNYGAlign.center
        case .end:          return RNYGAlign.flexEnd
        case .baseline:     return RNYGAlign.baseline
        }
    }
}

extension Flex.Wrap {
    var yogaValue: RNYGWrap {
        switch self {
        case .noWrap:      return RNYGWrap.noWrap
        case .wrap:        return RNYGWrap.wrap
        case .wrapReverse: return RNYGWrap.wrapReverse
        }
    }
}

extension Flex.Position {
    var yogaValue: RNYGPositionType {
        switch self {
        case .relative: return RNYGPositionType.relative
        case .absolute: return RNYGPositionType.absolute
        }
    }
}

extension Flex.LayoutDirection {
    var yogaValue: RNYGDirection {
        switch self {
        case .ltr: return RNYGDirection.LTR
        case .rtl: return RNYGDirection.RTL
        default:   return RNYGDirection.inherit
        }
    }
}

extension Flex.Display {
    var yogaValue: RNYGDisplay {
        switch self {
        case .flex: return RNYGDisplay.flex
        case .none: return RNYGDisplay.none
        }
    }
}

/*extension Flex.Overflow {
    var yogaValue: YGOverflow {
        switch self {
        case .visible: return YGOverflow.visible
        case .hidden:  return YGOverflow.hidden
        case .scroll:  return YGOverflow.scroll
        }
    }
}*/
