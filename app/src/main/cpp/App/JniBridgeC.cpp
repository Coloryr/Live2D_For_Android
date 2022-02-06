/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include <jni.h>
#include "JniBridgeC.hpp"
#include "LAppDelegate.hpp"
#include "LAppPal.hpp"
#include "LAppView.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppModel.hpp"
#include <CubismFramework.hpp>
#include <string.h>

using namespace Csm;

static JavaVM *g_JVM; // JavaVM is valid for all threads, so just save it globally
static jclass g_JniBridgeJavaClass;
static jmethodID g_LoadFileMethodId;
static jmethodID g_OnLoadModelMethodId;

JNIEnv *GetEnv()
{
    JNIEnv *env = NULL;
    g_JVM->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    return env;
}

// The VM calls JNI_OnLoad when the native library is loaded
jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    g_JVM = vm;

    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return JNI_ERR;
    }

    jclass clazz = env->FindClass("com/live2d/demo/JniBridgeJava");
    g_JniBridgeJavaClass = reinterpret_cast<jclass>(env->NewGlobalRef(clazz));
    g_LoadFileMethodId = env->GetStaticMethodID(g_JniBridgeJavaClass, "LoadFile", "(Ljava/lang/String;)[B");
    g_OnLoadModelMethodId = env->GetStaticMethodID(g_JniBridgeJavaClass, "onLoadModel", "(Ljava/lang/String;)V");

    return JNI_VERSION_1_6;
}

void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
    JNIEnv *env = GetEnv();
    env->DeleteGlobalRef(g_JniBridgeJavaClass);
}

char *JniBridgeC::LoadFileAsBytesFromJava(const char *filePath, unsigned int *outSize)
{
    JNIEnv *env = GetEnv();

    // ファイルロード
    jbyteArray obj = (jbyteArray)env->CallStaticObjectMethod(g_JniBridgeJavaClass, g_LoadFileMethodId, env->NewStringUTF(filePath));
    *outSize = static_cast<unsigned int>(env->GetArrayLength(obj));

    char *buffer = new char[*outSize];
    env->GetByteArrayRegion(obj, 0, *outSize, reinterpret_cast<jbyte *>(buffer));

    return buffer;
}

void JniBridgeC::OnLoadModel(char *name)
{
    JNIEnv *env = GetEnv();
    jstring str_arg = env->NewStringUTF(name);
    env->CallStaticVoidMethod(g_JniBridgeJavaClass, g_OnLoadModelMethodId, str_arg);
    env->DeleteLocalRef(str_arg);
}

