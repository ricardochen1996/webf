import 'dart:async';
import 'dart:ffi';
import 'dart:convert';
import 'package:ffi/ffi.dart';
import 'package:kraken/element.dart';

import 'from_native.dart';
import 'platform.dart';

// Steps for using dart:ffi to call a C function from Dart:
// 1. Import dart:ffi.
// 2. Create a typedef with the FFI type signature of the C function.
// 3. Create a typedef for the variable that you’ll use when calling the C function.
// 4. Open the dynamic library that contains the C function.
// 5. Get a reference to the C function, and put it into a variable.
// 6. Call the C function.

// representation of JSContext
class JSCallbackContext extends Struct {}

// Register invokeEventListener
typedef Native_InvokeEventListener = Void Function(Int32 contextIndex, Int32 type, Pointer<Utf8>);
typedef Dart_InvokeEventListener = void Function(int contextIndex, int type, Pointer<Utf8>);

final Dart_InvokeEventListener _invokeEventListener =
    nativeDynamicLibrary.lookup<NativeFunction<Native_InvokeEventListener>>('invokeEventListener').asFunction();

void invokeEventListener(int contextIndex, int type, String data) {
  _invokeEventListener(contextIndex, type, Utf8.toUtf8(data));
}

const UI_EVENT = 0;
const MODULE_EVENT = 1;

void emitUIEvent(int contextIndex, String data) {
  invokeEventListener(contextIndex, UI_EVENT, data);
}

void emitModuleEvent(int contextIndex, String data) {
  invokeEventListener(contextIndex, MODULE_EVENT, data);
}

void invokeOnPlatformBrightnessChangedCallback(int contextIndex) {
  String json = jsonEncode([WINDOW_ID, Event('colorschemechange')]);
  emitUIEvent(contextIndex, json);
}

// Register createScreen
typedef Native_CreateScreen = Pointer<ScreenSize> Function(Double, Double);
typedef Dart_CreateScreen = Pointer<ScreenSize> Function(double, double);

final Dart_CreateScreen _createScreen =
    nativeDynamicLibrary.lookup<NativeFunction<Native_CreateScreen>>('createScreen').asFunction();

Pointer<ScreenSize> createScreen(double width, double height) {
  return _createScreen(width, height);
}

// Register evaluateScripts
typedef Native_EvaluateScripts = Void Function(Int32 contextIndex, Pointer<Utf8> code, Pointer<Utf8> url, Int32 startLine);
typedef Dart_EvaluateScripts = void Function(int contextIndex, Pointer<Utf8> code, Pointer<Utf8> url, int startLine);

final Dart_EvaluateScripts _evaluateScripts =
    nativeDynamicLibrary.lookup<NativeFunction<Native_EvaluateScripts>>('evaluateScripts').asFunction();

void evaluateScripts(int contextIndex, String code, String url, int line) {
  Pointer<Utf8> _code = Utf8.toUtf8(code);
  Pointer<Utf8> _url = Utf8.toUtf8(url);
  try {
    _evaluateScripts(contextIndex, _code, _url, line);
  } catch (e, stack) {
    print('$e\n$stack');
  }
}

// Register initJsEngine
typedef Native_InitJSBridgePool = Void Function(Int32 poolSize);
typedef Dart_InitJSBridgePool = void Function(int poolSize);

final Dart_InitJSBridgePool _initJSBridgePool =
    nativeDynamicLibrary.lookup<NativeFunction<Native_InitJSBridgePool>>('initJSBridgePool').asFunction();

void initJSBridgePool(int poolSize) {
  _initJSBridgePool(poolSize);
}

typedef Native_DisposeBridge = Void Function(Int32 contextIndex);
typedef Dart_DisposeBridge = void Function(int contextIndex);

final Dart_DisposeBridge _disposeBridge =
    nativeDynamicLibrary.lookup<NativeFunction<Native_DisposeBridge>>('disposeBridge').asFunction();

void disposeBridge(int contextIndex) {
  _disposeBridge(contextIndex);
}

typedef Native_AllocateNewBridge = Int32 Function();
typedef Dart_AllocateNewBridge = int Function();

final Dart_AllocateNewBridge _allocateNewBridge =
    nativeDynamicLibrary.lookup<NativeFunction<Native_AllocateNewBridge>>('allocateNewBridge').asFunction();

int allocateNewBridge() {
  return _allocateNewBridge();
}

typedef Native_FreezeContext = Void Function(Int32 contextIndex);
typedef Dart_FreezeContext = void Function(int contextIndex);

final Dart_FreezeContext _freezeBridge =
    nativeDynamicLibrary.lookup<NativeFunction<Native_FreezeContext>>('freezeBridge').asFunction();

void freezeContext(int contextIndex) {
  _freezeBridge(contextIndex);
}

typedef Native_UnFreezeContext = Void Function(Int32 contextIndex);
typedef Dart_UnFreezeContext = void Function(int contextIndex);

final Dart_UnFreezeContext _unfreezeContext =
    nativeDynamicLibrary.lookup<NativeFunction<Native_UnFreezeContext>>('unfreezeContext').asFunction();

void unfreezeContext(int contextIndex) {
  _unfreezeContext(contextIndex);
}

// Register reloadJsContext
typedef Native_ReloadJSContext = Void Function(Int32 contextIndex);
typedef Dart_ReloadJSContext = void Function(int contextIndex);

final Dart_ReloadJSContext _reloadJSContext =
    nativeDynamicLibrary.lookup<NativeFunction<Native_ReloadJSContext>>('reloadJsContext').asFunction();

void reloadJSContext(int contextIndex) async {
  Completer completer = Completer<void>();
  Future.microtask(() {
    _reloadJSContext(contextIndex);
    completer.complete();
  });
  return completer.future;
}
