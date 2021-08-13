/*
 * Copyright (C) 2021 Alibaba Inc. All rights reserved.
 * Author: Kraken Team.
 */

#include "text_node.h"
#include "document.h"
#include "kraken_bridge.h"

namespace kraken::binding::qjs {

std::once_flag kTextNodeInitFlag;

void bindTextNode(std::unique_ptr<JSContext> &context) {
  auto *constructor = TextNode::instance(context.get());
  context->defineGlobalProperty("Text", constructor->classObject);
}

JSClassID TextNode::kTextNodeClassId{0};

TextNode::TextNode(JSContext *context) :  Node(context, "TextNode") {
  std::call_once(kTextNodeInitFlag, []() {
    JS_NewClassID(&kTextNodeClassId);
  });
  JS_SetPrototype(m_ctx, m_prototypeObject, Node::instance(m_context)->prototype());
}

OBJECT_INSTANCE_IMPL(TextNode);

JSValue TextNode::constructor(QjsContext *ctx, JSValue func_obj, JSValue this_val, int argc, JSValue *argv) {
  JSValue textContent = JS_NULL;
  if (argc == 1) {
    textContent = argv[0];
  }

  return (new TextNodeInstance(this, textContent))->instanceObject;
}

JSClassID TextNode::classId() {
  return kTextNodeClassId;
}

PROP_GETTER(TextNodeInstance, data)(QjsContext *ctx, JSValue this_val, int argc, JSValue *argv) {
  auto *textNode = static_cast<TextNodeInstance *>(JS_GetOpaque(this_val, TextNode::classId()));
  return textNode->m_data;
}
PROP_SETTER(TextNodeInstance, data)(QjsContext *ctx, JSValue this_val, int argc, JSValue *argv) {return JS_NULL;}

PROP_GETTER(TextNodeInstance, textContent)(QjsContext *ctx, JSValue this_val, int argc, JSValue *argv) {
  auto *textNode = static_cast<TextNodeInstance *>(JS_GetOpaque(this_val, TextNode::classId()));
  return textNode->m_data;
}
PROP_SETTER(TextNodeInstance, textContent)(QjsContext *ctx, JSValue this_val, int argc, JSValue *argv) {return JS_NULL;}

PROP_GETTER(TextNodeInstance, nodeValue)(QjsContext *ctx, JSValue this_val, int argc, JSValue *argv) {
  auto *textNode = static_cast<TextNodeInstance *>(JS_GetOpaque(this_val, TextNode::classId()));
  return textNode->m_data;
}
PROP_SETTER(TextNodeInstance, nodeValue)(QjsContext *ctx, JSValue this_val, int argc, JSValue *argv) {
  return JS_NULL;
}

PROP_GETTER(TextNodeInstance, nodeName)(QjsContext *ctx, JSValue this_val, int argc, JSValue *argv) {
  return JS_NewString(ctx, "#text");
}
PROP_SETTER(TextNodeInstance, nodeName)(QjsContext *ctx, JSValue this_val, int argc, JSValue *argv) {
  return JS_NULL;
}

TextNodeInstance::TextNodeInstance(TextNode *textNode, JSValue text) : NodeInstance(textNode, NodeType::TEXT_NODE, DocumentInstance::instance(
  Document::instance(
    textNode->m_context)), TextNode::classId(), "TextNode"), m_data(JS_DupValue(m_ctx, text)) {
  NativeString *args_01 = jsValueToNativeString(m_ctx, m_data);
  foundation::UICommandBuffer::instance(m_context->getContextId())
    ->addCommand(eventTargetId, UICommand::createTextNode, *args_01, &nativeEventTarget);
}

TextNodeInstance::~TextNodeInstance() {
  JS_FreeValue(m_ctx, m_data);
}

JSValue TextNodeInstance::internalGetTextContent() {
  return m_data;
}
void TextNodeInstance::internalSetTextContent(JSValue content) {
  m_data = JS_DupValue(m_ctx, content);

  std::string key = "data";
  NativeString *args_01 = stringToNativeString(key);
  NativeString *args_02 = jsValueToNativeString(m_ctx, content);
  foundation::UICommandBuffer::instance(m_context->getContextId())
    ->addCommand(eventTargetId, UICommand::setProperty, *args_01, *args_02, nullptr);
}
}
