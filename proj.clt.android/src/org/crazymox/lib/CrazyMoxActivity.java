package org.crazymox.lib;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.PixelFormat;
import android.media.AudioManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.FrameLayout;
import android.preference.PreferenceManager.OnActivityResultListener;

import org.crazymox.lib.CrazyMoxHelper.CrazyMoxHelperListener;

// 1.onStart	
// 2.onResume

// 3.onPause	
// 4.onStop
//
// 5.onRestart
// 6.onStart
// 7.onResume

// 3.onPause	
// 4.onStop
//   onDestroy
// 6.onStart
//   onRestoreInstanceState
// 7.onResume
//   onPause
//   onStop
//
//   onDestroy
//   onStart
//   onRestoreInstanceState
//   onResume


public abstract class CrazyMoxActivity extends Activity implements CrazyMoxHelperListener
{
	public static CrazyMoxGLSurfaceView mGLSurfaceView = null;
	public static CrazyMoxEditText mEditText = null;
	public static Context mContext = null;
	public static FrameLayout mFrameLayout = null;
	public static CrazyMoxVideoView mVideoView = null;
	public static CrazyMoxRecorder mRecorder = null;

	private static CrazyMoxTextInputWraper mTextInputWraper = null;
	private static Handler mHandler = null;
	private static ClipboardManager mClipboard = null;

	private final static int HANDLER_SHOW_DIALOG		= 0;
	private final static int HANDLER_OPEN_IME_KEYBOARD 	= 1;
	private final static int HANDLER_CLOSE_IME_KEYBOARD = 2;

	@Override
	protected void onCreate(final Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		if ((getIntent().getFlags() & Intent.FLAG_ACTIVITY_BROUGHT_TO_FRONT) != 0)
		{  
			finish();
			return;
		}

		try	// load library
		{
			ApplicationInfo ai = getPackageManager().getApplicationInfo(getPackageName(), PackageManager.GET_META_DATA);
			Bundle bundle = ai.metaData;
			String libName = bundle.getString("android.app.lib_name");
			System.loadLibrary(libName);
		}catch (Exception e) {
			e.printStackTrace();
		}
		mContext = this;
		mTextInputWraper = new CrazyMoxTextInputWraper();
		mEditText = new CrazyMoxEditText(this);
		mEditText.setOnEditorActionListener(mTextInputWraper);
		nativeActivityCreated(UpdateResource.savePath + "/");
		mClipboard = (ClipboardManager)getSystemService(CLIPBOARD_SERVICE);

		UpdateApk.mContext = this;
		UpdateApk.httpGetApkVers();
		CrazyMoxMusic.mContext = this;
		CrazyMoxSound.Init(this);
		CrazyMoxSoundLoop.Init(this);
		CrazyMoxHelper.init(this);

		File file;
		file = new File(UpdateResource.savePath);					file.mkdirs();
	//	file = new File(UpdateResource.savePath + "/dbUser");		file.mkdirs();
		file = new File(UpdateResource.savePath + "/XMBC");			file.mkdirs();

		// FrameLayout
		ViewGroup.LayoutParams framelayout_params = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
		mFrameLayout = new FrameLayout(this);
		mFrameLayout.setLayoutParams(framelayout_params);
		// CrazyMoxEditText layout
		ViewGroup.LayoutParams edittext_layout_params = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		mEditText.setLayoutParams(edittext_layout_params);
		// ...add to FrameLayout
		mFrameLayout.addView(mEditText);
		// CrazyMoxGLSurfaceView
		mGLSurfaceView = new CrazyMoxGLSurfaceView(this);
		mGLSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		CrazyMoxEGLConfig chooser = new CrazyMoxEGLConfig(8, 8, 8, 8, 16, 8);
		mGLSurfaceView.setEGLConfigChooser(chooser);
		// ...add to FrameLayout
		mFrameLayout.addView(mGLSurfaceView);
		// Switch to supported OpenGL (ARGB888) mode on emulator
		if (isAndroidEmulator()) mGLSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);	// 
		mGLSurfaceView.setCrazyMoxRenderer(new CrazyMoxRenderer());
		mGLSurfaceView.requestFocus();
		mGLSurfaceView.setKeepScreenOn(true);
		// Set framelayout as the content view
		setContentView(mFrameLayout);

