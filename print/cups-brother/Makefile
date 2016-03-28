# $FreeBSD$

PORTNAME=	cups-brother
PORTVERSION=	3.0.0
CATEGORIES=	print
MASTER_SITES=	http://www.brother.com/pub/bsc/linux/dlf/
DISTNAME=	mfcj450dw_cupswrapper_GPL_source_3.0.0-1
MAINTAINER=	Scoobi_doo@yahoo.com
COMMENT=	Brother print driver

WRKSRC_SUBDIR=	brcupsconfpt1
MAKE_ARGS=	CC=${CC} CXX=${CXX} LINK=${CC}

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/brcupsconfpt1 ${STAGEDIR}${LOCALBASE}/bin

.include <bsd.port.mk>

