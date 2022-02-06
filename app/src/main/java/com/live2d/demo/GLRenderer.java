/**
 * Copyright(c) Live2D Inc. All rights reserved.
 * <p>
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

package com.live2d.demo;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import java.nio.charset.StandardCharsets;

public class GLRenderer implements GLSurfaceView.Renderer {

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glClearColor(0f, 0f, 0f, 0f);
        JniBridgeJava.nativeOnSurfaceCreated();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        JniBridgeJava.nativeOnSurfaceChanged(width, height);
        //JniBridgeJava.LoadModel("sizuku", "shizuku");
        //JniBridgeJava.nativeEnableRandomMotion(false);
        //JniBridgeJava.nativeSetBreath("PARAM_BREATH".getBytes(StandardCharsets.UTF_8));
        //JniBridgeJava.nativeInitModel();
        //JniBridgeJava.nativeSetScale(1f);

    }

    @Override
    public void onDrawFrame(GL10 gl) {
        JniBridgeJava.nativeOnDrawFrame();
    }
}