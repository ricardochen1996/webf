/*
 * Copyright (C) 2019-2022 The Kraken authors. All rights reserved.
 * Copyright (C) 2022-present The WebF authors. All rights reserved.
 */
#ifndef BRIDGE_CORE_HTML_FORMS_HTML_TEXTAREA_ELEMENT_H_
#define BRIDGE_CORE_HTML_FORMS_HTML_TEXTAREA_ELEMENT_H_

#include "core/html/html_element.h"

namespace webf {

class HTMLTextareaElement : public HTMLElement {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit HTMLTextareaElement(Document&);

  bool IsAttributeDefinedInternal(const AtomicString& key) const override;
};

}  // namespace webf

#endif  // BRIDGE_CORE_HTML_FORMS_HTML_TEXTAREA_ELEMENT_H_
