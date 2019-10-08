#include "cocos2d.h"

GLint GOpenGLMaxTextureSize = 4096;
GLint GOpenGLMaxTextureUnits = 1;
bool GOpenGLSupportsShareableVAO = false;
bool GOpenGLSupportsTemp1 = false;
bool GOpenGLSupportsTemp2 = false;
bool GOpenGLSupportsTemp3 = false;

void OpenGLGatherGPUInfo()
{
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &GOpenGLMaxTextureSize);					// 最大纹理
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &GOpenGLMaxTextureUnits);
	char lg[512];
	sprintf(lg, "  {GOpenGLMaxTextureSize = %u; GOpenGLMaxTextureUnits = %u}", GOpenGLMaxTextureSize, GOpenGLMaxTextureUnits);
	_log(lg);
	if (GOpenGLMaxTextureUnits < 4) {MessageBoxA(0, "Max texture units < 4", "Error", MB_OK);}

//	_valueDict["gl.vendor"] = Value((const char*)glGetString(GL_VENDOR));		// 供应商
//	_valueDict["gl.renderer"] = Value((const char*)glGetString(GL_RENDERER));	// 渲染器
//	_valueDict["gl.version"] = Value((const char*)glGetString(GL_VERSION));		// 版本

	char* _glExtensions = (char*)glGetString(GL_EXTENSIONS);					// 扩展
	if (_glExtensions)
	{
		_log("  {GL_EXTENSIONS}");
		_log(_glExtensions);

		GOpenGLImageIsBGRA888 = strstr(_glExtensions, "GL_IMG_texture_format_BGRA888")? true: false;
		GOpenGLSupportsETC1 = strstr(_glExtensions, "GL_OES_compressed_ETC1_RGB8_texture")? true: false;
		GOpenGLSupportsS3TC = strstr(_glExtensions, "GL_EXT_texture_compression_s3tc")? true: false;
		GOpenGLSupportsPVRTC = strstr(_glExtensions, "GL_IMG_texture_compression_pvrtc")? true: false;

		_log((GOpenGLSupportsETC1)? 		"  {GOpenGLSupportsETC1 = true}": 			"  {GOpenGLSupportsETC1 = false}");
		_log((GOpenGLSupportsS3TC)? 		"  {GOpenGLSupportsS3TC = true}":			"  {GOpenGLSupportsS3TC = false}");
		_log((GOpenGLSupportsShareableVAO)? "  {GOpenGLSupportsShareableVAO = true}":	"  {GOpenGLSupportsShareableVAO = false}");
		_log((GOpenGLImageIsBGRA888)? 		"  {GOpenGLImageIsBGRA888 = true}":			"  {GOpenGLImageIsBGRA888 = false}");
	}
//	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);
//	_valueDict["gl.max_texture_units"] = Value((int)_maxTextureUnits);
/*
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	glGetIntegerv(GL_MAX_SAMPLES_APPLE, &_maxSamplesAllowed);
	_valueDict["gl.max_samples_allowed"] = Value((int)_maxSamplesAllowed);
#endif
*/
//	_supportsATITC = checkForGLExtension("GL_AMD_compressed_ATC_texture");
//	_valueDict["gl.supports_ATITC"] = Value(_supportsATITC);
//	_supportsPVRTC = checkForGLExtension("GL_IMG_texture_compression_pvrtc");
//	_valueDict["gl.supports_PVRTC"] = Value(_supportsPVRTC);
//	_supportsNPOT = true;
//	_valueDict["gl.supports_NPOT"] = Value(_supportsNPOT);
//	_supportsDiscardFramebuffer = checkForGLExtension("GL_EXT_discard_framebuffer");
//	_valueDict["gl.supports_discard_framebuffer"] = Value(_supportsDiscardFramebuffer);
//	_supportsShareableVAO = checkForGLExtension("vertex_array_object");
//	_valueDict["gl.supports_vertex_array_object"] = Value(_supportsShareableVAO);*/
}
/*
Configuration* Configuration::getInstance()
{
	if (! s_sharedConfiguration)
	{
		s_sharedConfiguration = new Configuration();
		s_sharedConfiguration->init();
	}

	return s_sharedConfiguration;
}

void Configuration::destroyInstance()
{
}

bool Configuration::checkForGLExtension(const std::string &searchName) const
{
   return  (_glExtensions && strstr(_glExtensions, searchName.c_str() ) ) ? true : false;
}

//
// getters for specific variables.
// Mantained for backward compatiblity reasons only.
//
int Configuration::getMaxTextureSize() const
{
	return _maxTextureSize;
}

int Configuration::getMaxModelviewStackDepth() const
{
	return _maxModelviewStackDepth;
}

int Configuration::getMaxTextureUnits() const
{
	return _maxTextureUnits;
}

bool Configuration::supportsNPOT() const
{
	return _supportsNPOT;
}

bool Configuration::supportsPVRTC() const
{
	return _supportsPVRTC;
}

bool Configuration::supportsETC() const
{
	//GL_ETC1_RGB8_OES is not defined in old opengl version
#ifdef GL_ETC1_RGB8_OES
	return _supportsETC1;
#else
	return false;
#endif
}

bool Configuration::supportsS3TC() const
{
	return _supportsS3TC;
}

bool Configuration::supportsATITC() const
{
	return _supportsATITC;
}

bool Configuration::supportsBGRA8888() const
{
	return _supportsBGRA8888;
}

bool Configuration::supportsDiscardFramebuffer() const
{
	return _supportsDiscardFramebuffer;
}

bool Configuration::supportsShareableVAO() const
{
}
*/
