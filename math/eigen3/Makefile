# $FreeBSD$

PORTNAME=	eigen
PORTVERSION=	3.2.9
CATEGORIES=	math
MASTER_SITES=	https://bitbucket.org/${PORTNAME}/${PORTNAME}/get/
DISTNAME=	${PORTVERSION}
DIST_SUBDIR=	${PORTNAME}

MAINTAINER=	kde@FreeBSD.org
COMMENT=	Lightweight library for vector and matrix math

LICENSE=	LGPL21 MPL
LICENSE_COMB=	multi

CMAKE_ARGS=	-DPKGCONFIG_INSTALL_DIR:PATH="${PREFIX}/libdata/pkgconfig"
USES=		cmake:outsource tar:bzip2
NO_BUILD=	yes

WRKSRC=		${WRKDIR}/${PORTNAME}-${PORTNAME}-dc6cfdf9bcec

.include <bsd.port.mk>
