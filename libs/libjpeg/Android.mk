LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libjpeg

LOCAL_SRC_FILES := jcapimin.c \
				   jcapistd.c \
				   jccoefct.c \
				   jccolor.c \
				   jcdctmgr.c \
				   jchuff.c \
				   jcinit.c \
				   jcmainct.c \
				   jcmarker.c \
				   jcmaster.c \
				   jcomapi.c \
				   jcparam.c \
				   jcprepct.c \
				   jcsample.c \
				   jctrans.c \
				   jdapimin.c \
				   jdapistd.c \
				   jdatadst.c \
				   jdatasrc.c \
				   jdcoefct.c \
				   jdcolor.c \
				   jddctmgr.c \
				   jdhuff.c \
				   jdinput.c \
				   jdmainct.c \
				   jdmarker.c \
				   jdmaster.c \
				   jdmerge.c \
				   jdpostct.c \
				   jdsample.c \
				   jdtrans.c \
				   jerror.c \
				   jfdctflt.c \
				   jfdctfst.c \
				   jfdctint.c \
				   jidctflt.c \
				   jidctfst.c \
				   jidctint.c \
				   jquant1.c \
				   jquant2.c \
				   jutils.c \
				   jcphuff.c \
				   jdphuff.c \
				   jidctred.c \
				   jpegtran.c \
                   jmemmgr.c
#				   cdjpeg.c \
#				   cjpeg.c \
#				   ckconfig.c \
#				   djpeg.c \
#				   example.c
#				   rdbmp.c \
#				   rdcolmap.c \
#				   rdgif.c \
#				   rdjpgcom.c \
#				   rdppm.c \
#				   rdrle.c \
#				   rdswitch.c \
3				   rdtarga.c \
#				   transupp.c \
#				   wrbmp.c \
#				   wrgif.c \
#				   wrjpgcom.c \
#				   wrppm.c \
#				   wrrle.c \
#				   wrtarga.c

#				   ansi2knr.c
#				   jmemansi.c
#				   jmemdos.c
#				   jmemmac.c
#				   jmemname.c

# Use the no backing store memory manager provided by
# libjpeg. See install.txt
LOCAL_SRC_FILES += jmemnobs.c

LOCAL_LDLIBS := -lz

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
