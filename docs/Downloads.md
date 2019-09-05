# Downloads

![SourceForge](https://img.shields.io/sourceforge/dm/prepress.svg) 
![GitHub Releases](https://img.shields.io/github/downloads/rodlie/cyan/latest/total.svg)
[![GitHub release](https://img.shields.io/github/release/rodlie/cyan.svg)](https://github.com/rodlie/cyan/releases)

# Release

Latest stable Cyan release:
 * [![Download for Windows](https://github.com/rodlie/cyan/raw/master/docs/images/download_for_windows.png)](https://github.com/rodlie/cyan/releases/latest) **Microsoft Windows 7/8/10 x64**
 * [![Download for Mac](https://github.com/rodlie/cyan/raw/master/docs/images/download_for_mac.png)](https://github.com/rodlie/cyan/releases/latest) **Apple Mac OS X 10.10-10.14**
 * [![Download for Linux](https://github.com/rodlie/cyan/raw/master/docs/images/download_for_linux.png)](https://github.com/rodlie/cyan/releases/latest) **GNU/Linux x86_64 (glibc 2.23+/libgcc 5.4+)**


# Snapshots

**WARNING: USE AT OWN RISK, INCLUDES EXPERIMENTAL CODE!**

Linux packages direct from the master branch for selected distributions:

* [CentOS 7](https://build.opensuse.org/package/binaries/home:rodlie/Cyan/CentOS_7)
* [Fedora 29](https://build.opensuse.org/package/binaries/home:rodlie/Cyan/Fedora_29)
* [Fedora 30](https://build.opensuse.org/package/binaries/home:rodlie/Cyan/Fedora_30)
* [Fedora RawHide](https://build.opensuse.org/package/binaries/home:rodlie/Cyan/Fedora_Rawhide)

*Selection is subject to change.*

## CentOS 7


```
cd /etc/yum.repos.d/
sudo wget https://download.opensuse.org/repositories/home:/rodlie/CentOS_7/home:rodlie.repo
sudo yum install cyan
```

## Fedora 29

```
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/rodlie/Fedora_29/home:rodlie.repo
sudo dnf install cyan
```

## Fedora 30

```
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/rodlie/Fedora_30/home:rodlie.repo
sudo dnf install cyan
```

## Fedora RawHide

```
sudo dnf config-manager --add-repo https://download.opensuse.org/repositories/home:/rodlie/Fedora_Rawhide/home:rodlie.repo
sudo dnf install cyan
```
