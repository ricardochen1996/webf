/*
 * Copyright (C) 2019-2022 The Kraken authors. All rights reserved.
 * Copyright (C) 2022-present The WebF authors. All rights reserved.
 */
#include "module_callback.h"

namespace webf {

std::shared_ptr<ModuleCallback> ModuleCallback::Create(std::shared_ptr<QJSFunction> function) {
  return std::make_shared<ModuleCallback>(function);
}

ModuleCallback::ModuleCallback(std::shared_ptr<QJSFunction> function) : function_(function) {}

std::shared_ptr<QJSFunction> ModuleCallback::value() {
  return function_;
}

}  // namespace webf
