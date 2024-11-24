#ifndef _IDDK_BASE_H_
#define _IDDK_BASE_H_


#ifdef __in
#undef __in
#endif

#ifdef __out
#undef __out
#endif

#ifdef __inout
#undef __inout
#endif

#if !defined(_MSC_VER) && !defined(__cdecl)
#	define __cdecl
#endif

/** INPUT **/
#define __in
/** OUTPUT **/
#define __out
/** INPUT and OUTPUT **/
#define __inout


typedef void* HIRICAMM;

typedef enum _IddkResult_
{
	IDDK_OK									= 0,
	IDDK_FAILED								= 1,
	IDDK_DEVICE_NOT_FOUND					= 2,	
	IDDK_DEVICE_OPEN_FAILED					= 3,	
	IDDK_DEVICE_NOT_OPEN					= 4,
	IDDK_DEVICE_ALREADY_OPEN				= 5,
	IDDK_DEVICE_ACCESS_DENIED				= 6,
	IDDK_TOO_MANY_OPEN_DEVICES				= 7,
	IDDK_DEVICE_IO_FAILED					= 8,		
	IDDK_DEVICE_IO_TIMEOUT					= 9,
	IDDK_DEVICE_IO_DATA_INVALID				= 10,		
	IDDK_UNSUPPORTED_IMAGE_FORMAT			= 11,
	IDDK_MEMORY_ALLOCATION_FAILED			= 12,
	IDDK_INVALID_MEMORY						= 13,
	IDDK_INVALID_HANDLE						= 14,
	IDDK_INVALID_PARAMETER					= 15,
	IDDK_AUTHEN_FAILED						= 16,	
	IDDK_NOT_ENOUGH_BUFFER					= 17,
	IDDK_VERSION_INCOMPATIBLE				= 18,
	
	IDDK_THREAD_FAILED						= 19,
	IDDK_UNSUPPORTED_COMMAND				= 20,	
	IDDK_IMAGE_CORRUPTED					= 21,
	IDDK_WRONG_EYE_SUBTYPE					= 22,
	IDDK_UNKNOWN_DEVICE						= 23,
	IDDK_UNEXPECTED_ERROR					= 255,

	IDDK_DEV_OUTOFMEMORY					= (0x1000 + 1),	
	IDDK_DEV_NOT_ENOUGH_MEMORY				= (0x1000 + 2),
	IDDK_DEV_INSUFFICIENT_BUFFER			= (0x1000 + 3),	
	IDDK_DEV_INVALID_LICENSE				= (0x1000 + 4),
	IDDK_DEV_IO_FAILED						= (0x1000 + 5),	
	IDDK_DEV_MODULE_NOT_FOUND				= (0x1000 + 6),
	IDDK_DEV_PROC_NOT_FOUND					= (0x1000 + 7),
	IDDK_DEV_BAD_DATA						= (0x1000 + 8),
	IDDK_DEV_FUNCTION_DISABLED				= (0x1000 + 9),
	IDDK_DEV_LOCKED							= (0x1000 + 10),
	IDDK_DEV_BUSY							= (0x1000 + 11),
	IDDK_DEV_LOW_BATTERY					= (0x1000 + 20),
	IDDK_DEV_RUNTIME_EXCEPTION				= (0x1000 + 255),

	IDDK_SEC_FAILED							= (0x2000 + 1),
	IDDK_SEC_INIT_FAILED					= (0x2000 + 2),
	IDDK_SEC_WRONG_PASSWORD					= (0x2000 + 3),
	IDDK_SEC_BAD_LEN						= (0x2000 + 4),
	IDDK_SEC_BAD_DATA						= (0x2000 + 5),
	IDDK_SEC_BAD_ALG						= (0x2000 + 6),
	IDDK_SEC_BAD_KEY						= (0x2000 + 7),
	IDDK_SEC_BAD_SIG						= (0x2000 + 8),
	IDDK_SEC_ENCRYPT_ERROR					= (0x2000 + 9),
	IDDK_SEC_DECRYPT_ERROR					= (0x2000 + 10),
	IDDK_SEC_IMPORT_ERROR					= (0x2000 + 11),
	IDDK_SEC_EXPORT_ERROR					= (0x2000 + 12),
	IDDK_SEC_KEYGEN_ERROR					= (0x2000 + 13),
	IDDK_SEC_HASH_ERROR						= (0x2000 + 14),
	IDDK_SEC_SIG_ERROR						= (0x2000 + 15),
	IDDK_SEC_PRIVILEGE_RESTRICTED			= (0x2000 + 16),


	IDDK_SE_NOT_INIT						= (0x3000 + 1),
	IDDK_SE_NO_CAM							= (0x3000 + 2),
	IDDK_SE_STARTSTOP_CAPTURE_FAILED		= (0x3000 + 3),	
	IDDK_SE_QM_FAILED						= (0x3000 + 4),
	IDDK_SE_NO_FRAME_AVAILABLE				= (0x3000 + 5),
	IDDK_SE_NO_QUALIFIED_FRAME				= (0x3000 + 6),
	IDDK_SE_RIGHT_FRAME_UNQUALIFIED			= (0x3000 + 7),
	IDDK_SE_LEFT_FRAME_UNQUALIFIED			= (0x3000 + 8),
	IDDK_SE_COMPRESSION_FAILED				= (0x3000 + 9),
	IDDK_SE_DECOMPRESSION_FAILED			= (0x3000 + 10),


	IDDK_GAL_NOT_INITIALIZED				= (0x4000 + 1),
	IDDK_GAL_LOAD_FAILED					= (0x4000 + 2),
	IDDK_GAL_EMPTY							= (0x4000 + 3),
	IDDK_GAL_FULL							= (0x4000 + 4),
	IDDK_GAL_ID_NOT_EXIST					= (0x4000 + 5),
	IDDK_GAL_ID_NOT_ENOUGH_SLOT				= (0x4000 + 6),
	IDDK_GAL_NOT_ENOUGH_SLOT				= (0x4000 + 7),
	IDDK_GAL_ID_SLOT_FULL					= (0x4000 + 8),
	IDDK_GAL_ENROLL_DUPLICATED				= (0x4000 + 9),

	IDDK_TPL_UNAVAILABLE					= (0x5000 + 1),
	IDDK_TPL_CORRUPTED						= (0x5000 + 2), 
	IDDK_TPL_GENERATION_FAILED				= (0x5000 + 3), 
	IDDK_TPL_COMPARISON_FAILED				= (0x5000 + 4),
	IDDK_TPL_TYPE_INVALID					= (0x5000 + 5),
	
	IDDK_ALG_VERSION_INVALID				= (0x6000 + 1),
	IDDK_ALG_FAILED							= (0x6000 + 2),	
	IDDK_ALG_NOT_INIT						= (0x6000 + 3),

	IDDK_CHECK_EYE_LIVENESS_FAILED			= (0x7000 + 1),
	IDDK_CHECK_EYE_SIDE_FAILED				= (0x7000 + 2),
	IDDK_CHECK_EYE_ORIENTATION_FAILED		= (0x7000 + 3),
}IddkResult;

