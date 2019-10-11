#ifndef _CRAZYMO_BASE_H_
#define _CRAZYMO_BASE_H_

//+Device
void postQuitMessage();
void setKeyboardState(bool open, uint8 winID, uint8 ctrID, int crLeft, int crTop, int crWide, int crHigh);

int  getDPI();
void setAccelerometerEnabled(bool isEnabled);
void setAccelerometerInterval(float interval);

void VideoPlay(const char VideoName[], int left, int top, int wide, int high);
void VideoComplete();
void RecorderStart(const char RecorderID[]);
void RecorderStop();
void PasteBoardGet(uint8 utf8[], int len);
void PasteBoardSet(const uint8 utf8[]);
//-Device
//+Configuration
extern GLint GOpenGLMaxTextureSize, GOpenGLMaxTextureUnits;
extern bool GOpenGLImageIsBGRA888;
extern bool GOpenGLSupportsETC1;
extern bool GOpenGLSupportsS3TC;
extern bool GOpenGLSupportsPVRTC;
extern bool GOpenGLSupportsShareableVAO;
/*
	GLint		   _maxModelviewStackDepth;
	bool			_supportsATITC;
	bool			_supportsNPOT;
	bool			_supportsDiscardFramebuffer;
	GLint		   _maxSamplesAllowed;
	GLint		   _maxTextureUnits;
	char *		  _glExtensions;
*/
void OpenGLGatherGPUInfo();
//-Configuration
//+Shader
bool shaderCreateCommonGray();								// ��ɫ�� ���㡢����  �Ҷ�
void shaderDeleteDeviceObjects();

extern GLuint GOpenGLShaderProgramCommon,		GOpenGLShaderProgramGray;	// ��ͨ �Ҷ�
extern GLint GOpenGLShaderMatrixCommonMVP,		GOpenGLShaderMatrixGrayMVP;
extern GLint GOpenGLShaderRGBACommonVec4,		GOpenGLShaderRGBAGrayVec4;
extern GLint GOpenGLShaderTextureCommon,		GOpenGLShaderTextureGray;

extern GLuint GOpenGLShaderProgramCommonETC,	GOpenGLShaderProgramGrayETC;	// ��ͨ �Ҷ� ETC
extern GLint GOpenGLShaderMatrixCommonMVPETC,	GOpenGLShaderMatrixGrayMVPETC;
extern GLint GOpenGLShaderRGBACommonVec4ETC,	GOpenGLShaderRGBAGrayVec4ETC;
extern GLint GOpenGLShaderTextureCommonETC,		GOpenGLShaderTextureGrayETC;

extern GLuint GOpenGLShaderProgramSmooth;					// ƽ�� ����
extern GLint GOpenGLShaderMatrixSmoothMVP;
extern GLint GOpenGLShaderRGBASmoothVec4;
extern GLint GOpenGLShaderTextureSmooth;
//-Shader
// ����2D��ƽ��ͶӰ
#define MatrixCreateOrthographicOffCenter(mm, viewWide, viewHigh, zNearPlane, zFarPlane) \
{ \
	mm[ 0] = 2.0f / (viewWide);	mm[ 1] = 0.0f;				mm[ 2] = 0.0f;															mm[ 3] = 0.0f; \
	mm[ 4] = 0.0f; 				mm[ 5] =-2.0f / (viewHigh);	mm[ 6] = 0.0f;															mm[ 7] = 0.0f; \
	mm[ 8] = 0.0f;				mm[ 9] = 0.0f;				mm[10] = 2.0f / ((zNearPlane) - (zFarPlane));							mm[11] = 0.0f; \
	mm[12] =-1.0f;				mm[13] = 1.0f;				mm[14] = ((zFarPlane) + (zNearPlane)) / ((zNearPlane) - (zFarPlane)); 	mm[15] = 1.0f; \
}
void SocketInit();
void SocketRelease();
//+socket game
void SocketGameCreate(const char ip[], uint16 port);
bool SocketSendMsgToConn(const char msgBody[], uint16 msgLens);	// �������ݸ�������
void SocketGameRecvMsgProcessCall(uint32 msgType, const char msgBody[], uint16 msgLens);	// һ���յ���������Ϣ
uint16 SocketGameGetLoginDataThread(char msgBody[], bool IsRec);	// ��ȡ��¼��Ϣ
//-socket game
//+socket RTMP
void SocketRTMPCreate(const char ip[], uint16 port);
void SocketRTMPRelease();
bool SocketRTMPSendMsg(uint16 Indx, const char msgBody[], uint16 msgLens);	// �������ݸ�������
void SocketRTMPRecvMsgProcessCall(uint32 msgType, const char msgBody[], uint16 msgLens);	// һ���յ���������Ϣ
//-socket RTMP
//+CDN
bool CDNInit();
void CDNProcess();
//-CDN

#endif	// _CRAZYMO_BASE_H_
