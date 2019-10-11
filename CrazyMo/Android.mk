LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := crazymox_static

LOCAL_MODULE_FILENAME := libcrazymo

LOCAL_SRC_FILES := \
cocos2d.cpp \
\
Configuration.cpp \
shader.cpp \
ConnSocketGame.cpp \
\
key256.cpp \
db.cpp \
dbGame.cpp \
sha2_256.cpp \
sha3.cpp \
json.cpp \
random.cpp \
base58.cpp \
bci.cpp \
ripemd160.cpp \
\
dbLink.cpp \
dbKey256.cpp \
\
secp256k1.cpp \
\
Image/atitc.cpp \
Image/etc1.cpp \
Image/s3tc.cpp \
Image/TGAlib.cpp \
\
ui/Texture2D.cpp \
ui/UIBamboo.cpp \
ui/UIBase.cpp \
ui/UIBattery.cpp \
ui/UIButton.cpp \
ui/UICheck.cpp \
ui/UICoins.cpp \
ui/UIColorGrid.cpp \
ui/UICombo.cpp \
ui/UICompass.cpp \
ui/UIEdit.cpp \
ui/UIFlashBox.cpp \
ui/UIFont3D.cpp \
ui/UIGoods.cpp \
ui/UIHtml.cpp \
ui/UIImages.cpp \
ui/UILabel.cpp \
ui/UIListBox.cpp \
ui/UIListCtrl.cpp \
ui/UINeon.cpp \
ui/UIOpenGL.cpp \
ui/UIPicture.cpp \
ui/UIProgress.cpp \
ui/UIRadio.cpp \
ui/UIRocker.cpp \
ui/UIScrollView.cpp \
ui/UISlider.cpp \
ui/UITime.cpp \
ui/UITitle.cpp \
ui/UIVScrollBar.cpp \
ui/UIWinBase.cpp \


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/. \
					$(LOCAL_PATH)/android \
					$(LOCAL_PATH)/android/jpeg \
					$(LOCAL_PATH)/android/png

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/. \
					$(LOCAL_PATH)/android \
					$(LOCAL_PATH)/android/jpeg \
					$(LOCAL_PATH)/android/png


LOCAL_EXPORT_LDLIBS := -lGLESv2 \
					   -llog \
					   -lz \
					   -landroid

LOCAL_WHOLE_STATIC_LIBRARIES := cocos_freetype2_static
LOCAL_WHOLE_STATIC_LIBRARIES += crazymoxandroid_static

# define the macro to compile through support/zip_support/ioapi.c
LOCAL_CFLAGS   :=  -DUSE_FILE32API
LOCAL_CPPFLAGS := -Wno-deprecated-declarations -Wno-extern-c-compat
LOCAL_EXPORT_CFLAGS   := -DUSE_FILE32API
LOCAL_EXPORT_CPPFLAGS := -Wno-deprecated-declarations -Wno-extern-c-compat

include $(BUILD_STATIC_LIBRARY)

$(call import-module,android)
