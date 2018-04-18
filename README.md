# Lumina Desktop Extras

Additional applications to enchance the Lumina Desktop Enviroment.

## Applications

### lumina-screensaver-service

An D-Bus session daemon that implements the **org.freedesktop.ScreenSaver** specification for Lumina.

### lumina-disk-manager

An D-Bus/UDisks2(bsdisk) device manager for Lumina.

## Build

Clone or download this repository, then make sure you have Qt 4/5 and the D-Bus bindings installed.

Build and install to /usr/local:

```
mkdir build && cd build
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
│   └── xdg
│       └── autostart
│           ├── lumina-screensaver-service.desktop
│           └── lumina-disk-manager.desktop
└── usr
    ├── bin
    │   ├── lumina-screensaver-service
    │   └── lumina-disk-manager
    └── share
        └── doc
            ├── lumina-screensaver-service-VERSION
            │   ├── LICENSE
            │   └── README.md
            └── lumina-disk-manager-VERSION
                ├── LICENSE
                └── README.md
```
 * The XDG destination can be customized with ``XDGDIR=``
 * The doc destination can be customized with ``DOCDIR=``
