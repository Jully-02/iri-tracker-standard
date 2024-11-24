#ifndef __IDDK_2000_API_H_
#define __IDDK_2000_API_H_

#include "Iddk2000Defs.h"

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#	ifdef IDDKAPI_EXPORTS
#		define IDDKAPI __declspec(dllexport)
#	else
#		define IDDKAPI __declspec(dllimport)
#	endif
#else
#	define IDDKAPI
#endif

#if defined(__cplusplus)
extern "C" {
#endif


	/*********** SDK Version && Config *********/
	IDDKAPI unsigned int Iddk_GetSdkVersion();

	IDDKAPI IddkResult Iddk_GetSdkDescription(
		__out char *pBuffer,
		__inout int *pnSize);

	IDDKAPI IddkResult Iddk_GetSdkConfig(
		__out IddkConfig *pIddkConfig);

	IDDKAPI IddkResult Iddk_SetSdkConfig(
		__in const IddkConfig *pIddkConfig);

	IDDKAPI IddkResult Iddk_GetDeviceConfig(
		__in HIRICAMM hDevice,
		__out IddkDeviceConfig *pDevConfig);

	IDDKAPI IddkResult Iddk_SetDeviceConfig(
		__in HIRICAMM hDevice,
		__in const IddkDeviceConfig *pDevConfig);

	/************** Device Control **************/
	IDDKAPI IddkResult Iddk_ScanDevices(
		__out const char ***ppDeviceDescs,
		__out int *pnDevicesCount);

	IDDKAPI IddkResult Iddk_OpenDevice(
		__in const char *pDeviceDesc, 
		__out HIRICAMM *phDevice);

	IDDKAPI IddkResult Iddk_CloseDevice(
		__in HIRICAMM hDevice);

	IDDKAPI IddkResult Iddk_GetDeviceInfo(
		__in HIRICAMM hDevice,
		__out IddkDeviceInfo *pDeviceInfo);

	IDDKAPI IddkResult Iddk_GetDeviceInfoEx(
		__in HIRICAMM hDevice,
		__out IddkDeviceInfoEx *pDeviceInfoEx);

	IDDKAPI IddkResult Iddk_InitCamera(
		__in HIRICAMM hDevice,
		__out int *pnImageWidth,
		__out int *pnImageHeight);

	IDDKAPI IddkResult Iddk_DeinitCamera(
		__in HIRICAMM hDevice);

	IDDKAPI IddkResult Iddk_StartCapture(
		__in HIRICAMM hDevice,
		__in IddkCaptureMode captureMode,
		__in int nCaptureModeParam,
		__in IddkQualityMode qualityMode,
		__in IddkCaptureOperationMode operationMode,
		__in IddkEyeSubtype eyeSubtype,
		__in unsigned char bAutoLeds,
		__in IDDKCAPTUREPROC captureProc,
		__in void *pParam);

	IDDKAPI IddkResult Iddk_StopCapture(
		__in HIRICAMM hDevice);

	IDDKAPI IddkResult Iddk_ClearCapture(
		__in HIRICAMM hDevice,
		__in IddkEyeSubtype eyeSubtype
		);

	IDDKAPI IddkResult Iddk_OperateCapture(
		__in HIRICAMM hDevice);

	IDDKAPI IddkResult Iddk_GetCaptureStatus(
		__in HIRICAMM hDevice,
		__out IddkCaptureStatus *pCaptureStatus);

	IDDKAPI IddkResult Iddk_GetStreamImage(
		__in HIRICAMM hDevice,
		__out IddkImage **pImages,
		__out int *pnMaxEyeSubtypes,
		__out IddkCaptureStatus *pCaptureStatus);

	IDDKAPI IddkResult Iddk_GetResultImage(
		__in HIRICAMM hDevice,
		__in IddkImageKind imageKind,
		__in IddkImageFormat imageFormat,
		__in unsigned char nCompressionQuality,
		__out IddkImage **pImages,
		__out int* pnMaxEyeSubtypes,
		__in int checkImageOptions = 0
		);

	IDDKAPI IddkResult Iddk_GetResultQuality(
		__in HIRICAMM hDevice,
		__out IddkIrisQuality **pQualities,
		__out int *pnMaxEyeSubtypes
		);

	IDDKAPI IddkResult Iddk_GetResultIsoImage( 
		__in HIRICAMM hDevice,
		__in IddkIsoRevision isoRevision, 
		__in IddkImageFormat imageFormat, 
		__in IddkImageKind imageKind, 
		__in unsigned char nCompressionQuality, 
		__in IddkEyeSubtype eyeSubtype,
        __in const IddkDataBuffer *pCaptureTimeBuffer, //Unofficial build to support MOSIP SBI/MDS
		__out IddkDataBuffer *pIsoImage
		);

	IDDKAPI IddkResult Iddk_GetImageData(
		__in const IddkImage *pIritechImage,
		__out IddkImage *pImage);

	/***************** IRIS RECOGNITION *************************/

	IDDKAPI IddkResult Iddk_GetResultTemplate( 
		__in HIRICAMM hDevice,
		__out IddkDataBuffer* pTemplate);

	IDDKAPI IddkResult Iddk_GetTemplateInfo(
		__in HIRICAMM hDevice,
		__in const IddkDataBuffer *pTemplate,
		__out IddkTemplateInfo *pTemplateInfo);

	IDDKAPI IddkResult Iddk_LoadGallery(
		__in HIRICAMM hDevice,
		__out char*** ppEnrollIds,
		__out unsigned int *pnEnrollIdsCount,
		__out unsigned int *pnUsedSlots,
		__out unsigned int *pnMaxSlots);

	IDDKAPI IddkResult Iddk_GetGalleryInfo(
		__in HIRICAMM hDevice,
		__out char ***ppEnrollIds,
		__out unsigned int *pnEnrollIdsCount,
		__out unsigned int *pnUsedSlots,
		__out unsigned int *pnMaxSlots);

	IDDKAPI IddkResult Iddk_EnrollTemplate( 
		__in HIRICAMM hDevice,
		__in const char *enrollId,
		__in const IddkDataBuffer *pTemplate);

	IDDKAPI IddkResult Iddk_EnrollCapture(
		__in HIRICAMM hDevice,
		__in const char *enrollId);

	IDDKAPI IddkResult Iddk_UnenrollTemplate( 
		__in HIRICAMM hDevice,
		__in const char *pEnrollId);

	IDDKAPI IddkResult Iddk_CommitGallery(
		__in HIRICAMM hDevice);

	IDDKAPI IddkResult Iddk_GetEnrolleeInfo(
		__in HIRICAMM hDevice,
		__in const char *enrollId,
		__out IddkTemplateInfo *pTemplateInfo);

	IDDKAPI IddkResult Iddk_GetEnrolleeTemplate(
		__in HIRICAMM hDevice,
		__in const char *enrollId,
		__out IddkDataBuffer *pTemplate);

	IDDKAPI IddkResult Iddk_Compare11(
		__in HIRICAMM hDevice,
		__in const char *enrollId,
		__out float *pDistance);

	IDDKAPI IddkResult Iddk_Compare11WithTemplate(
		__in HIRICAMM hDevice,
		__in const IddkDataBuffer *pTemplate,
		__out float *pDistance);

	IDDKAPI IddkResult Iddk_Compare1N(
		__in HIRICAMM hDevice,
		__in float maxDistance,
		__out IddkComparisonResult **pComparisonResults,
		__out int *pnResultsCount);

	/************** Security Infrastructure **************/

	IDDKAPI IddkResult Iddk_UpdateCustomerCertificate(
		__in HIRICAMM hDevice,
		__in const IddkDataBuffer *pSignedX509Cert);

	IDDKAPI IddkResult Iddk_GetCameraCertificate(
		__in HIRICAMM hDevice,
		__out IddkDataBuffer *pCamX509Cert);

	IDDKAPI IddkResult Iddk_UpdateCameraCertificate( 
		__in HIRICAMM hDevice,
		__in const IddkDataBuffer *pSignedPfxCert,
		__in const IddkDataBuffer *pEncryptedPassword);

	IDDKAPI IddkResult Iddk_UpdateNonvolatileInfo( 
		__in HIRICAMM hDevice,
		__in const IddkDataBuffer *pEncryptedNonvolatile);

	IDDKAPI IddkResult Iddk_GetEncryptedResultImage(
		__in HIRICAMM hDevice,
		__in IddkImageKind imageKind,
		__in IddkImageFormat imageFormat,
		__in unsigned char nCompressionQuality,
		__in const IddkDataBuffer *pVolatileInfo,
		__out IddkDataBuffer **pEncryptedImages,
		__out int *pnMaxEyeSubtypes
		);

	IDDKAPI IddkResult Iddk_GetEncryptedResultIsoImage(
		__in HIRICAMM hDevice,
		__in IddkIsoRevision isoRevision,
		__in IddkImageFormat imageFormat,
		__in IddkImageKind imageKind,
		__in unsigned char nCompressionQuality,
		__in IddkEyeSubtype eyeSubtype,
		__in IddkDataBuffer* pVolatile,
		__out IddkDataBuffer *pEncryptedIsoImage
		);

	IDDKAPI IddkResult Iddk_GetEncryptedResultTemplate(
		__in HIRICAMM hDevice,
		__in const IddkDataBuffer *pVolatileInfo,
		__out IddkDataBuffer *pEncryptedTemplate);

	IDDKAPI IddkResult Iddk_EnrollEncryptedTemplate(
		__in HIRICAMM hDevice,
		__in const char *enrollId,
		__in const IddkDataBuffer *pEncryptedTemplate);

	IDDKAPI IddkResult Iddk_GetEncryptedEnrolleeTemplate(
		__in HIRICAMM hDevice,
		__in const IddkDataBuffer *pVolatileInfo,
		__in const char *enrollId,
		__out IddkDataBuffer *pEncryptedTemplate);

	IDDKAPI IddkResult Iddk_Compare11WithEncryptedTemplate(
		__in HIRICAMM hDevice,
		__in const IddkDataBuffer *pEncryptedTemplate,
		__out float *pDistance);

	/********* Led Control ************/
	IDDKAPI IddkResult Iddk_ControlLed(
		__in HIRICAMM hDevice,
		__in IddkLedId ledId,
		__in unsigned char bOn,
		__in unsigned int nDuration);

	IDDKAPI IddkResult Iddk_ControlBlinkingLed(
		__in HIRICAMM hDevice,
		__in IddkLedId ledId,
		__in unsigned char bOn,
		__in unsigned int nHalfCycle,
		__in unsigned int nDuration);

	/********* System Management  ************/

	IDDKAPI IddkResult Iddk_EnrollAsAdmin(
		__in HIRICAMM hDevice,
		__in const char *enrollId);

	IDDKAPI IddkResult Iddk_UnenrollAdmin(
		__in HIRICAMM hDevice,
		__in const char *enrollId);

	IDDKAPI IddkResult Iddk_SetUserRole(
		__in HIRICAMM hDevice,
		__in const char *enrollId,
		__in IddkSystemRole userRole);

	IDDKAPI IddkResult Iddk_Login(
		__in HIRICAMM hDevice,
		__in const char *enrollId,
		__in IddkSystemRole sysRole);

	IDDKAPI IddkResult Iddk_Logout(
		__in HIRICAMM hDevice);

	IDDKAPI IddkResult Iddk_GetAdminGalleryInfo( 
		__in HIRICAMM hDevice,
		__out char ***ppEnrollIds,
		__out unsigned int *pnEnrollIds,
		__out unsigned int *pnUsedSlots,
		__out unsigned int *pnMaxSlots);

	IDDKAPI IddkResult Iddk_LockDevice(
		__in HIRICAMM hDevice);

	IDDKAPI IddkResult Iddk_UnlockDevice(
		__in HIRICAMM hDevice);

	IDDKAPI IddkResult Iddk_ResetCertificates(
		__in HIRICAMM hDevice,
		__in unsigned char bDeleteCustCert,
		__in unsigned char bRegenCamCert);

	/************ Power control **************/
	IDDKAPI IddkResult Iddk_SleepDevice(
		__in HIRICAMM hDevice,
		__in IddkSleepCode sleepCode,
		__in unsigned int nSleepArg);


	IDDKAPI IddkResult Iddk_Recovery(
		__in HIRICAMM hDevice,
		IddkRecoveryCode code);

#if defined(__cplusplus)
}
#endif

#endif
