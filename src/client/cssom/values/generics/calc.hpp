#pragma once

#include <variant>

namespace client_cssom::values::generics
{
  template <typename L>
  class GenericCalcNode
  {
  private:
    enum Tag
    {
      kLeaf,
      kNegate,
      kInvert,
      kSum,
      kProduct,
      kMinMax,
      kClamp,
      kRound,
      kModRem,
      kHypot,
      kAbs,
      kSign,
      kAnchor,
      kAnchorSize
    };

    struct LeafVariant
    {
      L leaf;
    };
    struct NegateVariant
    {
      std::unique_ptr<GenericCalcNode<L>> node;
    };
    struct InvertVariant
    {
      std::unique_ptr<GenericCalcNode<L>> node;
    };

    using NodeVariant = std::variant<LeafVariant,
                                     NegateVariant,
                                     InvertVariant>;

  private:
    Tag tag_;
    NodeVariant node_;
  };
}
