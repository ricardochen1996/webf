/*
 * Copyright (C) 2019-2022 The Kraken authors. All rights reserved.
 * Copyright (C) 2022-present The WebF authors. All rights reserved.
 */
#include "comment.h"
#include "built_in_string.h"
#include "document.h"
#include "tree_scope.h"

namespace webf {

Comment* Comment::Create(ExecutingContext* context, ExceptionState& exception_state) {
  return MakeGarbageCollected<Comment>(*context->document(), ConstructionType::kCreateOther);
}

Comment* Comment::Create(Document& document) {
  return MakeGarbageCollected<Comment>(document, ConstructionType::kCreateOther);
}

Comment::Comment(TreeScope& tree_scope, ConstructionType type)
    : CharacterData(tree_scope, built_in_string::kempty_string, type) {
  GetExecutingContext()->uiCommandBuffer()->addCommand(eventTargetId(), UICommand::kCreateComment,
                                                       (void*)bindingObject());
}

Node::NodeType Comment::nodeType() const {
  return Node::kCommentNode;
}
std::string Comment::nodeName() const {
  return "#comment";
}

Node* Comment::Clone(Document& factory, CloneChildrenFlag flag) const {
  Node* copy = Create(factory);
  std::unique_ptr<NativeString> args_01 = stringToNativeString(std::to_string(copy->eventTargetId()));
  GetExecutingContext()->uiCommandBuffer()->addCommand(eventTargetId(), UICommand::kCloneNode, std::move(args_01),
                                                       nullptr);
  return copy;
}

}  // namespace webf
