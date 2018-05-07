# Lumina Desktop Extras

Additional software to enchance the Lumina Desktop Enviroment.

## Applications

### lumina-screensaver-service

An D-Bus session daemon that implements the org.freedesktop.ScreenSaver specification for Lumina.

### lumina-disk-manager

An D-Bus/UDisks2(bsdisk) disk manager for Lumina.

### lumina-keyboard-manager

Enables users to set custom keyboard layout, variant and model in Lumina.

### lumina-power-manager

WORK-IN-PROGRESS D-Bus/UPower power manager for Lumina.

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
│           ├── lumina-disk-manager.desktop
│           ├── lumina-keyboard-loader.desktop
│           ├── lumina-power-manager.desktop
│           └── lumina-screensaver-service.desktop
└── usr
    ├── bin
    │   ├── lumina-disk-manager
    │   ├── lumina-keyboard-loader
    │   ├── lumina-keyboard-settings
    │   ├── lumina-power-manager
    │   └── lumina-screensaver-service
    └── share
        ├── applications
        │   └── lumina-keyboard-settings.desktop
        └── doc
            ├── lumina-disk-manager-1.0.0
            │   ├── LICENSE
            │   └── README.md
            ├── lumina-keyboard-loader-1.0.0
            │   ├── LICENSE
            │   └── README.md
            ├── lumina-keyboard-settings-1.0.0
            │   ├── LICENSE
            │   └── README.md
            ├── lumina-power-manager-0.1.0
            │   ├── LICENSE
            │   └── README.md
            └── lumina-screensaver-service-1.0.0
                ├── LICENSE
                └── README.md
```
 * The XDG destination can be customized with ``XDGDIR=``
 * The doc destination can be customized with ``DOCDIR=``
