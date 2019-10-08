#include "Global.h"

void GameAppMouseButtonLeftUIUp()	// 点了UI鼠标抬起的应用层响应
{
}
void GameAppMouseButtonLeftNoUp()	// 这里是没点UI鼠标抬起 应用层响应鼠标弹起
{
}
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
void UIKeyPressApplication(uint32 Key, bool IsChar)
{
}
#endif	// CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
