#ifndef TEXTURE_WINDOW_H
#define TEXTURE_WINDOW_H

#include <android/native_window_jni.h>

JavaVM* javaVM = NULL;
jclass activityClass;
jobject activityObj;
static ANativeWindow* _theNativeWindow;

namespace TextureWindow {

	static ANativeWindow* getANativeWindow(int texture) {
		logInf("TextureWindow::getANativeWindow START!");
		if (javaVM == NULL) {
			logErr("TextureSurface::setTextureSurface Error: javaVM is NULL");
			return 0;
		}
		JNIEnv *env;
		javaVM->AttachCurrentThread(&env, NULL);
		jmethodID method = env->GetMethodID(activityClass, "initSurfaceTexture", "(I)V");
		if (method == NULL) {
			logErr("TextureSurface::setTextureSurface Error: could not find createSurfaceFromTexture method");
			return 0;
		}
		logInf("TextureWindow::getANativeWindow A!");
		env->CallVoidMethod(activityObj, method, texture);
		logInf("TextureWindow::getANativeWindow B!");
		javaVM->DetachCurrentThread();
		logInf("TextureWindow::getANativeWindow C!");
		return _theNativeWindow;
	}

};

#ifdef __cplusplus
extern "C"
{
#endif

JNIEXPORT void JNICALL
Java_net_i2cat_videoproject_TextureSurfaceActivity_cacheJNIVars(JNIEnv *env, jobject jobj)
{
	env->GetJavaVM(&javaVM);
	jclass cls = env->GetObjectClass(jobj);
	activityClass = (jclass) env->NewGlobalRef(cls);
	activityObj = env->NewGlobalRef(jobj);
}

JNIEXPORT void JNICALL
Java_net_i2cat_videoproject_TextureSurfaceActivity_setSurface(JNIEnv *env, jclass clazz, jobject surface)
{
	// obtain a native window from a Java surface
	logInf("PUM A! %i", (int)surface);
	_theNativeWindow = ANativeWindow_fromSurface(env, surface);
	logInf("PUM B! %i", (int)_theNativeWindow);
}

#ifdef __cplusplus
}
#endif

#endif
