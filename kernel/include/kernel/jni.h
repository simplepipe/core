#ifndef __kernel_jni_h
#define __kernel_jni_h

#include <jni.h>

JNIEnv *jni_get_env();

void jni_set_jvm(JavaVM *jvm);

#endif
