[![Build Status](https://travis-ci.org/rodlie/lumina-extra.svg?branch=master)](https://travis-ci.org/rodlie/lumina-extra)

# Lumina Desktop Extras

Additional software to enchance the Lumina Desktop Enviroment.

### Lumina disk manager

An UDisks disk manager and library for Lumina. Supports removable and optical devices.

### Lumina power manager

An UPower power manager for Lumina.

 * Implements org.freedesktop.PowerManagement session daemon
 * Implements org.freedesktop.ScreenSaver session daemon
 * Tray icon with battery percent
 * Supports lock screen, suspend, hibernate, shutdown (TODO)
 * Supports lid actions
 * Hibernate/Shutdown on cirtical battery
 * Auto sleep

### Lumina keyboard manager

Enables users to set custom keyboard layout, variant and model in Lumina.

## Build

Clone or download this repository, then make sure you have Qt 4/5, XSS (X11 Screen Saver extension client library) and the D-Bus bindings for Qt installed.

Build and install to /usr/local:

```
git clone https://github.com/rodlie/lumina-extra
cd lumina-extra
git submodule update -i --recursive
mkdir build && cd build
qmake CONFIG+=release .. && make
make install
```

Package for distribution:

```
qmake CONFIG+=release PREFIX=/usr
make
make INSTALL_ROOT=/path/to/package install
```
```
package/
├── etc
│   └── xdg
│       └── autostart
│           ├── lumina-disk-manager.desktop
│           ├── lumina-keyboard-loader.desktop
│           └── lumina-power-manager.desktop
└── usr
    ├── bin
    │   ├── lumina-disk-manager
    │   ├── lumina-keyboard-loader
    │   ├── lumina-keyboard-settings
    │   ├── lumina-power-manager
    │   └── lumina-power-settings
    └── share
        ├── applications
        │   ├── lumina-keyboard-settings.desktop
        │   └── lumina-power-settings.desktop
        └── doc
            ├── lumina-disk-manager-1.4.2
            │   ├── LICENSE
            │   └── README.md
            ├── lumina-keyboard-manager-1.4.2
            │   ├── LICENSE
            │   └── README.md
            └── lumina-power-manager-1.4.2
                ├── LICENSE
                └── README.md
```
 * The XDG destination can be customized with ``XDGDIR=``
 * The doc destination can be customized with ``DOCDIR=``
