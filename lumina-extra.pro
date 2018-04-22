#
# lumina-extra
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

TEMPLATE = subdirs
CONFIG -= ordered

SUBDIRS += \
    lumina-screensaver-service \
    lumina-disk-manager \
    lumina-keyboard-manager

OTHER_FILES += lumina-extra.spec