extern "C"
{
    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeEnableRandomMotion(JNIEnv *env, jclass type, jboolean open)
    {
        LAppLive2DManager *l2d = LAppLive2DManager::GetInstance();
        LAppModel *model = l2d->GetModel();
        if (model)
        {
            model->SetEnableRandomMotion(open == JNI_TRUE);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeSetBreath(JNIEnv *env, jclass type, jbyteArray id)
    {
        LAppLive2DManager *l2d = LAppLive2DManager::GetInstance();
        LAppModel *model = l2d->GetModel();

        char *chars = NULL;
        jbyte *bytes;
        int chars_len;

        bytes = env->GetByteArrayElements(id, 0);
        chars_len = env->GetArrayLength(id);
        chars = new char[chars_len + 1];
        memset(chars, 0, chars_len + 1);
        memcpy(chars, bytes, chars_len);
        chars[chars_len] = 0;

        Csm::csmString id1 = Csm::csmString(chars);

        env->ReleaseByteArrayElements(id, bytes, 0);
        delete[] chars;

        if (model)
        {
            model->SetBreath(id1);
        }
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeLoadModel(JNIEnv *env, jclass type, jbyteArray path, jbyteArray name)
    {
        LAppLive2DManager *l2d = LAppLive2DManager::GetInstance();

        char *chars = NULL;
        jbyte *bytes;
        int chars_len;

        bytes = env->GetByteArrayElements(path, 0);
        chars_len = env->GetArrayLength(path);
        chars = new char[chars_len + 1];
        memset(chars, 0, chars_len + 1);
        memcpy(chars, bytes, chars_len);
        chars[chars_len] = 0;

        Csm::csmString path1 = Csm::csmString(chars);

        env->ReleaseByteArrayElements(path, bytes, 0);
        delete[] chars;

        bytes = env->GetByteArrayElements(name, 0);
        chars_len = env->GetArrayLength(name);
        chars = new char[chars_len + 1];
        memset(chars, 0, chars_len + 1);
        memcpy(chars, bytes, chars_len);
        chars[chars_len] = 0;

        Csm::csmString name1 = Csm::csmString(chars);

        env->ReleaseByteArrayElements(name, bytes, 0);
        delete[] chars;

        l2d->LoadModel(path1, name1);
    }

    JNIEXPORT jbyteArray JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeGetExpression(JNIEnv *env, jclass type, jint index)
    {
        LAppLive2DManager *l2d = LAppLive2DManager::GetInstance();
        LAppModel *model = l2d->GetModel();

        Csm::csmString data1 = model->GetExpression(index);

        jbyteArray data = env->NewByteArray(data1.GetLength());
        env->SetByteArrayRegion(data, 0, data1.GetLength(), (jbyte *)data1.GetRawString());

        env->ReleaseByteArrayElements(data, env->GetByteArrayElements(data, JNI_FALSE), 0);

        return data;
    }

    JNIEXPORT jbyteArray JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeGetMotion(JNIEnv *env, jclass type, jint index)
    {
        LAppLive2DManager *l2d = LAppLive2DManager::GetInstance();
        LAppModel *model = l2d->GetModel();

        Csm::csmString data1 = model->GetMotion(index);

        jbyteArray data = env->NewByteArray(data1.GetLength());
        env->SetByteArrayRegion(data, 0, data1.GetLength(), (jbyte *)data1.GetRawString());

        env->ReleaseByteArrayElements(data, env->GetByteArrayElements(data, JNI_FALSE), 0);

        return data;
    }

    JNIEXPORT jint JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeGetExpressionSize(JNIEnv *env, jclass type)
    {
        LAppLive2DManager *l2d = LAppLive2DManager::GetInstance();
        LAppModel *model = l2d->GetModel();

        return model->GetExpressionSize();
    }

    JNIEXPORT jint JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeGetMotionSize(JNIEnv *env, jclass type)
    {
        LAppLive2DManager *l2d = LAppLive2DManager::GetInstance();
        LAppModel *model = l2d->GetModel();

        if (model == nullptr)
        {
        }

        return model->GetMotionSize();
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeStartMotion(JNIEnv *env, jclass type, jbyteArray group, jint no, jint priority)
    {
        LAppLive2DManager *l2d = LAppLive2DManager::GetInstance();
        LAppModel *model = l2d->GetModel();

        char *chars = NULL;
        jbyte *bytes;
        bytes = env->GetByteArrayElements(group, 0);
        int chars_len = env->GetArrayLength(group);
        chars = new char[chars_len + 1];
        memset(chars, 0, chars_len + 1);
        memcpy(chars, bytes, chars_len);
        chars[chars_len] = 0;

        env->ReleaseByteArrayElements(group, bytes, 0);

        model->StartMotion(chars, no, priority);

        delete[] chars;
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeStartExpressions(JNIEnv *env, jclass type, jbyteArray name)
    {
        LAppLive2DManager *l2d = LAppLive2DManager::GetInstance();
        LAppModel *model = l2d->GetModel();

        char *chars = NULL;
        jbyte *bytes;
        bytes = env->GetByteArrayElements(name, 0);
        int chars_len = env->GetArrayLength(name);
        chars = new char[chars_len + 1];
        memset(chars, 0, chars_len + 1);
        memcpy(chars, bytes, chars_len);
        chars[chars_len] = 0;

        env->ReleaseByteArrayElements(name, bytes, 0);

        model->SetExpression(chars);

        delete[] chars;
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeOnStart(JNIEnv *env, jclass type)
    {
        LAppDelegate::GetInstance()->OnStart();
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeOnStop(JNIEnv *env, jclass type)
    {
        LAppDelegate::GetInstance()->OnStop();
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeOnDestroy(JNIEnv *env, jclass type)
    {
        LAppDelegate::GetInstance()->OnDestroy();
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeOnSurfaceCreated(JNIEnv *env, jclass type)
    {
        LAppDelegate::GetInstance()->OnSurfaceCreate();
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeOnSurfaceChanged(JNIEnv *env, jclass type, jint width, jint height)
    {
        LAppDelegate::GetInstance()->OnSurfaceChanged(width, height);
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeOnDrawFrame(JNIEnv *env, jclass type)
    {
        LAppDelegate::GetInstance()->Run();
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeOnTouchesBegan(JNIEnv *env, jclass type, jfloat pointX, jfloat pointY)
    {
        LAppDelegate::GetInstance()->OnTouchBegan(pointX, pointY);
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeOnTouchesEnded(JNIEnv *env, jclass type, jfloat pointX, jfloat pointY)
    {
        LAppDelegate::GetInstance()->OnTouchEnded(pointX, pointY);
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeOnTouchesMoved(JNIEnv *env, jclass type, jfloat pointX, jfloat pointY)
    {
        LAppDelegate::GetInstance()->OnTouchMoved(pointX, pointY);
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeSetPos(JNIEnv *env, jclass type, jfloat pointX, jfloat pointY)
    {
        LAppLive2DManager *manager = LAppLive2DManager::GetInstance();
        manager->x = pointX;
        manager->y = pointY;
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeSetScale(JNIEnv *env, jclass type, jfloat scale)
    {
        LAppLive2DManager *manager = LAppLive2DManager::GetInstance();
        manager->scale = scale;
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeSetPosX(JNIEnv *env, jclass type, jfloat data)
    {
        LAppLive2DManager *manager = LAppLive2DManager::GetInstance();
        manager->x = data;
    }

    JNIEXPORT void JNICALL
    Java_com_live2d_demo_JniBridgeJava_nativeSetPosY(JNIEnv *env, jclass type, jfloat data)
    {
        LAppLive2DManager *manager = LAppLive2DManager::GetInstance();
        manager->y = data;
    }
}
