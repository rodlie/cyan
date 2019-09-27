PKG             := imagemagick7
$(PKG)_WEBSITE  := https://www.imagemagick.org/
$(PKG)_DESCR    := ImageMagick
$(PKG)_IGNORE   :=
$(PKG)_VERSION  := 7.0.8-20
$(PKG)_CHECKSUM := cb6192c829f2beb724990bb166180794c1fd811805101065a06ffc9437efbd81
$(PKG)_SUBDIR   := ImageMagick-$($(PKG)_VERSION)
$(PKG)_FILE     := ImageMagick-$($(PKG)_VERSION).tar.xz
$(PKG)_URL      := https://www.imagemagick.org/download/releases/$($(PKG)_FILE)
$(PKG)_URL_2    := https://ftp.sunet.se/pub/multimedia/graphics/ImageMagick/$($(PKG)_FILE)
$(PKG)_DEPS     := cc xz bzip2 jpeg lcms libpng tiff zlib pthreads openjpeg pango libwebp openexr
# librsvg libxml2
# libraw

define $(PKG)_UPDATE
    $(WGET) -q -O- 'https://www.imagemagick.org/' | \
    $(SED) -n 's,.*<p>The current release is ImageMagick \([0-9][0-9.-]*\).*,\1,p' | \
    head -1
endef

define $(PKG)_BUILD
    cd '$(1)' && autoreconf && LDFLAGS="-lws2_32 -lz" ./configure \
        $(MXE_CONFIGURE_OPTS) \
	--enable-zero-configuration \
	--enable-hdri \
	--enable-largefile \
	--disable-deprecated \
	--disable-pipes \
	--disable-docs \
	--disable-legacy-support \
	--with-package-release-name=Cyan \
	--with-utilities=no \
	--with-quantum-depth=16 \
	--with-bzlib=yes \
	--with-autotrace=no \
	--with-djvu=no \
	--with-dps=no \
	--with-fftw=no \
	--with-flif=no \
	--with-fpx=no \
	--with-fontconfig=no \
	--with-freetype=no \
	--with-gslib=no \
	--with-gvc=no \
	--with-heic=no \
	--with-jbig=no \
	--with-jpeg=yes \
	--with-lcms=yes \
	--with-lqr=no \
	--with-ltdl=no \
	--with-lzma=yes \
	--with-magick-plus-plus=yes \
	--with-openexr=yes \
	--with-openjp2=yes \
	--with-pango=yes \
        --with-librsvg=no \
	--with-perl=no \
	--with-png=yes \
	--with-raqm=no \
	--with-raw=no \
	--with-tiff=yes \
	--with-webp=yes \
	--with-wmf=no \
	--with-x=no \
	--with-xml=no \
	--with-zlib=yes \
	--with-zstd=no
    $(SED) -i 's/#define MAGICKCORE_ZLIB_DELEGATE 1//g' '$(1)/MagickCore/magick-config.h'
    $(MAKE) -C '$(1)' -j '$(JOBS)' bin_PROGRAMS=
    $(MAKE) -C '$(1)' -j 1 install bin_PROGRAMS=

endef
