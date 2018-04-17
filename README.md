# Lumina Desktop Extras

Additional applications to enchance the Lumina Desktop Enviroment.

## Applications

### lumina-screensaver-service

An D-Bus session daemon that implements the **org.freedesktop.ScreenSaver** specification for Lumina.

### lumina-udisks-manager

An D-Bus/UDisks2 device manager for Lumina.

## Build

Clone or download this repository, then make sure you have Qt 4/5 and the D-Bus bindings installed.

Build and install to /usr/local:

```
mkdir build && build
qmake CONFIG+=release .. && make
make install
```

Package for distribution:

```
qmake CONFIG+=release PREFIX=/usr
make
make INSTALL_ROOT=/path/to/where/you/want/the/package/content install
```
```
package/
├── etc
│   └── xdg
│       └── autostart
│           ├── lumina-screensaver-service.desktop
│           └── lumina-udisks-manager.desktop
└── usr
    ├── bin
    │   ├── lumina-screensaver-service
    │   └── lumina-udisks-manager
    └── share
        └── doc
            ├── lumina-screensaver-service-1.0
            │   ├── LICENSE
            │   └── README.md
            └── lumina-udisks-manager-0.1
                ├── LICENSE
                └── README.md
```
 * The XDG destination can be customized with ``XDGDIR=``
 * The doc destination can be customized with ``DOCDIR=``
