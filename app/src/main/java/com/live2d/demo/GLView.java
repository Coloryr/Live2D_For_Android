package com.live2d.demo;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.View;

@SuppressLint("ViewConstructor")
public class GLView extends GLSurfaceView{
    private GLRenderer _glRenderer;

    public final MainActivity.MyTouchListener myTouchListener = event -> {
        //处理手势事件（根据个人需要去返回和逻辑的处理）
        float pointX = event.getX();
        float pointY = event.getY();
        try {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    JniBridgeJava.nativeOnTouchesBegan(pointX, pointY);
                    break;
                case MotionEvent.ACTION_UP:
                    JniBridgeJava.nativeOnTouchesEnded(pointX, pointY);
                    break;
                case MotionEvent.ACTION_MOVE:
                    JniBridgeJava.nativeOnTouchesMoved(pointX, pointY);
                    break;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    };

    public GLView(Context context) {
        super(context);
        JniBridgeJava.SetContext(context);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        getHolder().setFormat(PixelFormat.TRANSLUCENT);
        setZOrderOnTop(true);
        setEGLContextClientVersion(2);
        _glRenderer = new GLRenderer();
        setRenderer(_glRenderer);
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        //MainActivity.app.registerMyTouchListener(myTouchListener);

        JniBridgeJava.nativeOnStart();
    }

    public void Stop() {
        JniBridgeJava.nativeOnStop();
    }

    public void Destroy() {
        JniBridgeJava.nativeOnDestroy();
    }

    public void DestroyView() {
        MainActivity.app.unRegisterMyTouchListener(myTouchListener);
    }
}
