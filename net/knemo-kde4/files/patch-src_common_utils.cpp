--- src/common/utils.cpp.orig	2015-08-20 19:27:09 UTC
+++ src/common/utils.cpp
@@ -233,7 +233,7 @@ QString getSocketRoute( int afType, QStr
 }
 #endif

-QString getDefaultRoute( int afType, QString *defaultGateway, void *data )
+QString getDefaultRoute( int afType, QString *defaultGateway, void *data  __attribute__((unused)))
 {
 #ifdef __linux__
     return getNetlinkRoute( afType, defaultGateway, data );
