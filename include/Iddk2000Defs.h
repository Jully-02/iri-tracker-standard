#ifndef _IDDK_2000_DEFS_H_
#define _IDDK_2000_DEFS_H_

#include "IddkBase.h"
#include "IriLivenessBase.h"

using namespace IriLivenessDetector;

typedef struct __IddkConfig_
{
	IddkCommStd		commStd;
	unsigned char	enableLog;
	unsigned int	uartBaudrate;
	IddkFlowcontrol uartFlowcontrol;
	unsigned char	resetOnOpenDevice;
}IddkConfig;

typedef struct _IddkDataBuffer_
{
	int dataSize;
	unsigned char* data;
}IddkDataBuffer;

typedef struct _IddkImage_
{
	unsigned char imageKind;
	unsigned char imageFormat;
	int imageWidth;
	int imageHeight;
	unsigned char* imageData;
	int imageDataLen;
	LIVENESS_STATUS livenessStatus;
	IddkEyeSubtype eyeSide;
	bool isFlip;
}IddkImage;

typedef struct _IddkTemplateInfo_
{
	unsigned int templateVersion;
	unsigned int creatorVersion;
	unsigned int leftIrisCount;
	unsigned int rightIrisCount;
	unsigned int unknownIrisCount;
	unsigned int totalIrisCount;
}IddkTemplateInfo;

typedef struct _IddkComparisonResult_
{
	char enrollId[32];
	float comparisonDistance;
}IddkComparisonResult;

typedef struct _IddkIrisQuality_
{
	unsigned char totalScore;
	unsigned char usableArea;
}IddkIrisQuality;

typedef struct _IddkDeviceConfig_
{
	unsigned char encryptionMode;
	unsigned char enableDeduplication;
	float deduplicationThreshold;
	float authenThreshold;
	unsigned char supervisedEnrollment;
	unsigned char dataClosure;
	unsigned char enableStream;
	unsigned char streamScale;
	unsigned char streamFormat;
	unsigned char compressionQuality;
	unsigned int baudrate;
	unsigned char flowControl;
	unsigned char checkImageMargin;
	float		  leftRightRatio;// to iris radius;
	float		  topBottomRatio;// to Iris radius;
	unsigned char enableDeepsleepInterrupt;
	unsigned char sleepMode;
	unsigned short sleepTimeout;

	unsigned char reserved[12];
} IddkDeviceConfig;



typedef struct _IddkDeviceInfo_
{
	IddkProductId productID;	
	char productName[64];
	unsigned char isBinocular;
	char serialNumber[17];
	unsigned int propertyFlag;
	unsigned int kernelVersion;
	unsigned int kernelRevision;
	unsigned int deviceFeatures;
	unsigned int lenModel;
	char customerID[32];
}IddkDeviceInfo;

typedef struct _IddkDeviceInfoEx_:_IddkDeviceInfo_
{
	char algorithmProvider[8]; //Nhon: Add extractor Alg provider for firmware of IriShield as UIDAI's requirement.
	char  algorithmVersion[8]; //Nhon: Add extractor Alg version for firmware of IriShield as UIDAI's requirement.
	unsigned char reserved[8];
}IddkDeviceInfoEx;

typedef void (__cdecl *IDDKCAPTUREPROC)(
	__in void*	pParam,
	__in IddkImage* pImages, 
	__in int nMaxEyeSubtypes,
	__in IddkCaptureStatus	*pCaptureStatus,
	__in IddkResult *pCaptureError
	);

#define IDDK_CHECK_EYE_LIVENESS		(1 << 0)
#define IDDK_CHECK_EYE_SIDE			(1 << 1)
#define IDDK_CHECK_EYE_ORIENTATION  (1 << 2)

#endif
