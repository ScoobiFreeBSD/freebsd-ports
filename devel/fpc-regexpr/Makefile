# Created by: John Merryweather Cooper et al
# $FreeBSD$

CATEGORIES=	devel lang
PKGNAMESUFFIX=	-regexpr

MAINTAINER=	freebsd-fpc@FreeBSD.org
COMMENT=	Free Pascal regular expression routines

USE_FPC=	rtl-objpas

MASTERDIR=	${.CURDIR}/../../lang/fpc
WRKUNITDIR=	${FPCSRCDIR}/packages/${PKGNAMESUFFIX:S/-//}
EXTRACTUNITDIR=	${WRKUNITDIR}

.include "${MASTERDIR}/Makefile"
