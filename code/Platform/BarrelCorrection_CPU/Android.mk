LOCAL_PATH := $(call my-dir)

# Prebuild the 3rd libraries 
include $(CLEAR_VARS)
LOCAL_MODULE := LIB1
LOCAL_SRC_FILES := extralibs/libopenblas_armv7.a
include $(PREBUILT_STATIC_LIBRARY)

# Build native sdk 
include $(CLEAR_VARS)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/include/CL \
	$(LOCAL_PATH)/include/openblas
LOCAL_MODULE    := ISPCPUAlgFramework
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/ISPCPUVideoAlg.cpp)

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
LOCAL_SRC_FILES += $(call rwildcard,./module/,*.c)

#-mfpu=neon 使用 arm_neon.h 必须
#-mhard-float 使用 blas 必须
#-mfloat-abi=soft   || -mfloat-abi=softfp  -mfloat-abi=hard
#LOCAL_CFLAGS += -g -mfloat-abi=softfp -mfpu=neon -mhard-float -D_NDK_MATH_NO_SOFTFP=1  -D HAVE_CBLAS  -D HAVE_NEON #-march=armv7-a 
LOCAL_CFLAGS += -mfpu=neon -mhard-float -D RUN_ON_QUALC -D FAST_QUALC -O3 -fopenmp #-std=c99   -D HAVE_NEON -D HAVE_CBLAS -D FAST_QUALC
LOCAL_LDLIBS := -L$(LOCAL_PATH)/extralibs  -llog -lgomp
LOCAL_STATIC_LIBRARIES := LIB1 #LIB2
include $(BUILD_SHARED_LIBRARY)

# # Build tests for BC
include $(CLEAR_VARS)
LOCAL_C_INCLUDES += \
 	$(LOCAL_PATH)/include
LOCAL_MODULE    := CPUDemoBC
LOCAL_SRC_FILES := ./CPUAlgDemoBC.c
LOCAL_LDLIBS := -L$(LOCAL_PATH)/extralibs  #-lOpenCL #-lkeda_vida
LOCAL_CFLAGS += -pie -fPIE 
LOCAL_CFLAGS += -D RUN_ON_QUALC
LOCAL_LDFLAGS += -pie -fPIE
LOCAL_SHARED_LIBRARIES:= ISPCPUAlgFramework
include $(BUILD_EXECUTABLE)
