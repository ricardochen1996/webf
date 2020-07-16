/*
 * Copyright (C) 2019 Alibaba Inc. All rights reserved.
 * Author: Kraken Team.
 */

#include "timer.h"
#include "dart_methods.h"
#include "foundation/bridge_callback.h"
#include "foundation/logging.h"
#include "jsa.h"

namespace kraken {
namespace binding {

using namespace alibaba::jsa;
using namespace kraken::foundation;

void handlePersistentCallback(void *callbackContext, int32_t contextIndex, const char *errmsg) {
  auto *obj = static_cast<BridgeCallback::Context *>(callbackContext);
  JSContext &_context = obj->_context;

  if (!BridgeCallback::checkContext(_context, contextIndex)) {
    return;
  }

  if (!_context.isValid()) return;

  if (_context.isFreeze()) {
    KRAKEN_LOG(ERROR) << "Failed to trigger callback: timer callback is null." << std::endl;
    return;
  }

  if (obj->_callback == nullptr) {
    // throw JSError inside of dart function callback will directly cause crash
    // so we handle it instead of throw
    JSError error(_context, "Failed to trigger callback: timer callback is null.");
    obj->_context.reportError(error);
    return;
  }

  if (errmsg != nullptr) {
    JSError error(_context, errmsg);
    obj->_context.reportError(error);
    return;
  }

  Object callback = obj->_callback->getObject(_context);
  callback.asFunction(_context).call(_context, Value::undefined(), 0);
}

void handleRAFPersistentCallback(void *callbackContext, int32_t contextIndex, double result,
                                 const char *errmsg) {
  auto *obj = static_cast<BridgeCallback::Context *>(callbackContext);
  JSContext &_context = obj->_context;

  if (!BridgeCallback::checkContext(_context, contextIndex)) {
    return;
  }

  if (!_context.isValid()) return;

  if (_context.isFreeze()) {
    KRAKEN_LOG(ERROR) << "Failed to trigger callback: context is freeze" << std::endl;
    return;
  }

  if (obj->_callback == nullptr) {
    // throw JSError inside of dart function callback will directly cause crash
    // so we handle it instead of throw
    JSError error(_context, "Failed to trigger callback: requestAnimationFrame callback is null.");
    obj->_context.reportError(error);
    return;
  }

  if (errmsg != nullptr) {
    JSError error(_context, errmsg);
    obj->_context.reportError(error);
    return;
  }

  Object callback = obj->_callback->getObject(_context);
  callback.asFunction(_context).call(_context, Value(result), 0);
}

void handleTransientCallback(void *callbackContext, int32_t contextIndex, const char *errmsg) {
  handlePersistentCallback(callbackContext, contextIndex, errmsg);
}

void handleRAFTransientCallback(void *callbackContext, int32_t contextIndex, double result,
                                const char *errmsg) {
  handleRAFPersistentCallback(callbackContext, contextIndex, result, errmsg);
}

Value setTimeout(JSContext &context, const Value &thisVal, const Value *args, size_t count) {
  if (count < 1) {
    throw JSError(context, "Failed to execute 'setTimeout': 1 argument required, but only 0 present.");
  }

  if (!args->isObject() || !args->getObject(context).isFunction(context)) {
    throw JSError(context, "Failed to execute 'setTimeout': parameter 1 (callback) must be a function.");
  }

  if (context.isFreeze()) {
    KRAKEN_LOG(ERROR) << "Failed to execute 'setTimeout': context is freeze" << std::endl;
    return Value::undefined();
  }

  std::shared_ptr<Value> callbackValue = std::make_shared<Value>(Value(context, args[0].getObject(context)));
  Object &&callbackFunction = callbackValue->getObject(context);

  auto &&time = args[1];
  int32_t timeout;

  if (time.isUndefined() || count < 2) {
    timeout = 0;
  } else if (time.isNumber()) {
    timeout = time.getNumber();
  } else {
    throw JSError(context, "Failed to execute 'setTimeout': parameter 2 (timeout) only can be a number or undefined.");
  }

  if (getDartMethod()->setTimeout == nullptr) {
    throw JSError(context, "Failed to execute 'setTimeout': dart method (setTimeout) is not registered.");
  }

  auto callbackContext = std::make_unique<BridgeCallback::Context>(context, callbackValue);
  auto timerId = BridgeCallback::instance()->registerCallback<int32_t>(
    std::move(callbackContext),
    [&timeout](BridgeCallback::Context *callbackContext, int32_t contextIndex) {
      return getDartMethod()->setTimeout(callbackContext, contextIndex, handleTransientCallback, timeout);
    });

  // `-1` represents ffi error occurred.
  if (timerId == -1) {
    throw JSError(context, "Failed to execute 'setTimeout': dart method (setTimeout) execute failed");
  }

  return Value(timerId);
}

Value setInterval(JSContext &context, const Value &thisVal, const Value *args, size_t count) {
  if (count < 1) {
    throw JSError(context, "Failed to execute 'setInterval': 1 argument required, but only 0 present.");
  }

  if (!args->isObject() || !args->getObject(context).isFunction(context)) {
    throw JSError(context, "Failed to execute 'setInterval': parameter 1 (callback) must be a function.");
  }

  if (context.isFreeze()) {
    KRAKEN_LOG(ERROR) << "Failed to execute 'setInterval': context is freeze" << std::endl;
    return Value::undefined();
  }

  std::shared_ptr<Value> callbackValue = std::make_shared<Value>(Value(context, args[0].getObject(context)));
  Object &&callbackFunction = callbackValue->getObject(context);

  if (!callbackFunction.isFunction(context)) {
    throw JSError(context, "Failed to execute 'setInterval': parameter 1 (callback) must be a function.");
  }

  auto &&time = args[1];
  int32_t delay;

  if (time.isUndefined()) {
    delay = 0;
  } else if (time.isNumber()) {
    delay = time.getNumber();
  } else {
    throw JSError(context, "Failed to execute 'setInterval': parameter 2 (timeout) only can be a number or undefined.");
  }

  if (getDartMethod()->setInterval == nullptr) {
    throw JSError(context, "Failed to execute 'setInterval': dart method (setInterval) is not registered.");
  }

  // the context pointer which will be pass by pointer address to dart code.
  auto callbackContext = std::make_unique<BridgeCallback::Context>(context, callbackValue);

  auto timerId = BridgeCallback::instance()->registerCallback<int32_t>(
    std::move(callbackContext),
    [&delay](BridgeCallback::Context *callbackContext, int32_t contextIndex) {
      return getDartMethod()->setInterval(callbackContext, contextIndex, handlePersistentCallback, delay);
    });

  if (timerId == -1) {
    throw JSError(context, "Failed to execute 'setInterval': dart method (setInterval) got unexpected error.");
  }

  return Value(timerId);
}

Value clearTimeout(JSContext &context, const Value &thisVal, const Value *args, size_t count) {
  if (count <= 0) {
    throw JSError(context, "Failed to execute 'clearTimeout': 1 argument required, but only 0 present.");
  }

  if (context.isFreeze()) {
    KRAKEN_LOG(ERROR) << "Failed to execute 'clearTimeout': context is freeze" << std::endl;
    return Value::undefined();
  }

  const Value &timerId = args[0];
  if (!timerId.isNumber()) {
    throw JSError(context, "Failed to execute 'clearTimeout': parameter 1  is not an timer kind.");
  }

  auto id = static_cast<int32_t>(timerId.asNumber());

  if (getDartMethod()->clearTimeout == nullptr) {
    throw JSError(context, "Failed to execute 'clearTimeout': dart method (clearTimeout) is not registered.");
  }

  getDartMethod()->clearTimeout(context.getContextIndex(), id);
  return Value::undefined();
}

Value cancelAnimationFrame(JSContext &context, const Value &thisVal, const Value *args, size_t count) {
  if (count <= 0) {
    throw JSError(context, "Failed to execute 'cancelAnimationFrame': 1 argument required, but only 0 present.");
  }

  if (context.isFreeze()) {
    KRAKEN_LOG(ERROR) << "Failed to execute 'cancelAnimationFrame': context is freeze" << std::endl;
    return Value::undefined();
  }

  const Value &requestId = args[0];
  if (!requestId.isNumber()) {
    throw JSError(context, "Failed to execute 'cancelAnimationFrame': parameter 1 (timer) is not a timer kind.");
  }

  auto id = static_cast<int32_t>(requestId.asNumber());

  if (getDartMethod()->cancelAnimationFrame == nullptr) {
    throw JSError(context,
                  "Failed to execute 'cancelAnimationFrame': dart method (cancelAnimationFrame) is not registered.");
  }

  getDartMethod()->cancelAnimationFrame(context.getContextIndex(), id);

  return Value::undefined();
}

Value requestAnimationFrame(JSContext &context, const Value &thisVal, const Value *args, size_t count) {
  if (count <= 0) {
    throw JSError(context, "Failed to execute 'requestAnimationFrame': 1 argument required, but only 0 present.");
  }

  if (!args[0].isObject() || !args[0].getObject(context).isFunction(context)) {
    throw JSError(context, "Failed to execute 'requestAnimationFrame': parameter 1 (callback) must be a function.");
  }

  if (context.isFreeze()) {
    KRAKEN_LOG(ERROR) << "Failed to execute 'requestAnimationFrame': context is freeze" << std::endl;
    return Value::undefined();
  }

  std::shared_ptr<Value> callbackValue = std::make_shared<Value>(Value(context, args[0].getObject(context)));
  Object &&callbackFunction = callbackValue->getObject(context);

  // the context pointer which will be pass by pointer address to dart code.
  auto callbackContext = std::make_unique<BridgeCallback::Context>(context, callbackValue);

  if (getDartMethod()->requestAnimationFrame == nullptr) {
    throw JSError(context,
                  "Failed to execute 'requestAnimationFrame': dart method (requestAnimationFrame) is not registered.");
  }

  int32_t requestId = BridgeCallback::instance()->registerCallback<int32_t>(
    std::move(callbackContext), [](BridgeCallback::Context *callbackContext, int32_t contextIndex) {
      return getDartMethod()->requestAnimationFrame(callbackContext, contextIndex,
                                                    handleRAFTransientCallback);
    });

  // `-1` represents some error occurred.
  if (requestId == -1) {
    throw JSError(context, "Failed to execute 'requestAnimationFrame': dart method (requestAnimationFrame) executed "
                           "with unexpected error.");
  }

  return Value(requestId);
}

void bindTimer(std::unique_ptr<JSContext> &context) {
  JSA_BINDING_FUNCTION(*context, context->global(), "setTimeout", 0, setTimeout);
  JSA_BINDING_FUNCTION(*context, context->global(), "setInterval", 0, setInterval);
  JSA_BINDING_FUNCTION(*context, context->global(), "__kraken_request_animation_frame__", 0, requestAnimationFrame);
  JSA_BINDING_FUNCTION(*context, context->global(), "clearTimeout", 0, clearTimeout);
  JSA_BINDING_FUNCTION(*context, context->global(), "clearInterval", 0, clearTimeout);
  JSA_BINDING_FUNCTION(*context, context->global(), "cancelAnimationFrame", 0, cancelAnimationFrame);
}

} // namespace binding
} // namespace kraken