		mVideoView = new CrazyMoxVideoView();
		mRecorder = new CrazyMoxRecorder();
		this.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN);
        this.setVolumeControlStream(AudioManager.STREAM_MUSIC);

        mSocketRunnable socketThread = new mSocketRunnable();
        socketThread.start();

		initHandler();
	}
	private class mSocketRunnable extends Thread
	{
		@Override
		public void run()
		{
			super.run();
			while (true)
			{
				nativeActivitySocketThread();
				try
				{
					Thread.sleep(25);
				}catch (InterruptedException e) {
				}
			}
		}
	};
	private native synchronized void nativeActivitySocketThread();
	private static void initHandler()
	{
		mHandler = new Handler()
		{
			@Override
			public void handleMessage(final Message msg)
			{
				switch (msg.what)
				{
				case HANDLER_OPEN_IME_KEYBOARD:
					if (mEditText.requestFocus())
					{
						mEditText.removeTextChangedListener(mTextInputWraper);
						mEditText.setText("");
						final String text = (String)msg.obj;
						mEditText.append(text);
						CrazyMoxTextInputWraper.mOriginText = text;
						mEditText.addTextChangedListener(mTextInputWraper);
						final InputMethodManager imm = (InputMethodManager)mContext.getSystemService(Context.INPUT_METHOD_SERVICE);
						imm.showSoftInput(mEditText, 0);
					}
					break;
				case HANDLER_CLOSE_IME_KEYBOARD:
					{
						mEditText.removeTextChangedListener(mTextInputWraper);
						final InputMethodManager imm = (InputMethodManager)mContext.getSystemService(Context.INPUT_METHOD_SERVICE);
						imm.hideSoftInputFromWindow(mEditText.getWindowToken(), 0);
						mGLSurfaceView.requestFocus();
					}
					break;
				case HANDLER_SHOW_DIALOG:
					{
						AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
						builder.setTitle("Info")
						.setMessage((String)msg.obj)
						.setIcon(android.R.drawable.ic_dialog_alert)
						.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int which)	// Yes button clicked, do something
							{
							//	System.exit(0);
							}
						})
						.setCancelable(false)
						.show();
					}
					break;
				}
			}
		};
	}
	private boolean isAndroidEmulator()
	{
		String product = Build.PRODUCT;
		if (product != null)
		{
			return product.equals("sdk") || product.contains("_sdk") || product.contains("sdk_");
		}
		return false;
	}
	private native void nativeActivityCreated(final String filePath);

	@Override
	protected void onRestart()	// ,,,onRestart,GLSurfaceView
	{
		super.onRestart();
		Log.d("sddfasdfasdf", "onRestart  ----------------------------------------");

		mFrameLayout.removeView(mGLSurfaceView);
		// CrazyMoxGLSurfaceView
		mGLSurfaceView = new CrazyMoxGLSurfaceView(this);
		mGLSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
		CrazyMoxEGLConfig chooser = new CrazyMoxEGLConfig(8, 8, 8, 8, 16, 8);
		mGLSurfaceView.setEGLConfigChooser(chooser);
		// ...add to FrameLayout
		mFrameLayout.addView(mGLSurfaceView);
		// Switch to supported OpenGL (ARGB888) mode on emulator
		if (isAndroidEmulator()) mGLSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);	// 
		mGLSurfaceView.setCrazyMoxRenderer(new CrazyMoxRenderer());
		mGLSurfaceView.requestFocus();
		mGLSurfaceView.setKeepScreenOn(true);
	}
	@Override
	protected void onPause()
	{
		super.onPause();
		// 
//		mWakeLock.release();
		CrazyMoxHelper.onPause();
		mGLSurfaceView.onPause();
	}
	@Override
	protected void onResume()
	{
		super.onResume();
		// onResume()
//		mWakeLock.acquire();
		CrazyMoxHelper.onResume();
		mGLSurfaceView.onResume();
	}
	@Override
	protected void onDestroy()	// android,callActivitySaveUserFile
	{
	//	CrazyMoxMusic.Release();
	//	CrazyMoxSound.Release();
	//	CrazyMoxSoundLoop.Release();
		nativeActivityDestroy();
		super.onDestroy();
	}
	private native void nativeActivityDestroy();

	@Override
	public void showDialog(final String title, final String message)
	{
	}
	@Override
	public void showEditTextDialog(final String title, final String content, final int inputMode, final int inputFlag, final int returnType, final int maxLength)
	{
	}
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		for (OnActivityResultListener listener: CrazyMoxHelper.getOnActivityResultListeners())
		{
			listener.onActivityResult(requestCode, resultCode, data);
		}
		super.onActivityResult(requestCode, resultCode, data);
	}
	@Override
	public void runOnGLThread(final Runnable pRunnable)
	{
		CrazyMoxActivity.mGLSurfaceView.queueEvent(pRunnable);
	}

	//+Call
	private static void callShowDialog(final String pMessage, final String pTitle)
	{
		final Message msg = new Message();
		msg.what = HANDLER_SHOW_DIALOG; msg.obj = pMessage;
		mHandler.sendMessage(msg);
	}
	private static void callOpenIMEKeyboard(final String text, int winID, int ctrID)
	{
		CrazyMoxTextInputWraper.mWindowID = winID; CrazyMoxTextInputWraper.mControlEditID = ctrID;
		final Message msg = new Message();
		msg.what = HANDLER_OPEN_IME_KEYBOARD; msg.obj = text;
		mHandler.sendMessage(msg);
	}
	private static void callCloseIMEKeyboard()
	{
		final Message msg = new Message();
		msg.what = HANDLER_CLOSE_IME_KEYBOARD;
		mHandler.sendMessage(msg);
	}
	private static void callCreateDirectory(final String directory)
	{
		File file;
		file = new File(UpdateResource.savePath + "/" + directory);
		file.mkdirs();
	}
	private static void callActivitySaveUserFile(final String fileName, final String text)	//,androidfopen
	{
		File ResFile = new File(UpdateResource.savePath + "/" + fileName);
		try
		{
			ResFile.delete();	// MD5
			RandomAccessFile fos = new RandomAccessFile(ResFile, "rw");
			fos.seek(0);
			fos.writeBytes(text);
			fos.close();
		}catch (FileNotFoundException e) {
			e.printStackTrace();
		}catch (IOException e) {
		}
	}
	private static String callActivityPasteboardGet()
	{
        if (mClipboard == null) return "";
        ClipData clip = mClipboard.getPrimaryClip();
        if (clip == null) return "";
        if (clip.getItemCount() == 0) return "";
        return clip.getItemAt(0).getText().toString();
    }
	private static void callActivityPasteboardSet(final String text)
	{
		if (mClipboard == null) return;
        ClipData cd = ClipData.newPlainText("text", text);
        mClipboard.setPrimaryClip(cd);
    }
	//-Call
}
