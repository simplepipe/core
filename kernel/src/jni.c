#ifdef HAS_JNI
#include <pthread.h>
#include <kernel/thread.h>
#include <kernel/jni.h>

static JavaVM *g_jvm = NULL;
static pthread_key_t g_key;
static pthread_once_t g_key_once = PTHREAD_ONCE_INIT;

static void __init()
{
        pthread_key_create(&g_key, NULL);
}

static JNIEnv *__jni_create_env()
{
        JNIEnv* env;
        JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_4;
        args.name = NULL;
        args.group = NULL;
        (*g_jvm)->AttachCurrentThread(g_jvm, &env, &args);
        return env;
}

JNIEnv *jni_get_env()
{
        pthread_once(&g_key_once, __init);

        JNIEnv* env;
        if((env = pthread_getspecific(g_key)) == NULL) {
                env = __jni_create_env();
                pthread_setspecific(g_key, env);
        }

        return env;
}

void jni_set_jvm(JavaVM *jvm)
{
        g_jvm = jvm;
}
#endif
