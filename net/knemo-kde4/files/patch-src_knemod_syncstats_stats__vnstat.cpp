--- src/knemod/syncstats/stats_vnstat.cpp.orig	2015-08-20 19:27:09 UTC
+++ src/knemod/syncstats/stats_vnstat.cpp
@@ -25,6 +25,7 @@

 #include <KProcess>

+#include <cstdlib>
 #ifndef __linux__
 #include <sys/sysctl.h>
 #endif
@@ -96,7 +97,7 @@ StatsPair StatsVnstat::addLagged( uint l
     if ( mExternalDays->rowCount() < 1 )
         return lag;

-    if ( abs(mSysBtime - mVnstatBtime) > 30 )
+    if ( std::abs(mSysBtime - mVnstatBtime) > 30 )
     {
         if ( mSysBtime > lastUpdated )
         {
