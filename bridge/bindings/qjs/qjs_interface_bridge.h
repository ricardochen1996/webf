/*
* Copyright (C) 2019-2022 The Kraken authors. All rights reserved.
* Copyright (C) 2022-present The WebF authors. All rights reserved.
*/

#ifndef KRAKENBRIDGE_BINDINGS_QJS_QJS_INTERFACE_BRIDGE_H_
#define KRAKENBRIDGE_BINDINGS_QJS_QJS_INTERFACE_BRIDGE_H_

#include "core/executing_context.h"
#include "script_wrappable.h"

namespace kraken {

template <class QJST, class T>
class QJSInterfaceBridge {
 public:
  static T* ToWrappable(ExecutingContext* context, JSValue value) {
    return HasInstance(context, value) ? toScriptWrappable<T>(value) : nullptr;
  }

  static bool HasInstance(ExecutingContext* context, JSValue value) {
    return JS_IsInstanceOf(context->ctx(), value,
                           context->contextData()->constructorForType(QJST::GetWrapperTypeInfo()));
  };
};

}  // namespace kraken

#endif  // KRAKENBRIDGE_BINDINGS_QJS_QJS_INTERFACE_BRIDGE_H_
