/*
 * Copyright (C) 2019-2022 The Kraken authors. All rights reserved.
 * Copyright (C) 2022-present The WebF authors. All rights reserved.
 */

import 'package:flutter/foundation.dart';
import 'package:webf/module.dart';
import 'package:webf/launcher.dart';

import 'binding.dart';
import 'from_native.dart';
import 'to_native.dart';

/// The maximum webf pages running in the same times.
/// Can be upgrade to larger amount if you have enough memory spaces.
int kWebFJSPagePoolSize = 1024;

bool _firstView = true;

/// Init bridge
int initBridge(WebFViewController view) {
  if (kProfileMode) {
    PerformanceTiming.instance().mark(PERF_BRIDGE_REGISTER_DART_METHOD_START);
  }

  // Setup binding bridge.
  BindingBridge.setup();

  if (kProfileMode) {
    PerformanceTiming.instance().mark(PERF_BRIDGE_REGISTER_DART_METHOD_END);
  }

  int contextId = -1;

  List<int> dartMethods = makeDartMethodsData();

  if (_firstView) {
    initJSPagePool(kWebFJSPagePoolSize, dartMethods);
    _firstView = false;
    contextId = 0;
  } else {
    contextId = allocateNewPage(dartMethods);
    if (contextId == -1) {
      throw Exception('Can\' allocate new webf bridge: bridge count had reach the maximum size.');
    }
  }

  return contextId;
}
