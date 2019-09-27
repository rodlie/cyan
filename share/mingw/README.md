# Cyan Windows SDK

Components needed to setup a [MXE](https://mxe.cc) build environment that will create a Windows SDK for Cyan.

See [requirements](https://mxe.cc/#requirements) then run ``setup.sh`` from the source **root** directory.

```
cd <CYAN_SOURCE_DIRECTORY>
BUILD=1 sh share/mingw/setup.sh
```

This will build a complete SDK for Windows Vista+ x64 (only 7+ is tested).
