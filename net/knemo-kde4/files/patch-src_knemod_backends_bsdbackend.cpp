--- src/knemod/backends/bsdbackend.cpp.orig	2015-08-20 19:27:09 UTC
+++ src/knemod/backends/bsdbackend.cpp
@@ -43,6 +43,9 @@
 #include <net/if.h>
 #include <net/if_media.h>
 #include <netinet/in.h>
+#define __KAME_NETINET_IN_H_INCLUDED_
+#define _WANT_IFADDR
+#include <netinet6/in6.h>

 #include <net/if_dl.h>
 #include <net/if_types.h>
@@ -204,31 +207,45 @@ QString BSDBackend::getAddr( struct ifad
     }
     else if ( addrData.afType == AF_INET6 )
     {
-        struct in6_ifaddr *ifa6 = reinterpret_cast<struct in6_ifaddr *>(ifa);
-        if ( ifa6->ia6_flags & IN6_IFF_ANYCAST )
+        u_int32_t flags6 = 0;
+        const struct sockaddr_in6 * sin6 = NULL;
+        struct in6_ifreq ifr6;
+        int sock6;
+
+        sin6 = (const struct sockaddr_in6 *)ifa->ifa_addr;
+        if (sin6 != NULL) {
+            strncpy(ifr6.ifr_name, ifa->ifa_name, sizeof(ifr6.ifr_name));
+            ifr6.ifr_addr = *sin6;
+            if ((sock6 = socket(AF_INET6, SOCK_DGRAM, 0)) >= 0) {
+                if (ioctl(sock6, SIOCGIFAFLAG_IN6, &ifr6) == 0) {
+                    flags6 = ifr6.ifr_ifru.ifru_flags6;
+                }
+            }
+        }
+        if ( flags6 & IN6_IFF_ANYCAST )
             strFlags += i18n( " anycast" );
-        if ( ifa6->ia6_flags & IN6_IFF_TENTATIVE )
+        if ( flags6 & IN6_IFF_TENTATIVE )
             strFlags += i18n( " tentative" );
-        if ( ifa6->ia6_flags & IN6_IFF_DUPLICATED )
+        if ( flags6 & IN6_IFF_DUPLICATED )
             strFlags += i18n( " duplicate" );
-        if ( ifa6->ia6_flags & IN6_IFF_DETACHED )
+        if ( flags6 & IN6_IFF_DETACHED )
             strFlags += i18n( " detached" );
-        if ( ifa6->ia6_flags & IN6_IFF_DEPRECATED )
+        if ( flags6 & IN6_IFF_DEPRECATED )
             strFlags += i18n( " deprecated" );
-        if ( ifa6->ia6_flags & IN6_IFF_NODAD )
+        if ( flags6 & IN6_IFF_NODAD )
             strFlags += i18n( " nodad" );
-        if ( ifa6->ia6_flags & IN6_IFF_AUTOCONF )
+        if ( flags6 & IN6_IFF_AUTOCONF )
             strFlags += i18n( " autoconf" );
-        if ( ifa6->ia6_flags & IN6_IFF_TEMPORARY )
+        if ( flags6 & IN6_IFF_TEMPORARY )
             strFlags += i18n( " tempory" );
         addrData.ipv6Flags = strFlags;

         // Is this right?
-        if ( IN6_IS_ADDR_LOOPBACK( &ifa6->ia_addr.sin6_addr ) )
+        if ( IN6_IS_ADDR_LOOPBACK( &((const struct sockaddr_in6 *)(&ifa->ifa_addr))->sin6_addr ) )
             addrData.scope = RT_SCOPE_HOST;
-        else if ( IN6_IS_ADDR_LINKLOCAL( &ifa6->ia_addr.sin6_addr ) )
+        else if ( IN6_IS_ADDR_LINKLOCAL( &((const struct sockaddr_in6 *)(&ifa->ifa_addr))->sin6_addr ) )
             addrData.scope = RT_SCOPE_LINK;
-        else if ( IN6_IS_ADDR_SITELOCAL( &ifa6->ia_addr.sin6_addr ) )
+        else if ( IN6_IS_ADDR_SITELOCAL( &((const struct sockaddr_in6 *)(&ifa->ifa_addr))->sin6_addr ) )
             addrData.scope = RT_SCOPE_SITE;
         else
             addrData.scope = RT_SCOPE_UNIVERSE;
@@ -446,9 +463,11 @@ void BSDBackend::updateWirelessData( con
             break;
         case IEEE80211_M_STA:
             data->mode = i18n( "Managed" );
+	    break;
+	default:
+	    break;
     }

-    const uint8_t *cp;
     int len;

     union
@@ -468,7 +487,7 @@ void BSDBackend::updateWirelessData( con
     }

     if ( get80211len( ifName, IEEE80211_IOC_STA_INFO, &u, sizeof( u ), &len ) >= 0 &&
-         len >= sizeof( struct ieee80211req_sta_info )
+         (size_t)len >= sizeof( struct ieee80211req_sta_info )
        )
     {
         /*
