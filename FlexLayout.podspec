
Pod::Spec.new do |spec|
  spec.name         = "RNFlexLayout"
  spec.version      = "1.3.36"
  spec.summary      = "FlexLayout"
  spec.homepage     = "https://github.com/lucdion/FlexLayout.git"
  spec.license      = "MIT license"
  spec.author       = { "Luc Dion" => "luc_dion@yahoo.com" }

  spec.platform     = :ios, "12.0"
  spec.swift_versions = "5.0"
  spec.source       = { :git => "https://github.com/lj-mobile-team/FlexLayout.git", :tag => "#{spec.version}" }
  spec.source_files = "Sources/**/*.{swift,h,m,mm,cpp,c}"
  spec.public_header_files = "Sources/yoga/include/yoga/*.h", "Sources/YogaKit/include/YogaKit/*.h"
  spec.libraries    = 'c++'

  # Should match yoga_defs.bzl + BUCK configuration
  spec.compiler_flags = [
     '-fno-omit-frame-pointer',
     '-fexceptions',
     '-Wall',
     '-Werror',
     '-std=c++1y',
     '-fPIC'
  ]
end
