// swift-tools-version:5.3
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
  name: "RNFlexLayout",
  products: [
    .library(name: "RNFlexLayout", targets: ["RNFlexLayout"]),
    .library(name: "RNFlexLayoutYoga", targets: ["RNFlexLayoutYoga"]),
    .library(name: "RNFlexLayoutYogaKit", targets: ["RNFlexLayoutYogaKit"])
  ],
  dependencies: [
  ],
  targets: [
    .target(
      name: "RNFlexLayout",
      dependencies: ["RNFlexLayoutYoga", "RNFlexLayoutYogaKit"],
      path: "Sources/Swift",
      cSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ],
      cxxSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE"),
        .headerSearchPath("include/yoga/"),
        .headerSearchPath("./")
      ],
      swiftSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ]
    ),
    .target(
      name: "RNFlexLayoutYoga",
      dependencies: [],
      path: "Sources/yoga",
      cSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ],
      cxxSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE"),
        .headerSearchPath("include/yoga/"),
        .headerSearchPath("./")
      ]
    ),
    .target(
      name: "RNFlexLayoutYogaKit",
      dependencies: ["RNFlexLayoutYoga"],
      path: "Sources/YogaKit",
      cSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE")
      ],
      cxxSettings: [
        .define("FLEXLAYOUT_SWIFT_PACKAGE"),
        .headerSearchPath("include/YogaKit/"),
        .headerSearchPath("./")
      ]
    )
  ],
  cLanguageStandard: .gnu99,
  cxxLanguageStandard: .gnucxx11
)
