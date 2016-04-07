# $FreeBSD$

PORTNAME=	cups-brother
PORTVERSION=	3.0.0
CATEGORIES=	print
MASTER_SITES=	http://www.brother.com/pub/bsc/linux/dlf/
DISTFILES=	${printer_model}_cupswrapper_GPL_source_3.0.0-1.tar.gz \
		${printer_model}lpr-3.0.0-1.i386.rpm
MAINTAINER=	Scoobi_doo@yahoo.com
COMMENT=	Brother print driver
WRKSRC=		${WRKDIR}/${printer_model}_cupswrapper_GPL_source_3.0.0-1
WRKSRC_SUBDIR=	brcupsconfpt1
MAKE_ARGS=	CC=${CC} CXX=${CXX} LINK=${CC}
PLIST_SUB=	PRINTER_MODEL=${printer_model}

RUN_DEPENDS=	psnup:${PORTSDIR}/print/psutils \
		a2ps:${PORTSDIR}/print/a2ps \
		/usr/local/etc/papersize:${PORTSDIR}/print/papersize-default-letter

printer_model=	mfcj450dw

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/brcupsconfpt1 ${STAGEDIR}${LOCALBASE}/bin/brcupsconfpt1
	${MKDIR} ${STAGEDIR}${LOCALBASE}/share/ppd/Brother/
	${INSTALL_DATA} ${WRKSRC}/../PPD/brother_${printer_model}_printer_en.ppd ${STAGEDIR}${LOCALBASE}/share/ppd/Brother/brother_${printer_model}_printer_en.ppd
	${MKDIR} ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/ImagingArea ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/ImagingArea
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/PaperDimension ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/PaperDimension
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/br${printer_model}func ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/br${printer_model}func
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/br${printer_model}rc ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/br${printer_model}rc
	${MKDIR} ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AA.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AA.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AC.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AC.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AD.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AD.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AE.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AE.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AF.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AF.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AG.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AG.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AH.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AH.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AI.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AI.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AK.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AK.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AL.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AL.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AM.BCM ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/lut/BRPRI13AM.BCM
	${INSTALL_DATA} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/paperinfij2 ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/paperinfij2
	${INSTALL_SCRIPT} ${WRKDIR}/opt/brother/Printers/${printer_model}/inf/setupPrintcapij ${STAGEDIR}/opt/brother/Printers/${printer_model}/inf/setupPrintcapij
	${MKDIR} ${STAGEDIR}/opt/brother/Printers/${printer_model}/lpd
	${INSTALL_PROGRAM} ${WRKDIR}/opt/brother/Printers/${printer_model}/lpd/br${printer_model}filter ${STAGEDIR}/opt/brother/Printers/${printer_model}/lpd/br${printer_model}filter
	${INSTALL_SCRIPT} ${WRKDIR}/opt/brother/Printers/${printer_model}/lpd/filter${printer_model} ${STAGEDIR}/opt/brother/Printers/${printer_model}/lpd/filter${printer_model}
	${INSTALL_SCRIPT} ${WRKDIR}/opt/brother/Printers/${printer_model}/lpd/psconvertij2 ${STAGEDIR}/opt/brother/Printers/${printer_model}/lpd/psconvertij2
	${MKDIR} ${STAGEDIR}${LOCALBASE}/libexec/cups/filter
	${INSTALL_SCRIPT} files/brother_lpdwrapper ${STAGEDIR}${LOCALBASE}/libexec/cups/filter/brother_lpdwrapper_${printer_model}

.include <bsd.port.mk>
