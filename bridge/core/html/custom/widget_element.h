/*
 * Copyright (C) 2022-present The WebF authors. All rights reserved.
 */

#ifndef WEBF_CORE_DOM_WIDGET_ELEMENT_H_
#define WEBF_CORE_DOM_WIDGET_ELEMENT_H_

#include <unordered_map>
#include "core/html/html_element.h"

namespace webf {

// All properties and methods from WidgetElement are defined in Dart side.
//
// There must be a corresponding Dart WidgetElement class implements the properties and methods with this element.
// The WidgetElement class in C++ is a wrapper and proxy all operations to the dart side.
class WidgetElement : public HTMLElement {
  DEFINE_WRAPPERTYPEINFO();

 public:
  using ImplType = WidgetElement*;
  WidgetElement(const AtomicString& tag_name, Document* document);

  static bool IsValidName(const AtomicString& name);
  static bool IsUnderScoreProperty(const AtomicString& name);

  bool NamedPropertyQuery(const AtomicString& key, ExceptionState& exception_state);
  void NamedPropertyEnumerator(std::vector<AtomicString>& names, ExceptionState&);

  ScriptValue item(const AtomicString& key, ExceptionState& exception_state);
  bool SetItem(const AtomicString& key, const ScriptValue& value, ExceptionState& exception_state);

  bool IsWidgetElement() const override;

  void CloneNonAttributePropertiesFrom(const Element&, CloneChildrenFlag) override;

  void Trace(GCVisitor* visitor) const override;
  bool IsAttributeDefinedInternal(const AtomicString& key) const override;

 private:
  std::unordered_map<AtomicString, ScriptValue, AtomicString::KeyHasher> unimplemented_properties_;
};

template <>
struct DowncastTraits<WidgetElement> {
  static bool AllowFrom(const Element& element) { return element.IsWidgetElement(); }
};

}  // namespace webf

#endif  // WEBF_CORE_DOM_WIDGET_ELEMENT_H_
