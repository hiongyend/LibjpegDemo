
#include <android/bitmap.h>
#include <android/log.h>
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>
#include <time.h>
#include "jpeg/jpeglib.h"
#include "jpeg/jversion.h"
#include "jpeg/cdjpeg.h"
#include "jpeg/android/config.h"
typedef uint8_t BYTE;
#define TAG "image "
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

#define true 1
#define false 0
char *error;
struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  error=myerr->pub.jpeg_message_table[myerr->pub.msg_code];
  LOGE("jpeg_message_table[%d]:%s", myerr->pub.msg_code,myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
 // LOGE("addon_message_table:%s", myerr->pub.addon_message_table);
//  LOGE("SIZEOF:%d",myerr->pub.msg_parm.i[0]);
//  LOGE("sizeof:%d",myerr->pub.msg_parm.i[1]);
  longjmp(myerr->setjmp_buffer, 1);
}
//图片压缩方法
int generateJPEG(BYTE* data, int w, int h, int quality,
        const char* outfilename, jboolean optimize) {
    int nComponent = 3;

    struct jpeg_compress_struct jcs;

    struct my_error_mgr jem;

    jcs.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;
        if (setjmp(jem.setjmp_buffer)) {
            return 0;
         }
     //为JPEG对象分配空间并初始化
    jpeg_create_compress(&jcs);
    //获取文件信息
    FILE* f = fopen(outfilename, "wb");
    if (f == NULL) {
        return 0;
    }
    //指定压缩数据源
    jpeg_stdio_dest(&jcs, f);
    jcs.image_width = w;
    jcs.image_height = h;
    if (optimize) {
        LOGE("optimize==ture");
    } else {
    	LOGE("optimize==false");
    }

    jcs.arith_code = false;
    jcs.input_components = nComponent;
    if (nComponent == 1)
        jcs.in_color_space = JCS_GRAYSCALE;
    else
        jcs.in_color_space = JCS_RGB;

    jpeg_set_defaults(&jcs);
    jcs.optimize_coding = optimize;
    //为压缩设定参数，包括图像大小，颜色空间
    jpeg_set_quality(&jcs, quality, true);
    //开始压缩
    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride;
    row_stride = jcs.image_width * nComponent;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &data[jcs.next_scanline * row_stride];
        //写入数据
        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }

    if (jcs.optimize_coding) {
    	LOGE("optimize==ture");
        } else {
        	LOGE("optimize==false");
        }
    //压缩完毕
    jpeg_finish_compress(&jcs);
    //释放资源
    jpeg_destroy_compress(&jcs);
    fclose(f);

    return 1;
}

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} rgb;

//将java string转换为char*
char* jstringTostring(JNIEnv* env, jbyteArray barr) {
    char* rtn = NULL;
    jsize alen = (*env)->GetArrayLength(env, barr);
    jbyte* ba = (*env)->GetByteArrayElements(env, barr, 0);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    (*env)->ReleaseByteArrayElements(env, barr, ba, 0);
    return rtn;
}

JNIEXPORT jstring JNICALL Java_com_kincai_libjpeg_ImageUtils_compressBitmap
  (JNIEnv *env, jclass jclass, jobject bitmap, jint width, jint height, jint quality, jbyteArray fileName, jboolean optimize) {

	AndroidBitmapInfo infoColor;
	int ret;
	BYTE *pixelColor;
	BYTE *data;
	BYTE *tempData;
	char *filename = jstringTostring(env,fileName);


	if((ret = AndroidBitmap_getInfo(env,bitmap,&infoColor)) < 0) {\
		LOGE("解析错误");
		return (*env)->NewStringUTF(env,"0");
	}

	if((ret = AndroidBitmap_lockPixels(env,bitmap,&pixelColor)) < 0) {
		LOGE("加载失败");
	}

	BYTE r,g,b;
	int color;
	data = malloc(width* height * 3);
	tempData = data;
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			color = *((int*)pixelColor);
			r = ((color&0x00FF0000) >> 16);
			g = ((color&0x0000FF00) >> 8);
			b = color&0X000000FF;

			*data = b;
			*(data+1) = g;
			*(data+2) = r;
			data += 3;
			pixelColor += 4;
		}
	}

	AndroidBitmap_unlockPixels(env,bitmap);
	int resultCode = generateJPEG(tempData,width,height,quality,filename,optimize);

	free(tempData);
	if(resultCode == 0) {
		jstring result = (*env)->NewStringUTF(env,"0");
		return result;
	}

	return (*env)->NewStringUTF(env,"1");
}










