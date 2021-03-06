# Created by: Koop Mast <einekoai@chello.nl>
# $FreeBSD$

PORTNAME=	workrave
PORTVERSION=	1.10.15
CATEGORIES=	x11

MAINTAINER=	t@tobik.me
COMMENT=	RSI prevention tool

LICENSE=	GPLv3+

LIB_DEPENDS=	libgdome.so:textproc/gdome2

USES=	autoreconf:build gettext gmake libtool pathfix pkgconfig
USE_XORG=	ice recordproto sm x11 xext xmu xscrnsaver xtst
USE_GNOME=	intltool intlhack introspection gtkmm30
GNU_CONFIGURE=	yes
CONFIGURE_ARGS=	--disable-silent-rules \
		--disable-gsettings \
		--disable-pulse \
		--disable-gconf \
		--disable-mate \
		--disable-gnome2 \
		--disable-gnome3 \
		--disable-xfce \
		--disable-indicator
INSTALLS_ICONS=	yes
USE_LDCONFIG=	yes

USE_GITHUB=	yes
GH_ACCOUNT=	rcaelers
GH_TAGNAME=	v${PORTVERSION:S/./_/g}

CFLAGS+=	-I${LOCALBASE}/include
LDFLAGS+=	-L${LOCALBASE}/lib

INSTALL_TARGET=	install-strip

OPTIONS_DEFINE=		DBUS DISTRIBUTION GSTREAMER
OPTIONS_DEFAULT=	DBUS GSTREAMER
OPTIONS_SUB=		yes

DISTRIBUTION_DESC=	Build with networking support

DBUS_BUILD_DEPENDS=	${PYTHON_PKGNAMEPREFIX}cheetah>0:devel/py-cheetah
DBUS_CONFIGURE_ENABLE=	dbus
DBUS_USES=	python:2.7

DISTRIBUTION_CONFIGURE_ENABLE=	distribution

GSTREAMER_LIB_DEPENDS=	libgstreamer-1.0.so:multimedia/gstreamer1
GSTREAMER_CONFIGURE_ENABLE=	gstreamer
GSTREAMER_USE=		gstreamer1=good

# Remove gnome-shell cinnamon subdirs from build, so it doesn't install
# unused files
post-patch:
	@${REINPLACE_CMD} -e 's| gnome-shell cinnamon | |' \
		${WRKSRC}/frontend/applets/Makefile.am

# Run bundled autogen.sh instead of relying on USES=autoreconf which
# leads to errors like
# config.status: error: po/Makefile.in.in was not created by intltoolize.
pre-configure:
	@cd ${WRKSRC} && ./autogen.sh

.include <bsd.port.mk>