typedef enum _IddkCommStd_
{
	IDDK_COMM_USB	= 0x01,
	IDDK_COMM_UART	= 0x02
}IddkCommStd;

typedef enum _IddkCaptureMode_
{
	IDDK_TIMEBASED = 0x01,
	IDDK_FRAMEBASED
}IddkCaptureMode;

typedef enum _IddkCaptureOperationMode_
{
	IDDK_AUTO_CAPTURE = 0x01,
	IDDK_OPERATOR_INITIATED_AUTO_CAPTURE
}IddkCaptureOperationMode;

typedef enum _IddkQualityMode_
{
	IDDK_QUALITY_NORMAL = 0x01,
	IDDK_QUALITY_HIGH,
	IDDK_QUALITY_VERY_HIGH
}IddkQualityMode;


typedef enum _IddkCaptureStatus_
{
	IDDK_IDLE		= 0x00,
	IDDK_READY		= 0x01,
	IDDK_CAPTURING	= 0x02,	
	IDDK_COMPLETE	= 0x03,
	IDDK_ABORT		= 0x05,
}IddkCaptureStatus;


typedef enum _IddkIsoRevision_
{
	IDDK_IISO_UNKNOWN = 0x00,
	IDDK_IISO_2005,
	IDDK_IISO_2011
}IddkIsoRevision;

