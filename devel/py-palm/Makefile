# Created by: Veniamin Gvozdikov <vg@FreeBSD.org>
# $FreeBSD$

PORTNAME=	palm
PORTVERSION=	0.1.7
PORTREVISION=	1
CATEGORIES=	devel
MASTER_SITES=	CHEESESHOP
PKGNAMEPREFIX=	${PYTHON_PKGNAMEPREFIX}

MAINTAINER=	vg@FreeBSD.org
COMMENT=	Fast Protocol Buffer library for Python

LICENSE=	BSD2CLAUSE

BUILD_DEPENDS=	${PYTHON_PKGNAMEPREFIX}protobuf>0:devel/py-protobuf
RUN_DEPENDS=	${PYTHON_PKGNAMEPREFIX}simpleparse>0:devel/py-simpleparse

USES=		python:2
USE_PYTHON=	autoplist distutils

post-patch:
	@${REINPLACE_CMD} -e 's|"protobuf>=2.4.1,<=2.4.999"|"protobuf>=2.4.1"|' \
		${WRKSRC}/setup.py

post-install:
	@${STRIP_CMD} ${STAGEDIR}${PYTHON_SITELIBDIR}/palm/palm.so

.include <bsd.port.mk>
