# $FreeBSD$

PORTNAME=	libevhtp
PORTVERSION=	1.2.11
PORTREVISION=	2
CATEGORIES=	www

MAINTAINER=	ultima1252@gmail.com
COMMENT=	Flexible replacement for libevent httpd API

LICENSE=	BSD3CLAUSE
LICENSE_FILE=	${WRKSRC}/LICENSE

LIB_DEPENDS=	libevent.so:devel/libevent2

USE_GITHUB=	yes
GH_ACCOUNT=	ellzey

USES=		cmake:outsource
USE_LDCONFIG=	yes
CMAKE_ARGS=	-DCMAKE_INCLUDE_PATH:PATH=include/event2 \
		-DCMAKE_LIBRARY_PATH:PATH=lib/event2

PLIST_SUB+=	PORTVERSION=${PORTVERSION}

OPTIONS_DEFINE=	SSL THREADS STATIC DEBUG
OPTIONS_DEFAULT=SSL THREADS
OPTIONS_SUB=	yes

SSL_CMAKE_ON=		-DEVHTP_DISABLE_SSL:STRING=OFF
SSL_CMAKE_OFF=		-DEVHTP_DISABLE_SSL:STRING=ON
THREADS_CMAKE_ON=	-DEVHTP_DISABLE_EVTHR:STRING=OFF
THREADS_CMAKE_OFF=	-DEVHTP_DISABLE_EVTHR:STRING=ON
STATIC_CMAKE_ON=	-DEVHTP_BUILD_SHARED:STRING=OFF
STATIC_CMAKE_OFF=	-DEVHTP_BUILD_SHARED:STRING=ON

post-patch:
	@${REINPLACE_CMD} '/HAS_SYS_ONIG onig/d ; s/lib\/pkgconfig/libdata\/pkgconfig/ ; s/PATCH_VERSION [0-9]*/PATCH_VERSION ${PORTVERSION:S/1.2.//}/' \
		${WRKSRC}/CMakeLists.txt
	@${REINPLACE_CMD} 's/<evhtp-config.h>/"evhtp-config.h"/ ; s/<evthr.h>/"evthr.h"/ ; s/<htparse.h>/"htparse.h"/' \
	 ${WRKSRC}/evhtp.h ${WRKSRC}/evthr.h

post-build:
	@${REINPLACE_CMD} 's/<onigposix.h>/"onigposix.h"/' \
		${WRKSRC}/evhtp.h

.include <bsd.port.mk>
