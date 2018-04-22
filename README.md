# Lumina Desktop Extras

Additional applications to enchance the Lumina Desktop Enviroment.

## Applications

### lumina-screensaver-service

An D-Bus session daemon that implements the org.freedesktop.ScreenSaver specification for Lumina.

### lumina-disk-manager

An D-Bus/UDisks2(bsdisk) disk manager for Lumina.

### lumina-keyboard-manager

Enables users to set custom keyboard layout, variant and model in Lumina.

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
└── usr
    └── local
        ├── bin
        │   ├── lumina-disk-manager
        │   ├── lumina-keyboard-manager
        │   └── lumina-screensaver-service
        ├── etc
        │   └── xdg
        │       └── autostart
        │           ├── lumina-disk-manager.desktop
        │           ├── lumina-keyboard-manager.desktop
        │           └── lumina-screensaver-service.desktop
        └── share
            └── doc
                ├── lumina-disk-manager-VERSION
                │   ├── LICENSE
                │   └── README.md
                ├── lumina-keyboard-manager-VERSION
                │   ├── LICENSE
                │   └── README.md
                └── lumina-screensaver-service-VERSION
                    ├── LICENSE
                    └── README.md
```
 * The XDG destination can be customized with ``XDGDIR=``
 * The doc destination can be customized with ``DOCDIR=``
