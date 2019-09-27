#JOBS := 4
MXE_TARGETS := x86_64-w64-mingw32.shared
MXE_PLUGIN_DIRS := plugins/gcc7
LOCAL_PKG_LIST := cc cmake qtbase fontconfig lcms pango imagemagick7 ffmpeg
.DEFAULT_GOAL := local-pkg-list
local-pkg-list: $(LOCAL_PKG_LIST)
