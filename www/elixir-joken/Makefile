# $FreeBSD$

PORTNAME=	joken
PORTVERSION=	1.2.2
DISTVERSIONPREFIX=	v
CATEGORIES=	www
PKGNAMEPREFIX=	elixir-

MAINTAINER=	vanilla@FreeBSD.org
COMMENT=	JSON Web Token implementation in Elixir

LICENSE=	APACHE20

USES=		elixir
USE_GITHUB=	yes
GH_ACCOUNT=	bryanjos

MIX_BUILD_DEPS=	devel/elixir-plug \
		security/elixir-jose
MIX_RUN_DEPS:=	${MIX_BUILD_DEPS}

.include <bsd.port.mk>