typedef enum _IddkImageKind_
{
	IDDK_IKIND_UNKNOWN	= 0x00,
	IDDK_IKIND_K1		= 0x01,
	IDDK_IKIND_K2		= 0x02,
	IDDK_IKIND_K3		= 0x03,
	IDDK_IKIND_K7		= 0x07,
	IDDK_IKIND_K7_1_5	= 0x1007,
	IDDK_IKIND_K7_2_5	= 0x1008,
	IDDK_IKIND_K7_3_5	= 0x1009,
	IDDK_IKIND_K7_5_0	= 0x100A,
	IDDK_IKIND_K7_2_0	= 0x100B,
	IDDK_IKIND_K7_7_0	= 0x100C,
	IDDK_IKIND_K7_10_0	= 0x100D,

}IddkImageKind;

typedef enum _IddkImageFormat_
{	
	IDDK_IFORMAT_MONO_RAW			= 0x02,
	IDDK_IFORMAT_MONO_JPEG			= 0x06,
	IDDK_IFORMAT_MONO_JPEG2000		= 0x0A,
	IDDK_IFORMAT_MONO_PNG			= 0x0E,
	IDDK_IFORMAT_IRITECH_RAW		= (0x80 | IDDK_IFORMAT_MONO_RAW),
	IDDK_IFORMAT_IRITECH_JPEG2000	= (0x80 | IDDK_IFORMAT_MONO_JPEG2000)
}IddkImageFormat;


typedef enum _IddkEyeSubtype_
{
	IDDK_UNKNOWN_EYE = 0x0,
	IDDK_RIGHT_EYE,
	IDDK_LEFT_EYE,
	IDDK_BOTH_EYE,
}IddkEyeSubtype;


typedef enum _IddkProductId_
{
	UNKNOWN = 0x0,

	IRISHIELD_M2120		= 0xF001,
	IRISHIELD_M2121		= 0xF002,
	IRISHIELD_B2121		= 0xF005,

	IRISHIELD_UART_M2120 = 0xF008,
	IRISHIELD_UART_M2121 = 0xF009,

	IRIMAGIC_1000BO		= 0x8002,
	IRIMAGIC_1000BK		= 0x8004,
	IRIMAGIC_1000MO		= 0x8001,
	IRIMAGIC_1000MK		= 0x8003,
	
	IRIHERALD_1000MO	= 0x8403,
	IRIHERALD_1000MK	= 0x8404,
	IRIHERALD_2021UM	= 0x2caa,
	
	IRIMAGIC_1200MO		= 0x8005,
	IRIMAGIC_1200MK		= 0x8007,
	IRIMAGIC_1200BO		= 0x8006,
	IRIMAGIC_1200BK		= 0x8008,
}IddkProductId;

typedef enum _IddkSleepCode_
{
	IDDK_PM_STANDBY		= 0x1,
	IDDK_PM_SLEEP		= 0x2,
	IDDK_PM_DEEPSLEEP	= 0x4
}IddkSleepCode;

typedef enum _IddkRecoveryCode_
{
	IDDK_USB_CANCEL_IO,
	IDDK_USB_RESET_PIPES,
	IDDK_USB_RESET_PORT,
	IDDK_USB_CYCLE_PORT,
	IDDK_SOFT_RESET
}IddkRecoveryCode;

typedef enum _IddkLedId_
{
	IDDK_LED_GREEN	= 0x03,
	IDDK_LED_RED	= 0x04,
	IDDK_LED_ORANGE = 0x06,
	IDDK_LED_BLUE	= 0x0A
}IddkLedId;

typedef enum _IddkFlowcontrol_
{
	IDDK_UART_FC_NONE,
	IDDK_UART_FC_HW	
}IddkFlowcontrol;

typedef enum _IddkEncryptionMode_
{
	IDDK_ENCRYPTION_NONE = 0x00,
	IDDK_ENCRYPTION_FORCE = 0x01
} IddkEncryptionMode;

typedef enum _IddkSystemRole_
{
	IDDK_SYS_ROLE_ADMIN 	= 0x00,
	IDDK_SYS_ROLE_SUPERUSER = 0x01,
	IDDK_SYS_ROLE_USER		= 0x02
} IddkSystemRole;

/******* Device Features *********

 Every IriShield device has auto capturing feature as default.
 However, depending on the requirements of specific customer, device
 may or may not have the features of
 template generation or template comparison.
 IDDK_DEVICE_FEATURE_TEMPLATE_GENERATION: device supports template generation
 IDDK_DEVICE_FEATURE_TEMPLATE_COMPARISON: device supports a template gallery and template comparison

********************************/

#define IDDK_DEVICE_FEATURE_TEMPLATE_GENERATION	(1 << 0)
#define IDDK_DEVICE_FEATURE_TEMPLATE_COMPARISON	(1 << 1)

#endif
