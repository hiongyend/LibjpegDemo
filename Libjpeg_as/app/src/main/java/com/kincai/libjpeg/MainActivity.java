package com.kincai.libjpeg;

import java.io.File;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;

/**
 * MainActivity
 *
 * @author KINCAI
 *
 */
public class MainActivity extends Activity implements OnClickListener {
    private Button mCombineCompress, mCompressBtn,mOriginalBtn,mAfterCompressBtn;
    private ImageView mImage;


    /** 图片存放根目录*/
    private final String mImageRootDir = Environment
            .getExternalStorageDirectory().getPath() + "/jpeg_picture/";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 压缩后保存临时文件目录
        File tempFile = new File(mImageRootDir);
        if (!tempFile.exists()) {
            tempFile.mkdirs();
        }

        mCompressBtn = (Button) findViewById(R.id.compress_btn);
        mCombineCompress = (Button) findViewById(R.id.size_quality_libjpeg_compress_btn);
        mAfterCompressBtn = (Button) findViewById(R.id.after_compress_btn);
        mOriginalBtn = (Button) findViewById(R.id.original_btn);
        mImage = (ImageView) findViewById(R.id.image);


        mCompressBtn.setOnClickListener(this);
        mCombineCompress.setOnClickListener(this);
        mOriginalBtn.setOnClickListener(this);
        mAfterCompressBtn.setOnClickListener(this);

    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.compress_btn://直接jni libjpeg压缩

                new Thread(new Runnable() {

                    @Override
                    public void run() {
                        final File afterCompressImgFile = new File(mImageRootDir
                                + "/终极压缩666.jpg");

                        String tempCompressImgPath = mImageRootDir+File.separator+"temp.jpg";//事先准备好的sd卡目录下的图片
                        //直接使用jni libjpeg压缩
                        Bitmap bitmap = BitmapFactory.decodeFile(tempCompressImgPath);
                        String codeString = ImageUtils.compressBitmap(bitmap, bitmap.getWidth(), bitmap.getHeight(), 40, afterCompressImgFile.getAbsolutePath().getBytes(), true);
                        Log.e("code", "code "+codeString);

                        MainActivity.this.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mImage.setImageBitmap(BitmapFactory
                                        .decodeFile(afterCompressImgFile.getPath()));
                            }
                        });
                    }
                }).start();

                break;
            case R.id.size_quality_libjpeg_compress_btn://尺寸 质量 libjpeg结合压缩

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        final File afterCompressImgFile = new File(mImageRootDir
                                + "/终极压缩666.jpg");

                        //先尺寸质量压缩后在用jni libjpeg压缩   (先保证SD卡目录下/jpeg_picture/temp.jpg存在)
                        String tempCompressImgPath = mImageRootDir+File.separator+"temp.jpg";
                        ImageUtils.compressBitmap(tempCompressImgPath, afterCompressImgFile.getPath());

                        MainActivity.this.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mImage.setImageBitmap(BitmapFactory
                                        .decodeFile(afterCompressImgFile.getPath()));
                            }
                        });
                    }
                }).start();


                break;
            case R.id.after_compress_btn://压缩后

                final File afterCompressImgFile = new File(mImageRootDir
                        + "/终极压缩666.jpg");
                mImage.setImageBitmap(BitmapFactory
                        .decodeFile(afterCompressImgFile.getPath()));
                break;
            case R.id.original_btn://原图

                String tempCompressImgPath = mImageRootDir+File.separator+"temp.jpg";
                mImage.setImageBitmap(BitmapFactory
                        .decodeFile(tempCompressImgPath));
                break;

            default:
                break;
        }

    }

}
