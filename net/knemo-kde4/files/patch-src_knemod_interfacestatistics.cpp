--- src/knemod/interfacestatistics.cpp.orig	2015-08-20 19:27:09 UTC
+++ src/knemod/interfacestatistics.cpp
@@ -1005,7 +1005,7 @@ void InterfaceStatistics::checkWarnings(
     QList<WarnRule> warn = mInterface->settings().warnRules;
     for ( int wi=0; wi < warn.count(); ++wi )
     {
-        if ( warn[wi].warnDone || !warn[wi].threshold > 0.0 )
+        if ( warn[wi].warnDone || !(warn[wi].threshold > 0.0) )
             continue;

         quint64 total = 0;
