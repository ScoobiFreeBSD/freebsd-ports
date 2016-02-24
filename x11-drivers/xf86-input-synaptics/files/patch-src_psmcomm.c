--- src/psmcomm.c.orig	2014-08-28 22:04:49 UTC
+++ src/psmcomm.c
@@ -164,11 +164,18 @@ PSMReadHwState(InputInfoPtr pInfo,
     return PS2ReadHwStateProto(pInfo, &psm_proto_operations, comm, hwRet);
 }
 
+static Bool
+PSMAutoDevProbe(InputInfoPtr pInfo, const char *device)
+{
+    return pInfo && pInfo->name && !strcmp(pInfo->name, "PS/2 Mouse") &&
+           device && !strncmp(device, "/dev/psm", 8);
+}
+
 struct SynapticsProtocolOperations psm_proto_operations = {
     NULL,
     NULL,
     PSMQueryHardware,
     PSMReadHwState,
-    NULL,
+    PSMAutoDevProbe,
     NULL
 };
