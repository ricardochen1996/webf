/*
 * Copyright (C) 2019-2022 The Kraken authors. All rights reserved.
 * Copyright (C) 2022-present The WebF authors. All rights reserved.
 */
#include "script_state.h"
#include "defined_properties_initializer.h"
#include "event_factory.h"
#include "html_element_factory.h"
#include "names_installer.h"

namespace webf {

thread_local JSRuntime* runtime_ = nullptr;
thread_local std::atomic<int32_t> runningContexts{0};

ScriptState::ScriptState() {
  runningContexts++;
  bool first_loaded = false;
  if (runtime_ == nullptr) {
    runtime_ = JS_NewRuntime();
    first_loaded = true;
  }
  // Avoid stack overflow when running in multiple threads.
  JS_UpdateStackTop(runtime_);
  ctx_ = JS_NewContext(runtime_);

  if (first_loaded) {
    names_installer::Init(ctx_);
    DefinedPropertiesInitializer::Init();
    // Bump up the built-in classId. To make sure the created classId are larger than JS_CLASS_CUSTOM_CLASS_INIT_COUNT.
    for (int i = 0; i < JS_CLASS_CUSTOM_CLASS_INIT_COUNT - JS_CLASS_GC_TRACKER + 2; i++) {
      JSClassID id{0};
      JS_NewClassID(&id);
    }
  }
}

JSRuntime* ScriptState::runtime() {
  return runtime_;
}

ScriptState::~ScriptState() {
  ctx_invalid_ = true;
  JS_FreeContext(ctx_);

  // Run GC to clean up remaining objects about m_ctx;
  JS_RunGC(runtime_);

  if (--runningContexts == 0) {
    // Prebuilt strings stored in JSRuntime. Only needs to dispose when runtime disposed.
    DefinedPropertiesInitializer::Dispose();
    names_installer::Dispose();
    HTMLElementFactory::Dispose();
    EventFactory::Dispose();
    JS_FreeRuntime(runtime_);
    runtime_ = nullptr;
  }
  ctx_ = nullptr;
}
}  // namespace webf
