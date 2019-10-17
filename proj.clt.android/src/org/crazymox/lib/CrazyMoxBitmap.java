package org.crazymox.lib;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Align;
import android.graphics.Paint.FontMetricsInt;
import android.graphics.Typeface;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class CrazyMoxBitmap
{
	private static Context mContext = null;

	public static void setContext(final Context context)
	{
		CrazyMoxBitmap.mContext = context;
	}

	private static Paint newPaint(final String fontName, final int fontSize)
	{
		final Paint paint = new Paint();
		paint.setColor(Color.WHITE);
		paint.setTextSize(fontSize);
		paint.setAntiAlias(true);
		paint.setTextAlign(Align.LEFT);

		/* Set type face for paint, now it support .ttf file. */
		if (fontName.endsWith(".ttf"))
		{
			try
			{
				final Typeface typeFace;
				if (fontName.startsWith("/")) typeFace = Typeface.createFromFile(fontName);
				else typeFace = Typeface.createFromAsset(CrazyMoxBitmap.mContext.getAssets(), fontName);
				paint.setTypeface(typeFace);
			}catch (final Exception e) {	// The file may not find, use system font.
				paint.setTypeface(Typeface.create(fontName, Typeface.NORMAL));
			}
		}else {
			paint.setTypeface(Typeface.create(fontName, Typeface.NORMAL));
		}
		return paint;
	}
	private static boolean createTextBitmapShadowStroke(String ftText, final String ftName, final int ftSizeRGB,
													final boolean shadow, final float shadowDX, final float shadowDY, final float shadowBlur, final float shadowOpacity,
													final boolean stroke, final int strokeSizeRGB)
	{
		final Paint paint = CrazyMoxBitmap.newPaint(ftName, ftSizeRGB>>24);
		paint.setARGB(255, (ftSizeRGB>>16)&0xff, (ftSizeRGB>>8)&0xff, (ftSizeRGB)&0xff);	// set the paint color

		final FontMetricsInt fmi = paint.getFontMetricsInt();
		final int bitmapTotalHigh = (int)Math.ceil(fmi.bottom - fmi.top);
		final int bitmapTotalWide = (int)Math.ceil(paint.measureText(ftText, 0, ftText.length()));
		if ((bitmapTotalWide == 0) || (bitmapTotalHigh == 0))
		{
			return false;
		}

		// padding needed when using shadows (not used otherwise)
		final Bitmap bitmap = Bitmap.createBitmap(bitmapTotalWide, bitmapTotalHigh, Bitmap.Config.ARGB_8888);
		if (bitmap == null) return false;

		final Canvas canvas = new Canvas(bitmap); 
		if (stroke)	// draw again with stroke on if needed
		{
			final Paint paintStroke = CrazyMoxBitmap.newPaint(ftName, ftSizeRGB>>24);
			paintStroke.setStyle(Paint.Style.STROKE);
			paintStroke.setStrokeWidth(strokeSizeRGB>>24);
			paintStroke.setARGB(255, (strokeSizeRGB>>16)&0xff, (strokeSizeRGB>>8)&0xff, (strokeSizeRGB)&0xff);
			
			canvas.drawText(ftText, 0.0f, -fmi.top - (fmi.bottom-fmi.top) + (fmi.descent-fmi.ascent), paintStroke);
		}
		canvas.drawText(ftText, 0.0f, -fmi.top - (fmi.bottom-fmi.top) + (fmi.descent-fmi.ascent), paint);

		final byte[] pixels = new byte[bitmap.getWidth() * bitmap.getHeight() * 4];
		final ByteBuffer buf = ByteBuffer.wrap(pixels);
		if (buf == null) return false;
		buf.order(ByteOrder.nativeOrder());
		bitmap.copyPixelsToBuffer(buf);
		nativeInitBitmapDC(bitmap.getWidth(), bitmap.getHeight(), pixels, fmi.ascent, fmi.descent, fmi.leading, fmi.top, fmi.bottom);

		return true;
	}

	private static native void nativeInitBitmapDC(final int width, final int height, final byte[] pixels, int ascent, int descent, int leading, int top, int bottom);	// c
}
