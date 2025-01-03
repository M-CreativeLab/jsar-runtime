#pragma once

#include "./document.hpp"

namespace dom
{
  template <typename T>
    requires std::is_base_of_v<Document, T>
  std::shared_ptr<T> Document::As(std::shared_ptr<Document> document)
  {
    if (document != nullptr &&
        document->documentType == T::kDocumentType)
      return std::dynamic_pointer_cast<T>(document);
    else
      return nullptr;
  }
}
