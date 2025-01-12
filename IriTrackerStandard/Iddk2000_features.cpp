#include "Iddk2000_features.h"
#include "IriLivenessBase.h"
#include <string>
#include <QObject>
#include <QDebug>

using namespace IriLivenessDetector;

/* The current handle to device, this handle can be initialized by using get_device_handle() */
HIRICAMM g_hDevice = NULL;
char g_binDir[256] = "D:/IrisTech/Project/IriTrackerStandard/images";
unsigned int error_level = -1;
bool g_deviceSleep = false;
bool g_deviceDeepSleep = false;
IddkEyeSubtype g_selectEyeMode = IDDK_UNKNOWN_EYE;
bool g_isBinocular = false;
bool g_isUsbDevice = false;

void reset_error_level(IddkResult result)
{
	if (result == IDDK_OK)
	{
		error_level = -1;
		if (g_deviceSleep) g_deviceSleep = false;
		if (g_deviceDeepSleep) g_deviceDeepSleep = false;
	}
}


void reset_error_level_custom(IddkResult result)
{
	if (result == IDDK_OK)
	{
		error_level = -1;
		if (g_deviceSleep) g_deviceSleep = false;
		if (g_deviceDeepSleep) g_deviceDeepSleep = false;
	}
}

/*************************************************************************
/*  After each operation, user should clear any current captured images so
/*  that other person cannot use your iris image.
/************************************************************************/
void clear_capture()
{
	IddkResult iRet = IDDK_OK;
	// default to clear both captured eyes for binocular device
	iRet = Iddk_ClearCapture(g_hDevice, IDDK_UNKNOWN_EYE);
	if (iRet == IDDK_OK)
	{
		//printf("Clear capture successfully\n");
	}
	else
	{
		printf("Clear capture failed \n");
		handle_error(iRet);
	}
}

/*************************************************************************
/* This function check the quality of the current captured images
/* forEnrollment: true if the captured image is for enrollment, otherwise it is for verification/identification.
/* isGrayZone: The output parameter that receives true if at least one eye has quality in the grayzone
/* numAcceptableEyes: The output parameter to receive the number of iris images that have acceptable qualities
/* Return: false if there is no iris image with acceptable quality.
/*************************************************************************/

bool check_image_quality(bool forEnrollment, bool& isGrayZone, int& numAcceptableEyes)
{
	bool bRet = false;
	IddkIrisQuality* pQualities = NULL;
	int nQualityCount = 0;
	numAcceptableEyes = 0;
	int nBadTotalScore = forEnrollment ? 50 : 30;
	int nBadUsableArea = forEnrollment ? 50 : 30;
	int nGoodTotalScore = 70;
	int nGoodUsableArea = 70;

	IddkResult iRet = Iddk_GetResultQuality(g_hDevice, &pQualities, &nQualityCount);

	if (iRet == IDDK_OK || iRet == IDDK_SE_LEFT_FRAME_UNQUALIFIED || iRet == IDDK_SE_RIGHT_FRAME_UNQUALIFIED)
	{
		if (forEnrollment)
		{
			// at least one eye's quality is in grayzone
			isGrayZone = ((pQualities[RIGHT_EYE_IDX].totalScore > nBadTotalScore && pQualities[RIGHT_EYE_IDX].usableArea > nBadUsableArea
				&& (pQualities[RIGHT_EYE_IDX].totalScore <= nGoodTotalScore || pQualities[RIGHT_EYE_IDX].usableArea <= nGoodUsableArea))
				|| (pQualities[LEFT_EYE_IDX].totalScore > nBadTotalScore && pQualities[LEFT_EYE_IDX].usableArea > nBadUsableArea
					&& (pQualities[LEFT_EYE_IDX].totalScore <= nGoodTotalScore || pQualities[LEFT_EYE_IDX].usableArea <= nGoodUsableArea)));
		}
		else
		{
			//For verification there is no grayzone, just one threshold (30) 
			isGrayZone = false;
		}

		// number of eyes with acceptable quality (not bad)
		if (pQualities[RIGHT_EYE_IDX].totalScore > nBadTotalScore && pQualities[RIGHT_EYE_IDX].usableArea > nBadUsableArea)
			numAcceptableEyes++;

		if (pQualities[LEFT_EYE_IDX].totalScore > nBadTotalScore && pQualities[LEFT_EYE_IDX].usableArea > nBadUsableArea)
			numAcceptableEyes++;

		if (numAcceptableEyes == 0)
		{
			//Clear all captured iris images in the camera device
			Iddk_ClearCapture(g_hDevice, IDDK_UNKNOWN_EYE);
			printf("\nNo captured iris image has acceptable quality for the %s. Please try to capture your iris(es) again!\n", forEnrollment ? "enrollment" : "matching");
			bRet = false;
			goto RETSEC;
		}

		if (g_isBinocular)
		{
			if (iRet != IDDK_SE_RIGHT_FRAME_UNQUALIFIED && (pQualities[RIGHT_EYE_IDX].totalScore <= nBadTotalScore || pQualities[RIGHT_EYE_IDX].usableArea <= nBadUsableArea))
			{
				// clear right eye
				if (Iddk_ClearCapture(g_hDevice, IDDK_RIGHT_EYE))
				{
					bRet = false;
					goto RETSEC;
				}
				printf("\nThe right iris image has bad quality. It was cleared!\n");
			}
			if (iRet != IDDK_SE_LEFT_FRAME_UNQUALIFIED && (pQualities[LEFT_EYE_IDX].totalScore <= nBadTotalScore || pQualities[LEFT_EYE_IDX].usableArea <= nBadUsableArea))
			{
				// clear left eye
				if (Iddk_ClearCapture(g_hDevice, IDDK_LEFT_EYE))
				{
					bRet = false;
					goto RETSEC;
				}
				printf("\nThe left iris image has bad quality. It was cleared!\n");
			}
		}
	}
	else
	{
		handle_error(iRet);
		goto RETSEC;
	}
	bRet = true;
RETSEC:
	reset_error_level(iRet);
	return bRet;
}

/*************************************************************************
/*	This function demonstrates how to login to the device as an Administrator or
/*	a Superuser so that you can control the device as your wish. To login as
/*	an Administrator, you have to enroll your irises to the device using function
/*	Iddk_EnrollAsAdmin first. To login as a Superuser, you have to enroll
/*	your irises using Iddk_EnrollCapture first and then set user role using
/*	Iddk_SetUserRole.
/*************************************************************************/
void login()
{
	bool isAdmin = true;
	char enrollID[255];
	IddkResult iRet = IDDK_OK;
	IddkCaptureStatus captureStatus;
	bool isGrayZone = false;
	int numAcceptableEyes = 0;

	/* For LoadGallery */
	char** pEnrollIds = NULL;
	unsigned int nEnrollIds = 0;
	unsigned int nUsedSlots = 0;
	unsigned int nMaxSlots = 0;

	/* Check as if the device is opened */
	if (g_hDevice == NULL) return;

	iRet = Iddk_GetCaptureStatus(g_hDevice, &captureStatus);
	if (iRet == IDDK_OK)
	{
		if (captureStatus != IDDK_COMPLETE)
		{
			/* Start a capturing process */
			printf("\nTo login the device, you need to capture your iris first. Capturing process starts ... \n");
			capturing_process(true, false, false);
		}
	}
	else
	{
		handle_error(iRet);
		goto exitsec;
	}


	if (!check_image_quality(false, isGrayZone, numAcceptableEyes))
	{
		goto exitsec;
	}
	/* Ask user first */
	printf("\nLogin as: \n\t1. Administrator (default)\n\t2. Superuser\n");
	printf("Enter your choice: ");
	switch (choose_option(2))
	{
	case -1:
		printf("1\n");
	case 1:
		isAdmin = true;
		break;
	case 2:
		isAdmin = false;
	}

	/* Enter ID */
	printf("\nEnter %s ID (less than 32 characters): ", isAdmin ? "Administrator" : "Superuser");
	gets_s(enrollID);

	if (!isAdmin)
	{
		//Load gallery before login as SuperUser
		printf("\nLoading gallery ... ");
		iRet = Iddk_LoadGallery(g_hDevice, &pEnrollIds, &nEnrollIds, &nUsedSlots, &nMaxSlots);
		if (iRet != IDDK_OK)
		{
			handle_error(iRet);
			goto exitsec;
		}
		else
		{
			printf("done.\n");
		}
	}

	iRet = Iddk_Login(g_hDevice, enrollID, isAdmin ? IDDK_SYS_ROLE_ADMIN : IDDK_SYS_ROLE_SUPERUSER);
	if (iRet == IDDK_OK)
	{
		printf("\nHi \"%s\" !!! Login successfully!\n", enrollID);
		reset_error_level(iRet);
	}
	else
	{
		printf("\nSorry \"%s\" !!! Login failed!\n", enrollID);
		handle_error(iRet);
	}

exitsec:
	clear_capture();
	printf("\nPlease press ENTER to continue ...\n");
	getchar();
}

/**************************************************************************
/*	This functions demonstrates how to logout from the device.
/*************************************************************************/
void logout()
{
	IddkResult iRet = IDDK_OK;

	/* Check as if the device is opened */
	if (g_hDevice == NULL) return;

	iRet = Iddk_Logout(g_hDevice);
	if (iRet == IDDK_OK)
	{
		printf("Logout successfully!\n");

		/* Remember to do that */
		clear_capture();
	}
	else
	{
		printf("Logout failed!\n");
		handle_error(iRet);
	}

	reset_error_level(iRet);

	printf("\nPlease press ENTER to continue ...\n");
	getchar();
}

/**************************************************************************
/*	This function demonstrates how to configure the device. Note that, this
/*	function is only used by an Administrator, so login as Administrator first.
/*  Device configuration parameters:
/*	1.	Encryption mode
/*	2.	Deduplication
/*	3.	Supervised enrollment
/*	4.	Iris data closure
/*	5.	Deduplication/Authentication threshold
/*	6	Stream images enable
/*	7.	Stream scale
/*	8.  Stream format
/*	9.	Baudrate
/**************************************************************************/
void device_configuration()
{
	int option = -1;
	IddkDeviceConfig devConfig;
	IddkResult iRet = IDDK_OK;
	unsigned char deleteCustCer = 0;
	unsigned char regenCamCer = 0;
	bool exitProgram = false;
	char enrollID[255];
	IddkCaptureStatus captureStatus;
	bool isGrayZone = false;
	int numAcceptableEyes = 0;

	_menu_list device_config_menu = {
		"Main Menu",
		"Get Device Configuration",
		"Set Device Configuration",
		"Lock Device",
		"Unlock Device",
		"Reset Certificates",
		"Unenroll Admin",
		"Enroll Admin",
		"Exit"
	};

	/* Check as if the device is opened */
	if (g_hDevice == NULL) return;

	while (true)
	{
		printf("\nDEVICE MANAGEMENT: Please select one menu item below\n");
		option = display_menu(device_config_menu, sizeof(device_config_menu) / sizeof(device_config_menu[0]), -1);

		if (option != 9 && option != -1 && option != 1)
		{
			printf("\n\n**************** %s ****************\n\n", device_config_menu[option - 1]);
		}

		switch (option)
		{
		case -1:
			continue;
		case 1:
			/* Back to main menu */
			clear_capture();

			goto RETSEC;
		case 2:
			/* Get device configuration */
			iRet = Iddk_GetDeviceConfig(g_hDevice, &devConfig);
			if (iRet == IDDK_OK)
			{
				printf("\nCurrent device configuration:\n");
				printf("\t+ Encryption mode: %s\n", devConfig.encryptionMode == IDDK_ENCRYPTION_NONE ? "No encryption" : "Forced encryption");
				printf("\t+ Deduplication mode: %s\n", devConfig.enableDeduplication ? "Enabled" : "Disabled");
				printf("\t+ Deduplication threshold: %f\n", devConfig.deduplicationThreshold);
				printf("\t+ Authentication threshold: %f\n", devConfig.authenThreshold);
				printf("\t+ Supervised enrollment: %s\n", devConfig.supervisedEnrollment ? "Enabled" : "Disabled");
				printf("\t+ Iris data closure: %s\n", devConfig.dataClosure ? "Enabled" : "Disabled");
				printf("\t+ Stream images: %s\n", devConfig.enableStream ? "Yes" : "No");
				if (devConfig.enableStream)
				{
					printf("\t+ Stream scale: %d\n", devConfig.streamScale);
					const char* strImageFormat = "Unknown";
					if (devConfig.streamFormat == IDDK_IFORMAT_MONO_RAW)
						strImageFormat = "IDDK_IFORMAT_MONO_RAW";
					else if (devConfig.streamFormat == IDDK_IFORMAT_MONO_JPEG)
						strImageFormat = "IDDK_IFORMAT_MONO_JPEG";

					printf("\t+ Stream format: %s\n", strImageFormat);
				}
				printf("\t+ UART Baudrate: %d\n", devConfig.baudrate);
				printf("\t+ Compression Quality: %d\n", devConfig.compressionQuality);
				printf("\t+ UART Hardware Flow Control: %s\n", (devConfig.flowControl != 0) ? "Enabled" : "Disabled");
				if (devConfig.sleepMode != 0)
				{
					if (devConfig.sleepMode == IDDK_PM_STANDBY || devConfig.sleepMode == IDDK_PM_SLEEP || devConfig.sleepMode == IDDK_PM_DEEPSLEEP)
					{
						printf("\t+ Sleep Mode: %s\n", (devConfig.sleepMode == IDDK_PM_STANDBY) ? "Stanby" : ((devConfig.sleepMode == IDDK_PM_SLEEP) ? "Sleep" : "Deepsleep"));
						printf("\t+ Sleep Timeout: %d\n", devConfig.sleepTimeout);
					}
				}
			}
			break;
		case 3:
			/* Get device configuration */
			iRet = Iddk_GetDeviceConfig(g_hDevice, &devConfig);
			if (iRet != IDDK_OK) break;

			/* Set device configuration */
			printf("\nSet encryption mode?\n\t1. No encryption\n\t2. Force encryption\n");
			printf("Enter your choice (Press ENTER to skip): ");
			switch (choose_option(2))
			{
			case -1:
				printf("Current value: %s\n", devConfig.encryptionMode == IDDK_ENCRYPTION_NONE ? "No encryption" : "Force encryption");
				break;
			case 1:
				devConfig.encryptionMode = IDDK_ENCRYPTION_NONE;
				break;
			case 2:
				devConfig.encryptionMode = IDDK_ENCRYPTION_FORCE;
			}

			printf("\nSet deduplication mode?\n\t1. Disable\n\t2. Enable\n");
			printf("Enter your choice (Press ENTER to skip): ");
			switch (choose_option(2))
			{
			case -1:
				printf("Current value: %s\n", devConfig.enableDeduplication ? "Enabled" : "Disabled");
				break;
			case 1:
				devConfig.enableDeduplication = 0;
				break;
			case 2:
				devConfig.enableDeduplication = 1;
			}

			if (devConfig.enableDeduplication)
			{
				devConfig.deduplicationThreshold = read_float("\nSet deduplication threshold (Press [ENTER] for current value): ", 0.8f, 1.1f, devConfig.deduplicationThreshold);
			}

			devConfig.authenThreshold = read_float("\nSet authentication threshold (Press [ENTER] for current value): ", 0.8f, 1.1f, devConfig.authenThreshold);

			printf("\nSet supervised enrollment mode?\n\t1. Disable\n\t2. Enable\n");
			printf("Enter your choice (Press ENTER to skip): ");
			switch (choose_option(2))
			{
			case -1:
				printf("Current value: %s\n", devConfig.supervisedEnrollment ? "Enabled" : "Disabled");
				break;
			case 1:
				devConfig.supervisedEnrollment = 0;
				break;
			case 2:
				devConfig.supervisedEnrollment = 1;
			}

			printf("\nSet iris data closure mode?\n\t1. Disable\n\t2. Enable\n");
			printf("Enter your choice (Press ENTER to skip): ");
			switch (choose_option(2))
			{
			case -1:
				printf("Current value: %s\n", devConfig.dataClosure ? "Enabled" : "Disabled");
				break;
			case 1:
				devConfig.dataClosure = 0;
				break;
			case 2:
				devConfig.dataClosure = 1;
			}

			printf("\nDo you want to stream images?\n\t1. Yes\n\t2. No\n");
			printf("Enter your choice (Press ENTER to skip): ");
			switch (choose_option(2))
			{
			case -1:
				printf("Current value: %s\n", devConfig.enableStream ? "Yes" : "No");
				break;
			case 1:
				devConfig.enableStream = 1;
				break;
			case 2:
				devConfig.enableStream = 0;
			}

			if (devConfig.enableStream)
			{
				printf("\nSet stream scale?\n\t1. 1\n\t2. 2\n\t3. 4\n\t4. 8\n\t5. 16\n");
				printf("Enter your choice (Press ENTER to skip): ");
				switch (choose_option(5))
				{
				case -1:
					printf("Current value: %d\n", devConfig.streamScale);
					break;
				case 1:
					devConfig.streamScale = 1;
					break;
				case 2:
					devConfig.streamScale = 2;
					break;
				case 3:
					devConfig.streamScale = 4;
					break;
				case 4:
					devConfig.streamScale = 8;
					break;
				case 5:
					devConfig.streamScale = 16;
				}

				/* For the image format of streaming:
					- In USB connection: Only raw image format is supported.
					- In UART connection: Only Jpeg format is supported.
				*/
			}

			/* Set the device baudrate */
			while (true)
			{
				int baudrate = read_uint("\nEnter UART baudrate (not less than 56000, press ENTER for current value): ", 56000, 916000, devConfig.baudrate);
				if (check_supported_baudrate(baudrate))
				{
					devConfig.baudrate = baudrate;
					break;
				}
				else
				{
					printf("Unsupported baudrate, enter again !\n");
				}
			}

			printf("\nSet UART hardware flow control?\n\t1. Disable\n\t2. Enable\n");
			printf("Enter your choice (Press ENTER to skip): ");
			switch (choose_option(2))
			{
			case -1:
				printf("Current value: %s\n", devConfig.flowControl ? "Enabled" : "Disabled");
				break;
			case 1:
				devConfig.flowControl = 0;
				break;
			case 2:
				devConfig.flowControl = 1;
			}

			devConfig.compressionQuality = read_uint("\nEnter compression quality (Press ENTER to skip): ", 1, 100, devConfig.compressionQuality);

			if (devConfig.sleepMode != 0)
			{
				if (devConfig.sleepMode == IDDK_PM_STANDBY || devConfig.sleepMode == IDDK_PM_SLEEP || devConfig.sleepMode == IDDK_PM_DEEPSLEEP)
				{
					printf("\nSet sleep mode?\n\t1. Standby\n\t2. Sleep\n\t3. Deepsleep\n");
					printf("Enter your choice (Press ENTER to skip): ");
					switch (choose_option(3))
					{
					case -1:
						printf("Current value: %s\n", (devConfig.sleepMode == IDDK_PM_STANDBY) ? "Stanby" : ((devConfig.sleepMode == IDDK_PM_SLEEP) ? "Sleep" : "Deepsleep"));
						break;
					case 1:
						devConfig.sleepMode = IDDK_PM_STANDBY;
						break;
					case 2:
						devConfig.sleepMode = IDDK_PM_SLEEP;
						break;
					case 3:
						devConfig.sleepMode = IDDK_PM_DEEPSLEEP;
					}

					devConfig.sleepTimeout = read_uint("\nEnter sleep timeout (Press ENTER to skip): ", 10, 65535, 0, devConfig.sleepTimeout);
				}
			}

			/* Now, we set the device configuration. Note that, only Administrator can do this */
			iRet = Iddk_SetDeviceConfig(g_hDevice, &devConfig);
			if (iRet == IDDK_OK)
			{
				printf("New device configuration was set successfully !\n");
			}

			break;
		case 4:
			/* Lock device */
			iRet = Iddk_LockDevice(g_hDevice);
			if (iRet == IDDK_OK)
			{
				printf("Device was locked successfully !\n");
			}
			break;
		case 5:
			/* Unlock device */
			iRet = Iddk_UnlockDevice(g_hDevice);
			if (iRet == IDDK_OK)
			{
				printf("Device was unlocked successfully !\n");
			}
			break;
		case 6:
			/* Reset certificates */
			printf("\nDelete customer certificate?\n\t1. No (default)\n\t2. Yes\n");
			printf("Enter your choice: ");
			switch (choose_option(2))
			{
			case -1:
				printf("1\n");
			case 1:
				deleteCustCer = 0;
				break;
			case 2:
				deleteCustCer = 1;
			}

			printf("\nRegenerate camera certificate?\n\t1. No (default)\n\t2. Yes\n");
			printf("Enter your choice: ");
			switch (choose_option(2))
			{
			case -1:
				printf("1\n");
			case 1:
				regenCamCer = 0;
				break;
			case 2:
				regenCamCer = 1;
			}

			if (deleteCustCer || regenCamCer)
			{
				iRet = Iddk_ResetCertificates(g_hDevice, deleteCustCer, regenCamCer);
				if (iRet == IDDK_OK)
				{
					printf("Camera certificates are reset !\n");
				}
			}

			break;
		case 7:
			/* Unenroll Admin */
			printf("\nEnter admin ID (less than 32 characters): ");
			gets_s(enrollID);
			iRet = Iddk_UnenrollAdmin(g_hDevice, enrollID);
			if (iRet == IDDK_OK)
			{
				printf("\nAdmin \"%s\" just unenrolled !\n", enrollID);
			}
			else
			{
				printf("\nUnenroll admin \"%s\" failed !\n", enrollID);
			}
			break;
		case 8:
			/* Enroll Admin */

			/* Check as if the capture finished */
			iRet = Iddk_GetCaptureStatus(g_hDevice, &captureStatus);
			if (iRet == IDDK_OK)
			{
				if (captureStatus != IDDK_COMPLETE)
				{
					printf("\nTo enroll as an Administrator, you need to capture your iris first. Capturing process starts ... \n");
					capturing_process(true, false, false);
				}
			}
			else
				break;
			if (!check_image_quality(true, isGrayZone, numAcceptableEyes))
			{
				break;
			}

			if (g_isBinocular && g_selectEyeMode != IDDK_LEFT_EYE && g_selectEyeMode != IDDK_RIGHT_EYE && numAcceptableEyes == 1)
			{
				printf("User chose to capture both eyes but only one is qualified for the enrollment.\n");
			}
			if (isGrayZone)
			{
				printf("The captured image(s) is enrollable but not in sufficient quality to warrant the best accuracy."
					"\nThe subject is recommended to have his/her iris image recaptured with the eye opened widely.\n");
			}
			if ((g_isBinocular && g_selectEyeMode != IDDK_LEFT_EYE && g_selectEyeMode != IDDK_RIGHT_EYE && numAcceptableEyes == 1)
				|| isGrayZone)
			{
				printf("Do you want to proceed anyway?"
					"\n\t1. Yes.\n\t2. No (default).");
				printf("\nEnter your choice: ");
				option = choose_option(2);
				if (option == -1)
					printf("2\n");
				if (option == -1 || option == 2)
				{
					clear_capture();
					goto RETSEC;
				}
			}
			printf("\nEnter admin ID (less than 32 characters): ");
			gets_s(enrollID);
			iRet = Iddk_EnrollAsAdmin(g_hDevice, enrollID);
			if (iRet == IDDK_OK)
			{
				printf("\nAdmin \"%s\" just enrolled !\n", enrollID);

				clear_capture();
			}
			else
			{
				printf("\nEnroll admin \"%s\" failed !\n", enrollID);
			}

			break;
		case 9:
			exitProgram = true;
			goto RETSEC;
		}

		if (iRet != IDDK_OK)
		{
			handle_error(iRet);
		}

		reset_error_level(iRet);

		printf("\n\n****************************************************\n\n");

		printf("\nPlease press ENTER to continue ...\n");
		getchar();
	}

RETSEC:
	if (exitProgram)
	{
		finalize_device();
		exit(SUCCESS);
	}
}

/*******************************************************************************
/*	This function demonstrates how to get useful information from current device
/*	This function is taken into account:
/*	- Iddk_GetDeviceInfo
/********************************************************************************/
void get_device_information()
{
	unsigned int nDevVersion = 0;
	IddkDeviceInfo deviceInfo;
	IddkResult iRet = IDDK_OK;

	/* Check as if the device is opened */
	if (g_hDevice == NULL) return;

	/* Get device information */
	iRet = Iddk_GetDeviceInfo(g_hDevice, &deviceInfo);
	if (iRet == IDDK_OK)
	{
		printf("\nDevice information: \n");
		printf("\tProduct ID\t\t\t0x%04X\n", deviceInfo.productID);
		printf("\tProduct Name\t\t\t%s\n", (const char*)deviceInfo.productName);
		printf("\tDevice Model\t\t\t%s\n", deviceInfo.isBinocular ? "Binocular" : "Monocular");
		printf("\tSerial Number\t\t\t%s\n", deviceInfo.serialNumber);
		printf("\tDevice Properties Flag\t\t0x%08X\n", deviceInfo.propertyFlag);
		printf("\tKernel Version\t\t\t%d.%d\n", deviceInfo.kernelVersion, deviceInfo.kernelRevision);
		if (deviceInfo.deviceFeatures)
		{
			printf("\tDevice Additional Features:\n");
			if (deviceInfo.deviceFeatures & IDDK_DEVICE_FEATURE_TEMPLATE_GENERATION)
				printf("\t\t- Template generation\n");
			if (deviceInfo.deviceFeatures & IDDK_DEVICE_FEATURE_TEMPLATE_COMPARISON)
				printf("\t\t- Template comparison\n");
		}
	}
	else
	{
		printf("Cannot get device information !\n");
		handle_error(iRet);
	}

	reset_error_level(iRet);

	printf("\nPlease press ENTER to continue ...\n");
	getchar();
}

/**************************************************************************
/*	This function demonstrates how to get useful information from the SDK
/*	These functions are taken into account:
/*	- Iddk_GetSdkVersion
/*	- Iddk_GetSdkDescription
/*	- Iddk_GetSdkConfig
/**************************************************************************/
void get_information()
{
	/* SDK Version */
	unsigned int sdkVersion;
	unsigned short majorNumber;
	unsigned short minorNumber;
	unsigned short buildNumber;

	/* SDK description */
	int strLength = 256;
	char strSdkDesc[256] = { 0 };

	/* SDK configuration */
	IddkConfig iddkConfig;
	IddkResult iRet = IDDK_OK;

	/* Get the SDK version and show it to user */
	printf("\nSDK version:");
	sdkVersion = Iddk_GetSdkVersion();
	majorNumber = (sdkVersion & 0xff000000) >> 24;
	minorNumber = (sdkVersion & 0x00ffff00) >> 16;
	buildNumber = sdkVersion & 0x000000ff;
	printf("%d.%d.%d\n", majorNumber, minorNumber, buildNumber);

	/* Get the SDK description and show it to user */
	printf("\nSDK description: ");

	iRet = Iddk_GetSdkDescription(strSdkDesc, &strLength);
	if (iRet == IDDK_OK)
	{
		printf("%s\n", (char*)strSdkDesc);
	}
	else
	{
		printf("Cannot get SDK description !\n");
		goto RETSEC;
	}

	/* Get the current SDK config */
	printf("\nCurrent SDK configuration:\n");
	iRet = Iddk_GetSdkConfig(&iddkConfig);
	if (iRet == IDDK_OK)
	{
		printf("\tCommunication type: %s\n", (iddkConfig.commStd == IDDK_COMM_USB) ? "USB" : ((iddkConfig.commStd == IDDK_COMM_UART) ? "UART" : "Unknown"));
		printf("\tLogging: %s\n", iddkConfig.enableLog ? "Enabled" : "Disabled");
		if (iddkConfig.commStd == IDDK_COMM_UART)
		{
			printf("\tUART baudrate: %d\n", iddkConfig.uartBaudrate);
		}
	}
	else
	{
		printf("Cannot get SDK configuration !\n");
	}

	get_device_information();

RETSEC:
	if (iRet != IDDK_OK)
	{
		handle_error(iRet);
	}
}

/***********************************************************************************************
/*	This function demonstrates how to access to an attached device. At first, it asks user about
/*	the device information such as: USB or UART, Baudrate in case UART is in use.
/*	It changes the SDK default config with user inputs by using Iddk_SetIddkConfig.
/*	Finally, it opens the attached device to acquire the device handle, g_hDevice.
/*	These functions are taken into account:
/*	- Iddk_GetSdkConfig
/*	- Iddk_SetSdkConfig
/*  - Iddk_ScanDevices
/*	- Iddk_OpenDevice
/*	Note: This function is called only once before user trying to access to device
/************************************************************************************************/
void get_device_handle()
{
	int i = 0;
	int option = -1;
	IddkConfig config;
	IddkResult iRet = IDDK_OK;
	unsigned int baudrate = 115200;
	const char** ppDeviceDescs = NULL;
	int nDeviceCnt = 0;
	char buffer[256];
	_menu_list comm_methods = {
		"USB",
		"UART"
	};

	/* We should get the current configuration before setting new one */
	iRet = Iddk_GetSdkConfig(&config);
	if (iRet != IDDK_OK)
	{
		/* Oops ! Something wrong happens */
		goto RETSEC;
	}

	/* Now we check the configuration of the currently attached device to change the SDK configuration accordingly */
	printf("Your attached device can be accessed through: \n");
	option = display_menu(comm_methods, sizeof(comm_methods) / sizeof(comm_methods[0]), 0);
	switch (option)
	{
	case -1:
		/* User press Enter */
		printf("1\n");
	case 1:
		/* USB */
		config.commStd = IDDK_COMM_USB;
		g_isUsbDevice = true;
		break;
	case 2:
		/* UART */
		config.commStd = IDDK_COMM_UART;
		g_isUsbDevice = false;
	}

	/* Set baudrate if the communication type is UART */
	if (config.commStd == IDDK_COMM_UART)
	{
#ifndef _MSC_VER
		printf("\nSet baudrate, available values: \n");
		printf("\t57600\t  460800\n");
		printf("\t115200\t 500000\n");
		printf("\t230400\t 576000\n");
#endif
		while (true)
		{
			baudrate = read_uint("\nEnter UART baudrate (not less than 56000, press enter for default value 115200): ", 56000, 916000, 115200);
			if (check_supported_baudrate(baudrate))
			{
				config.uartBaudrate = baudrate;
				break;
			}
			else
			{
				printf("Unsupported baudrate, enter again !\n");
			}
		}

		/********************************************************************************************************
		** NOTE: The device must be also set the same baudrate as specified value above via device_configuration
		*********************************************************************************************************/

		printf("\nEnable hardware flow control: \n\t1. Disable (default) \n\t2. Enable");
		printf("\nEnter your choice: ");
		switch (choose_option(2))
		{
		case -1:
			printf("1\n");
		case 1:
			config.uartFlowcontrol = IDDK_UART_FC_NONE;
			break;
		case 2:
			config.uartFlowcontrol = IDDK_UART_FC_HW;
		}
	}

	printf("\nReset on open device: \n\t1. Yes (default)\n\t2. No");
	printf("\nEnter your choice: ");
	switch (choose_option(2))
	{
	case -1:
		printf("1\n");
	case 1:
		config.resetOnOpenDevice = 1;
		break;
	case 2:
		config.resetOnOpenDevice = 0;
	}

	/* Set new SDK configuration */
	iRet = Iddk_SetSdkConfig(&config);
	if (iRet != IDDK_OK)
	{
		printf("\nFailed to set new configuration !\n");
		goto RETSEC;
	}

	/* Now, we can open the device and retrieve the handle */
	/* If USB, we should scan devices first */
	if (config.commStd == IDDK_COMM_USB)
	{
		printf("\nScan devices ... ");
		iRet = Iddk_ScanDevices(&ppDeviceDescs, &nDeviceCnt);
		if (iRet != IDDK_OK)
		{
			if (iRet == IDDK_DEVICE_NOT_FOUND)
			{
				printf("No IriTech devices found.\n");
			}
			goto RETSEC;
		}
		printf("%d device(s) found !\n", nDeviceCnt);
		for (i = 0; i < nDeviceCnt; i++)
		{
			printf("\t%d. %s\n", i + 1, ppDeviceDescs[i]);
		}

		/* Open the first found device */
		printf("\nOpen device %s ... ", ppDeviceDescs[0]);
		iRet = Iddk_OpenDevice(ppDeviceDescs[0], &g_hDevice);
		if (iRet == IDDK_OK)
		{
			printf("done.\n");
			// check type of device is monocular or binocular
			IddkDeviceInfo deviceInfo;
			if (IDDK_OK == Iddk_GetDeviceInfo(g_hDevice, &deviceInfo))
			{
				g_isBinocular = deviceInfo.isBinocular;
			}
		}
		else
		{
			goto RETSEC;
		}
	}
	else /* UART */
	{
		while (true)
		{
#ifdef _MSC_VER
			printf("\nPlease enter the port name (COM1, COM2, etc.): ");
#else
			printf("\nPlease enter the path to UART device node (tty0, ttySAC0, etc.): ");
#endif
			sprintf(buffer, "%s", "");
			gets_s(buffer);
			if (strlen(buffer) > 0) break;
		}
		printf("\nOpen device ... ");
		iRet = Iddk_OpenDevice((const char*)buffer, &g_hDevice);
		if (iRet == IDDK_OK)
		{
			printf("done.\n");
		}
		else
		{
			printf("failed.\n");
		}
	}

RETSEC:
	if (iRet != IDDK_OK)
	{
		handle_error(iRet);
	}

	reset_error_level(iRet);
}

void get_device_handle_custom()
{
	int i = 0;
	int option = -1;
	IddkConfig config;
	IddkResult iRet = IDDK_OK;
	unsigned int baudrate = 115200;
	const char** ppDeviceDescs = NULL;
	int nDeviceCnt = 0;
	char buffer[256];

	/* We should get the current configuration before setting new one */
	iRet = Iddk_GetSdkConfig(&config);
	if (iRet != IDDK_OK)
	{
		/* Oops ! Something wrong happens */
		goto RETSEC;
	}

	/* Now we check the configuration of the currently attached device to change the SDK configuration accordingly */
	config.commStd = IDDK_COMM_USB;
	g_isUsbDevice = true;

	// Reset device
	config.resetOnOpenDevice = 1;

	/* Set new SDK configuration */
	iRet = Iddk_SetSdkConfig(&config);
	if (iRet != IDDK_OK)
	{
		printf("\nFailed to set new configuration !\n");
		goto RETSEC;
	}

	/* Now, we can open the device and retrieve the handle */
	/* If USB, we should scan devices first */
	while (true) {
		printf("\nScan devices ... ");
		iRet = Iddk_ScanDevices(&ppDeviceDescs, &nDeviceCnt);
		if (iRet != IDDK_OK)
		{
			if (iRet == IDDK_DEVICE_NOT_FOUND)
			{
				printf("No IriTech devices found.\n");
			}
			continue;
			//goto RETSEC;
		}
		printf("%d device(s) found !\n", nDeviceCnt);
		for (i = 0; i < nDeviceCnt; i++)
		{
			printf("\t%d. %s\n", i + 1, ppDeviceDescs[i]);
		}

		/* Open the first found device */
		printf("\nOpen device %s ... ", ppDeviceDescs[0]);
		iRet = Iddk_OpenDevice(ppDeviceDescs[0], &g_hDevice);
		if (iRet == IDDK_OK)
		{
			printf("done.\n");
			// check type of device is monocular or binocular
			IddkDeviceInfo deviceInfo;
			if (IDDK_OK == Iddk_GetDeviceInfo(g_hDevice, &deviceInfo))
			{
				g_isBinocular = deviceInfo.isBinocular;
			}
			break;
		}
		else
		{
			continue;
		}
	}

RETSEC:
	if (iRet != IDDK_OK)
	{
		handle_error(iRet);
	}

	reset_error_level(iRet);
}
static void __cdecl capture_proc(
	__in void* pParam,
	__in IddkImage* pImages,
	__in int nMaxEyeSubtypes,
	__in IddkCaptureStatus* pCaptureStatus,
	__in IddkResult* pCaptureError
)
{
	//This doesn't mean there is error to stop the capture process.
	if (pCaptureError)
	{
		handle_error(*pCaptureError);
	}

	if (pCaptureStatus)//Capture status changed
	{
		if (*pCaptureStatus == IDDK_CAPTURING)
		{
			printf("\nEyes are detected!!!\n");
		}
		else if (*pCaptureStatus == IDDK_ABORT)
		{
			printf("Capture aborted!!!\n");
#ifdef _MSC_VER
			//Signal the event to inform the capture process finished
			HANDLE hEvent = (HANDLE)pParam;
			SetEvent(hEvent);
#endif
		}
		else if (*pCaptureStatus == IDDK_COMPLETE)
		{
			printf("Capture finished!!!\n");
#ifdef _MSC_VER
			HANDLE hEvent = (HANDLE)pParam;
			//signal the event to inform the capture process finished
			SetEvent(hEvent);
#endif
		}
	}
	else
		printf(".");

	if (pImages)
	{
		if (nMaxEyeSubtypes == 1)//Mono device
		{
			//Process for unknown eye
			if (pImages[0].imageData != 0)
			{
			}
		}
		else//nMaxEyeSubtypes == 2 (Bino device)
		{
			if (pImages[RIGHT_EYE_IDX].imageData != 0)//If the right eye image data is available
			{
				//Process for right stream image
			}
			if (pImages[LEFT_EYE_IDX].imageData != 0)//If the left eye image data is available
			{
				//Process for left stream image
			}
		}
	}
}

std::string getEyeSideStr(IddkEyeSubtype eyeSide)
{
	switch (eyeSide)
	{
	case IDDK_UNKNOWN_EYE:
		return "Unknown_";
	case IDDK_RIGHT_EYE:
		return "Right_";
	case IDDK_LEFT_EYE:
		return "Left_";
	default:
		break;
	}
}

/********************************************************************************************
/*	This function is a helper one. It is used by capturing_process to get result image after
/*	making a capture
/*********************************************************************************************/
void get_result_image(int times)
{
	/* For result image */
	char eyeFile[256] = "";
	char eyeFilePath[256] = "";
	IddkImage* pResultImages = NULL;
	int nMaxEyeSubtypes = 0;
	int nCompressRatio = 1;
	IddkImageFormat imageFormat = IDDK_IFORMAT_MONO_RAW;
	IddkImageKind imageKind = IDDK_IKIND_K1;
	IddkImage imgData;

	/* Other params */
	IddkResult iRet = IDDK_OK;
	int imageWidth = 640, imageHeight = 480;

	/* Ask user first */
	/*
	printf("\nDo you want to get the result image? \n\t1. No (default)\n\t2. Yes\n");
	printf("Enter your choice: ");
	switch(choose_option(2))
	{
	case -1:
		printf("1\n");
	case 1:
		return;
		break;
	case 2:
		break;
	}
	*/

	//Ask user for image kind
	/*
	printf("\nSelect image kind: \n\t1. Original Image - K1 (default) \n\t2. VGA Image - K2 \n\t3. Cropped Image - K3\n\t4. Cropped and Masked Image - K7\n");
	printf("Enter your choice: ");
	switch(choose_option(4))
	{
	case -1:
		printf("1\n");
	case 1:
		imageKind = IDDK_IKIND_K1;
		break;
	case 2:
		imageKind = IDDK_IKIND_K2;
		break;
	case 3:
		imageKind = IDDK_IKIND_K3;
		break;
	case 4:
		imageKind = IDDK_IKIND_K7;
	}
	*/
	// Default
	imageKind = IDDK_IKIND_K1;

	//Ask user for image format
	/*
	printf("\nSelect image format: \n\t1. Mono JP2 Image (default)\n\t2. Mono Raw Image\n\t3. IriTech JP2 Image\n\t4. IriTech Raw Image\n");
	printf("Enter your choice: ");
	switch(choose_option(4))
	{
	case -1:
		printf("1\n");
	case 1:
		imageFormat = IDDK_IFORMAT_MONO_JPEG2000;
		break;
	case 2:
		imageFormat = IDDK_IFORMAT_MONO_RAW;
		break;
	case 3:
		imageFormat = IDDK_IFORMAT_IRITECH_JPEG2000;
		break;
	case 4:
		imageFormat = IDDK_IFORMAT_IRITECH_RAW;
	}
	*/
	// Default
	imageFormat = IDDK_IFORMAT_MONO_RAW;

	/* get check image options*/
	int checkImageOptions = 0;
	/*
	int checkImageOptions = 0;
	printf("\nDo you want to check the liveness of the result image? \n\t1. No (default)\n\t2. Yes\n");
	printf("Enter your choice: ");
	switch (choose_option(2))
	{
	case -1:
		printf("1\n");
	case 1:
		break;
	case 2:
		checkImageOptions |= IDDK_CHECK_EYE_LIVENESS;
		break;
	}
	*/

	/*
	printf("\nDo you want to check the eye side of the result image? \n\t1. No (default)\n\t2. Yes\n");
	printf("Enter your choice: ");
	switch (choose_option(2))
	{
	case -1:
		printf("1\n");
	case 1:
		break;
	case 2:
		checkImageOptions |= IDDK_CHECK_EYE_SIDE;
		break;
	}
	*/

	/*
	printf("\nDo you want to check the eye orientation of the result image? \n\t1. No (default)\n\t2. Yes\n");
	printf("Enter your choice: ");
	switch (choose_option(2))
	{
	case -1:
		printf("1\n");
	case 1:
		break;
	case 2:
		checkImageOptions |= IDDK_CHECK_EYE_ORIENTATION;
		break;
	}
	*/
	/* We have enough information to get the result image */
	printf("\n\nGet result image ...");
	iRet = Iddk_GetResultImage(g_hDevice, imageKind, imageFormat, nCompressRatio, &pResultImages, &nMaxEyeSubtypes, checkImageOptions);
	if (iRet != IDDK_OK && iRet != IDDK_SE_LEFT_FRAME_UNQUALIFIED && iRet != IDDK_SE_RIGHT_FRAME_UNQUALIFIED)
	{
		printf("Cannot get result image !\n");
		handle_error(iRet);
	}
	else
	{
		printf("done\n");
		if (iRet == IDDK_SE_LEFT_FRAME_UNQUALIFIED)
		{
			printf("\t\tOnly right image is qualified.\n");
		}
		else if (iRet == IDDK_SE_RIGHT_FRAME_UNQUALIFIED)
		{
			printf("\t\tOnly left image is qualified.\n");
		}

		reset_error_level(iRet);

		for (int eyeIdx = 0; eyeIdx < nMaxEyeSubtypes; eyeIdx++)
		{
			if (pResultImages[eyeIdx].imageData == NULL)
				continue;

			char sEyeLabel[256] = "";
			std::string sLivenessLabel = "";
			std::string sSideLabel = "";
			std::string sOrientationLabel = "";
			if (checkImageOptions & IDDK_CHECK_EYE_LIVENESS)
			{
				if (pResultImages[eyeIdx].livenessStatus == EYE_REAL)
				{
					sLivenessLabel = "Real_";
				}
				else if (pResultImages[eyeIdx].livenessStatus == EYE_FAKE)
				{
					sLivenessLabel = "Fake_";
				}
				else if (pResultImages[eyeIdx].livenessStatus == EYE_CONTACT_LENS)
				{
					sLivenessLabel = "Contact_Lens_";
				}
			}
			if (checkImageOptions & IDDK_CHECK_EYE_SIDE)
			{
				sSideLabel = getEyeSideStr(pResultImages[eyeIdx].eyeSide).c_str();
			}
			else
			{
				sSideLabel = (eyeIdx == RIGHT_EYE_IDX) ? "Right_" : "Left_";
				if (nMaxEyeSubtypes == 1)//nMaxEyeSubtypes = 1 for Mono device. nMaxEyeSubtypes = 2 for Bino device
					sSideLabel = "Unknown_";//Because the device is monocular
			}

			if (checkImageOptions & IDDK_CHECK_EYE_ORIENTATION)
			{
				if (pResultImages[eyeIdx].isFlip)
				{
					sOrientationLabel = "Flipped_";
				}
			}

			sprintf(sEyeLabel, "%s%s%s", sLivenessLabel.c_str(), sSideLabel.c_str(), sOrientationLabel.c_str());

			if (imageFormat == IDDK_IFORMAT_MONO_RAW)
			{
				sprintf(eyeFile, "%sEyeImage_%dx%d_%d.raw", sEyeLabel, pResultImages[eyeIdx].imageWidth, pResultImages[eyeIdx].imageHeight, times);
			}
			sprintf(eyeFilePath, "%s/%s", g_binDir, eyeFile);

			return;

			//if(save_file(eyeFilePath, pResultImages[eyeIdx].imageData, pResultImages[eyeIdx].imageDataLen))
			//{
			//	// Cutom
			//	printf("\t\tSaved ./%s\n", eyeFile);
			//}
			//else
			//{
			//	printf("\t\tSaving ./%s failed.\n", eyeFile);
			//}				
		}
	}
}

IddkImage* get_result_image_custom(int times)
{
	/* For result image */
	char eyeFile[256] = "";
	char eyeFilePath[256] = "";
	IddkImage* pResultImages = NULL;
	int nMaxEyeSubtypes = 0;
	int nCompressRatio = 1;
	IddkImageFormat imageFormat = IDDK_IFORMAT_MONO_RAW;
	IddkImageKind imageKind = IDDK_IKIND_K1;
	IddkImage imgData;

	/* Other params */
	IddkResult iRet = IDDK_OK;
	int imageWidth = 640, imageHeight = 480;


	// Default
	imageKind = IDDK_IKIND_K1;

	// Default
	imageFormat = IDDK_IFORMAT_MONO_RAW;

	/* get check image options*/
	int checkImageOptions = 0;

	/* We have enough information to get the result image */
	printf("\n\nGet result image ...");
	iRet = Iddk_GetResultImage(g_hDevice, imageKind, imageFormat, nCompressRatio, &pResultImages, &nMaxEyeSubtypes, checkImageOptions);
	if (iRet != IDDK_OK && iRet != IDDK_SE_LEFT_FRAME_UNQUALIFIED && iRet != IDDK_SE_RIGHT_FRAME_UNQUALIFIED)
	{
		printf("Cannot get result image !\n");
		handle_error(iRet);
	}
	else
	{
		printf("done\n");
		if (iRet == IDDK_SE_LEFT_FRAME_UNQUALIFIED)
		{
			printf("\t\tOnly right image is qualified.\n");
		}
		else if (iRet == IDDK_SE_RIGHT_FRAME_UNQUALIFIED)
		{
			printf("\t\tOnly left image is qualified.\n");
		}

		reset_error_level(iRet);

		for (int eyeIdx = 0; eyeIdx < nMaxEyeSubtypes; eyeIdx++)
		{
			if (pResultImages[eyeIdx].imageData == NULL)
				continue;

			char sEyeLabel[256] = "";
			std::string sLivenessLabel = "";
			std::string sSideLabel = "";
			std::string sOrientationLabel = "";
			if (checkImageOptions & IDDK_CHECK_EYE_LIVENESS)
			{
				if (pResultImages[eyeIdx].livenessStatus == EYE_REAL)
				{
					sLivenessLabel = "Real_";
				}
				else if (pResultImages[eyeIdx].livenessStatus == EYE_FAKE)
				{
					sLivenessLabel = "Fake_";
				}
				else if (pResultImages[eyeIdx].livenessStatus == EYE_CONTACT_LENS)
				{
					sLivenessLabel = "Contact_Lens_";
				}
			}
			if (checkImageOptions & IDDK_CHECK_EYE_SIDE)
			{
				sSideLabel = getEyeSideStr(pResultImages[eyeIdx].eyeSide).c_str();
			}
			else
			{
				sSideLabel = (eyeIdx == RIGHT_EYE_IDX) ? "Right_" : "Left_";
				if (nMaxEyeSubtypes == 1)//nMaxEyeSubtypes = 1 for Mono device. nMaxEyeSubtypes = 2 for Bino device
					sSideLabel = "Unknown_";//Because the device is monocular
			}

			if (checkImageOptions & IDDK_CHECK_EYE_ORIENTATION)
			{
				if (pResultImages[eyeIdx].isFlip)
				{
					sOrientationLabel = "Flipped_";
				}
			}

			sprintf(sEyeLabel, "%s%s%s", sLivenessLabel.c_str(), sSideLabel.c_str(), sOrientationLabel.c_str());

			if (imageFormat == IDDK_IFORMAT_MONO_RAW)
			{
				sprintf(eyeFile, "%sEyeImage_%dx%d_%d.raw", sEyeLabel, pResultImages[eyeIdx].imageWidth, pResultImages[eyeIdx].imageHeight, times);
			}
			sprintf(eyeFilePath, "%s/%s", g_binDir, eyeFile);

			return pResultImages;

			//if(save_file(eyeFilePath, pResultImages[eyeIdx].imageData, pResultImages[eyeIdx].imageDataLen))
			//{
			//	// Cutom
			//	printf("\t\tSaved ./%s\n", eyeFile);
			//}
			//else
			//{
			//	printf("\t\tSaving ./%s failed.\n", eyeFile);
			//}				
		}
	}
}

int getLocaltimeInTimeZone(int timezoneBiasFromUTC /* in minutes */,
	char datetime[20],
	bool haveMillisec)
{
	SYSTEMTIME utcTime = { 0 };
	GetSystemTime(&utcTime);

	TIME_ZONE_INFORMATION timeZone = { 0 };
	timeZone.Bias = -timezoneBiasFromUTC;

	SYSTEMTIME localTime = { 0 };
	int fail = !SystemTimeToTzSpecificLocalTime(&timeZone, &utcTime, &localTime);
	if (fail)
	{
		return fail;
	}

	if (!haveMillisec)
	{
		//Format: YYYY-MM-DDThh:mm:ss (derived from ISO 8601), 'T' as separator.
		//19 bytes is enough for this format + zero-ending byte.
		sprintf(datetime, "%04d-%02d-%02dT%02d:%02d:%02d",
			localTime.wYear, localTime.wMonth, localTime.wDay,
			localTime.wHour, localTime.wMinute, localTime.wSecond);
	}
	else
	{
		//Format: YYYY-MM-DDThh:mm:ss.mmm (derived from ISO 8601), 'T' as separator.
		//19+4 bytes is enough for this format + zero-ending byte.
		sprintf(datetime, "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
			localTime.wYear, localTime.wMonth, localTime.wDay,
			localTime.wHour, localTime.wMinute, localTime.wSecond,
			localTime.wMilliseconds);
	}

	return fail;
}

std::string getUtcTimestamp(const char* postfix, bool haveMillisec)
{
	//UTC_DATETIME_PATTERN = "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'"
	//ie: "2029-03-30T09:50:57.702Z"

	//UTC_DATETIME_PATTERN = "yyyy-MM-dd'T'HH:mm:ssZ"
	//ie: "2029-03-30T09:50:57Z"

	char timestamp[64] = { 0 };
	//Format: YYYY-MM-DDThh:mm:ss
	int fail = getLocaltimeInTimeZone(0, timestamp, haveMillisec);
	(void)fail;
	if (postfix && *postfix)
	{
		strcat(timestamp, postfix);
	}
	return timestamp;
}

inline int is_big_endian(void)
{
	const int n = 1;
	return ((const char*)&n)[0] == 0;
}

template <class T>
void BE_write_integral(void* buffer, T val)
{
	unsigned char* dst = (unsigned char*)buffer;
	if (is_big_endian())
	{
		memcpy(dst, &val, sizeof(val));
	}
	else
	{
		unsigned char* s = (unsigned char*)&val;
		for (size_t i = 0; i < sizeof(val); ++i)
		{
			dst[i] = s[sizeof(val) - i - 1];
		}
	}
}

//REF: copied from https://25.19.218.61:8080/svn/esw/project/dsw/project/irisentinel/branches/2022.03.14-trunk-mds-wolfssl/src/projects/sentinel-service/sentinelserv/IriIsoUtil.h
	//Supported input format:
	//UTC_DATETIME_PATTERN = "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'"
	//ie: "2029-03-30T09:50:57.702Z"
	//UTC_DATETIME_PATTERN = "yyyy-MM-dd'T'HH:mm:ssZ"
	//ie: "2029-03-30T09:50:57Z"
int gen_iso_19794_1_time(unsigned char buf[9], const char* szUtc, unsigned short defaultMilliseconds = 0xFFFF)
{
	//Supported input format: "2029-03-30T09:50:57.702Z"
	//                      : "2021-03-30T06:12:52Z"
	int y, m, d, hh, mn, ss, ms;
	int r;
	r = sscanf(szUtc, "%d-%d-%dT%d:%d:%d.%dZ", &y, &m, &d, &hh, &mn, &ss, &ms);
	if (r != 7)
	{
		r = sscanf(szUtc, "%d-%d-%dT%d:%d:%dZ", &y, &m, &d, &hh, &mn, &ss);
		if (r != 6)
		{
			memset(buf, 0xFF, 9);
			//indicating incomplete format passing in, still output buffer filled with undefined value.
			return -1;
		}
		ms = defaultMilliseconds;
	}

	int idx = 0;
	unsigned short usVal;

	//year
	usVal = y;
	BE_write_integral(buf + idx, usVal);
	idx += sizeof(usVal);

	buf[idx++] = m;
	buf[idx++] = d;
	buf[idx++] = hh;
	buf[idx++] = mn;
	buf[idx++] = ss;

	//millisecond
	usVal = ms;
	BE_write_integral(buf + idx, usVal);
	idx += sizeof(usVal);
	return 0;
}

/********************************************************************************************
/*	This function is a helper one. It is used by capturing_process to get result ISO image
/*	after making a capture
/*********************************************************************************************/
void get_result_ISO_image(int times)
{
	IddkImageFormat imageFormat = IDDK_IFORMAT_MONO_RAW;
	IddkImageKind imageKind = IDDK_IKIND_K1;
	IddkDataBuffer pIsoImage;
	IddkIsoRevision isoRevision = IDDK_IISO_2005;
	IddkEyeSubtype eyeSubtype = IDDK_UNKNOWN_EYE;
	char isoFile[256];
	IddkResult iRet = IDDK_OK;
	int nCompressRatio = 1;

	/* Initialize */
	pIsoImage.data = NULL;
	pIsoImage.dataSize = 0;

	/* Ask user first to fill in some parameters */
	printf("\nDo you want to get result ISO image: \n\t1. No (default)\n\t2. Yes\n");
	printf("Enter your choice: ");
	switch (choose_option(2))
	{
	case -1:
		printf("1\n");
	case 1:
		return;
	case 2:
		break;
	}

	printf("\nSelect image kind: \n\t1. Original Image - K1 (default) \n\t2. VGA Image - K2 \n\t3. Cropped Image - K3\n\t4. Cropped and Masked Image - K7\n");
	printf("Enter your choice: ");
	switch (choose_option(4))
	{
	case -1:
		printf("1\n");
	case 1:
		imageKind = IDDK_IKIND_K1;
		break;
	case 2:
		imageKind = IDDK_IKIND_K2;
		break;
	case 3:
		imageKind = IDDK_IKIND_K3;
		break;
	case 4:
		imageKind = IDDK_IKIND_K7;
	}

	/* K3, K7 only works with ISO_2011 */
	if (imageKind == IDDK_IKIND_K3 || imageKind == IDDK_IKIND_K7)
	{
		isoRevision = IDDK_IISO_2011;
	}
	else
	{
		printf("\nSelect ISO revision: \n\t1. Iso 2005 (default)\n\t2. Iso 2011\n");
		printf("Enter your choice: ");
		switch (choose_option(2))
		{
		case -1:
			printf("1\n");
		case 1:
			isoRevision = IDDK_IISO_2005;
			break;
		case 2:
			isoRevision = IDDK_IISO_2011;
		}
	}

	printf("\nSelect image format: \n\t1. Mono Raw \n\t2. Mono Jpeg2000 (default)\n");
	printf("Enter your choice: ");
	switch (choose_option(2))
	{
	case 1:
		imageFormat = IDDK_IFORMAT_MONO_RAW;
		break;
	case -1:
		printf("2\n");
	case 2:
		imageFormat = IDDK_IFORMAT_MONO_JPEG2000;
		/* Ask compress ratio in case imageFormat = IDDK_IFORMAT_MONO_JPEG2000 */
		nCompressRatio = read_uint("\nEnter compress ratio (enter for default): ", 1, 100, 100);
	}

	printf("\nSpecify eye label: \n\t1. Left Eye \n\t2. Right Eye \n\t3. Unknown eye(default)\n");
	printf("Enter your choice: ");
	switch (choose_option(3))
	{
	case 1:
		eyeSubtype = IDDK_LEFT_EYE;
		break;
	case 2:
		eyeSubtype = IDDK_RIGHT_EYE;
		break;
	case -1:
		printf("3\n");
	case 3:
		eyeSubtype = IDDK_UNKNOWN_EYE;
	}

	/* Get result ISO image */
	printf("\nGet result ISO image ... ");

	std::string captureTime = getUtcTimestamp("Z", true);
	unsigned char iso_19794_1_time[9] = { 0xFF };
	int timeErr = gen_iso_19794_1_time(iso_19794_1_time, captureTime.c_str(), 0x0000);
	IddkDataBuffer isoTimeBuf = { sizeof(iso_19794_1_time), iso_19794_1_time };

	iRet = Iddk_GetResultIsoImage(g_hDevice, isoRevision, imageFormat, imageKind, nCompressRatio, eyeSubtype, &isoTimeBuf, &pIsoImage);
	if (iRet == IDDK_OK || (eyeSubtype == IDDK_UNKNOWN_EYE && iRet == IDDK_SE_LEFT_FRAME_UNQUALIFIED) || (eyeSubtype == IDDK_UNKNOWN_EYE && iRet == IDDK_SE_RIGHT_FRAME_UNQUALIFIED))
	{
		printf("done.\n");
		sprintf(isoFile, "%s/IsoImage_%d.bin", g_binDir, times);
		if (save_file(isoFile, pIsoImage.data, pIsoImage.dataSize))
		{
			printf("\n\tSaved ./IsoImage_%d.bin\n", times);
		}
		else
		{
			printf("\n\tSaving ./IsoImage_%d.bin failed.\n", times);
		}
		reset_error_level(iRet);
	}
	else
	{
		printf("\nCannot get result ISO image !\n");
		handle_error(iRet);
	}
}

std::string getLivenessStatusStr(LIVENESS_STATUS status)
{
	switch (status)
	{
	case IriLivenessDetector::EYE_REAL:
		return "Real Eye";
	case IriLivenessDetector::EYE_FAKE:
		return "Fake Eye";
	case IriLivenessDetector::EYE_CONTACT_LENS:
		return "Contact Len Eye";
	default:
		break;
	}
}

/********************************************************************************************
/*	This function demonstrates how to capture user's eyes, get the result image, and save to
/*	specified folder.
/*	These functions are taken into account:
/*	- Iddk_InitCamera
/*	- Iddk_StartCapture
/*	- Iddk_GetCaptureStatus
/*	- Iddk_GetStreamImage
/*	- Iddk_GetResultImage
/*	- Iddk_GetImageData
/*	- Iddk_GetResultIsoImage
/*	- Iddk_DeinitCamera
/*	@params:
/*	- bDefaultParams: set the default params for StartCapture function or not
/*	- bMultiple: check whether we want to capture many times or just only once
/*	- bProcessResult: If this value is true then this function will get the result images after capturing successful. false for otherwise.
/**********************************************************************************************/

void capturing_process(bool bDefaultParams, bool bMultiple, bool bProcessResult)
{
	/* For streaming images */
	IddkImage* pImages = NULL;
	int nMaxEyeSubtypes = 0;
	IddkEyeSubtype eyeSubtype = IDDK_UNKNOWN_EYE;

	/* Parameters for capturing */
	IddkCaptureMode captureMode = IDDK_TIMEBASED;
	IddkQualityMode qualityMode = IDDK_QUALITY_NORMAL;
	bool autoLeds = true;
	bool bStreamMode = true;
	int iCount = 3;

	/* Other params */
	IddkResult iRet = IDDK_OK;
	IddkCaptureStatus captureStatus = IDDK_IDLE;
	bool bRun = true;
	int imageWidth = 640, imageHeight = 480;
	int i = 0;
	bool eyeDetected = false;
	int option = -1;
	int times = 0;

	IddkIrisQuality* pQualities;
	int nQualityCount = 0;

	/* We have to init camera first */
	printf("\nInit Camera: \n");
	iRet = Iddk_InitCamera(g_hDevice, &imageWidth, &imageHeight);
	if (iRet != IDDK_OK)
	{
		printf("\nFailed to initialize camera\n");
		handle_error(iRet);
		return;
	}
	printf("\n\tImage width: %d\n", imageWidth);
	printf("\tImage height: %d\n", imageHeight);

	/* OK, we capture many times until user exits */
	while (true)
	{
		/* Init variables in inner loop */
		i = 0;
		bRun = true;
		eyeDetected = false;
		times++;

		/* Ask user to fill in all parameters again */
		if (!bDefaultParams)
		{
			prepare_param_for_capturing(captureMode, qualityMode, eyeSubtype, autoLeds, iCount);
		}

		/* Now, we capture user's eyes */
		printf("\nPut your eyes in front of the camera\n");
		iRet = Iddk_StartCapture(g_hDevice, captureMode, iCount, qualityMode, IDDK_AUTO_CAPTURE, eyeSubtype, autoLeds, NULL, NULL);

#ifdef _MSC_VER
		/******************************************************************************************/
		/* Below is an example how to use the Iddk_StartCapture with a callback function to get stream images*/
		/*
		HANDLE hEvent = CreateEvent(0, FALSE, FALSE, 0);
		iRet = Iddk_StartCapture(g_hDevice, captureMode, iCount, qualityMode, IDDK_AUTO_CAPTURE, eyeSubtype, autoLeds, capture_proc, hEvent);
		if(iRet != IDDK_OK)
		{
			handle_error(iRet);
		}

		WaitForSingleObject(hEvent, 30000);

		Iddk_StopCapture(g_hDevice);

		return;
		*/
#endif

		if (iRet != IDDK_OK)
		{
			/* Remember to deinit camera */
			handle_error(iRet);
			goto RETSEC;
		}
		reset_error_level(iRet);
		/*
		Start a loop to check the device status.
		(You can use the callback function capture_proc instead of this while loop).
		*/
		printf("\tScanning for eyes");
		while (bRun)
		{
			if (bStreamMode)
			{
				iRet = Iddk_GetStreamImage(g_hDevice, &pImages, &nMaxEyeSubtypes, &captureStatus);
				if (iRet == IDDK_OK)
				{
					//TODO/////////////////////////////////////////////////////////////////
					//
					// Your code to process stream image.
					//
					///////////////////////////////////////////////////////////////////////
				}
				//If the stream image is not allowed by device configuration
				else if (iRet == IDDK_DEV_FUNCTION_DISABLED)
				{
					bStreamMode = false;
					iRet = Iddk_GetCaptureStatus(g_hDevice, &captureStatus);
				}
				else if (iRet == IDDK_SE_NO_FRAME_AVAILABLE)
				{
					// when Iddk_GetStreamImage returns IDDK_SE_NO_FRAME_AVAILABLE, there are 2 possibilities:
					// 1. The capturing process ended.
					// 2. The capturing process has not started yet
					//So try to check capturing status to know which above possiblity is:
					iRet = Iddk_GetCaptureStatus(g_hDevice, &captureStatus);
				}
			}
			else
			{
				iRet = Iddk_GetCaptureStatus(g_hDevice, &captureStatus);
				wait(100);
			}
			/* If GetStreamImage and GetCaptureStatus cause no error, process the capture status.*/
			if (iRet == IDDK_OK)
			{
				if (captureStatus == IDDK_CAPTURING)
				{
					if (!eyeDetected)
					{
						printf("\n\tEyes are detected.\n");
						eyeDetected = true;
					}
				}
				else if (captureStatus == IDDK_COMPLETE)
				{
					/* capture has finished */
					bRun = false;
				}
				else if (captureStatus == IDDK_ABORT)
				{
					/* capture has been aborted */
					printf("\n\tCapture aborted\n");
					bRun = false;
				}
				else
				{
					printf(".");
					/* We set up a counter to break the loop if user doesn't place the eyes in front of the camera */
					i++;
					if (i > 300)
					{
						bRun = false;
						printf("\n\tOops! No eye detected for so long. The capture process aborted.\n");
					}
				}

				reset_error_level(iRet);
			}
			else
			{
				/* handle error and terminate this capture */
				handle_error(iRet);
				bRun = false;
			}
		}

		/* Try to stop capturing*/
		iRet = Iddk_StopCapture(g_hDevice);
		if (iRet != IDDK_OK)
		{
			handle_error(iRet);
			goto RETSEC;
		}

		// check liveness of captured images.
		//std::vector<IriLivenessDetector::LivenessResult> livenessResults;
  //      float decisionValue = 0;
  //      int ret = Iddk_CheckLivenessResultImage(g_hDevice, livenessResults);
  //      if (ret == IRI_LN_OK || 
  //          ret == IddkResult::IDDK_SE_RIGHT_FRAME_UNQUALIFIED || 
  //          ret == IddkResult::IDDK_SE_LEFT_FRAME_UNQUALIFIED)
  //      {
		//	// check liveness OK
  //      }
  //      else
  //      {
  //          printf("\nCheck Liveness Result Image: %d ", ret);
  //          printf("\n");
  //      }
		//~check liveness.


		printf("The capture process completed\n");
		reset_error_level(iRet);
		iRet = Iddk_GetResultQuality(g_hDevice, &pQualities, &nMaxEyeSubtypes);
		if (iRet == IDDK_OK)
		{
			if (nMaxEyeSubtypes == 1)//monocular device model
			{
				printf("Quality of the current captured image:\n\t1. Total score: %d\n\t2. Usable area: %d\n\t",
					pQualities[0].totalScore, pQualities[0].usableArea);
			}
			else// binocular device model (nMaxEyeSubtypes == 2)
			{
				printf("Quality of the current captured images:\n\t");
				printf("1. Total score of the right eye: %d\n\t2. Usable area of the right eye: %d\n\t",
					pQualities[RIGHT_EYE_IDX].totalScore, pQualities[RIGHT_EYE_IDX].usableArea);
				printf("4. Total score of the left eye: %d\n\t5. Usable area of the left eye: %d\n\t",
					pQualities[LEFT_EYE_IDX].totalScore, pQualities[LEFT_EYE_IDX].usableArea);
			}
		}
		else if (iRet == IDDK_SE_LEFT_FRAME_UNQUALIFIED)
		{
			// binocular device model
			printf("Left eye image is not qualified. Quality of the current captured image:\n\t");
			printf("1. Total score of right eye: %d\n\t2. Usable area of right eye: %d\n\t",
				pQualities[RIGHT_EYE_IDX].totalScore, pQualities[RIGHT_EYE_IDX].usableArea);
		}
		else if (iRet == IDDK_SE_RIGHT_FRAME_UNQUALIFIED)
		{
			// binocular device model
			printf("Right eye image is not qualified. Quality of the current captured image:\n\t");
			printf("1. Total score of left eye: %d\n\t2. Usable area of left eye: %d\n\t",
				pQualities[LEFT_EYE_IDX].totalScore, pQualities[LEFT_EYE_IDX].usableArea);
		}
		else
		{
			handle_error(iRet);
			goto RETSEC;
		}
		reset_error_level(iRet);
		if (bProcessResult && captureStatus == IDDK_COMPLETE)
		{
			/* Get the result image */
			get_result_image(times);

			/* Get result ISO image */
			get_result_ISO_image(times);
		}

		/* iris_recognition calls, we break the loop */
		if (!bMultiple)
		{
			goto RETSEC;
		}

		/* Let's try another capturing? */
		printf("\nTry another capture?\n\t1. Yes (default)\n\t2. No");
		printf("\nEnter your choice: ");
		option = choose_option(2);

		if (option == -1) printf("1\n");
		if (option == 2)
		{
			goto RETSEC;
		}

		/* Number of capturing */
		times++;
	}

RETSEC:
	/* Finally, we deinit the camera */
	iRet = Iddk_DeinitCamera(g_hDevice);
	if (iRet != IDDK_OK)
	{
		handle_error(iRet);
	}

	reset_error_level(iRet);
}

/********************************************************************************************
/*	This function is a helper one. It is used by iris_recognition to get the result template after
/*	making a capture
/*********************************************************************************************/
IddkDataBuffer get_result_template_custom(int times)
{
	IddkResult iRet = IDDK_OK;
	IddkCaptureStatus captureStatus = IDDK_IDLE;
	int option = -1;
	IddkDataBuffer pTemplate;
	char resultTemplateFile[256];

	/* Init */
	pTemplate.data = NULL;
	pTemplate.dataSize = 0;

	/* Do main job here */
	printf("\nGet result template ... ");

	iRet = Iddk_GetResultTemplate(g_hDevice, &pTemplate);
	if (iRet == IDDK_OK || iRet == IDDK_SE_LEFT_FRAME_UNQUALIFIED || iRet == IDDK_SE_RIGHT_FRAME_UNQUALIFIED)
	{
		printf("done.\n");
		sprintf(resultTemplateFile, "%s/ResultTemplate_%d.tpl", g_binDir, times);
		if(save_file(resultTemplateFile, pTemplate.data, pTemplate.dataSize))
		{
			printf("\n\tSaved ./ResultTemplate_%d.tpl.\n", times);
		}
		else
		{
			printf("\n\tSaving ./ResultTemplate_%d.tpl failed.\n", times);
		}
	}
	//else if (iRet == IDDK_SE_NO_FRAME_AVAILABLE)
	//{
	//	qDebug() << "No iris image was captured. Start a capture first!";
	//}
	else
	{
		qDebug() << "failed. ";
		handle_error(iRet);
	}

	return pTemplate;
}

void get_result_template(int times)
{
	IddkResult iRet = IDDK_OK;
	IddkCaptureStatus captureStatus = IDDK_IDLE;
	int option = -1;
	IddkDataBuffer pTemplate;
	char resultTemplateFile[256];

	/* Init */
	pTemplate.data = NULL;
	pTemplate.dataSize = 0;

	/* Do main job here */
	printf("\nGet result template ... ");

	iRet = Iddk_GetResultTemplate(g_hDevice, &pTemplate);
	if (iRet == IDDK_OK || iRet == IDDK_SE_LEFT_FRAME_UNQUALIFIED || iRet == IDDK_SE_RIGHT_FRAME_UNQUALIFIED)
	{
		printf("done.\n");
		sprintf(resultTemplateFile, "%s/ResultTemplate_%d.tpl", g_binDir, times);
		if (save_file(resultTemplateFile, pTemplate.data, pTemplate.dataSize))
		{
			printf("\n\tSaved ./ResultTemplate_%d.tpl.\n", times);
			qDebug() << resultTemplateFile;
		}
		else
		{
			printf("\n\tSaving ./ResultTemplate_%d.tpl failed.\n", times);
		}
	}
	else if (iRet == IDDK_SE_NO_FRAME_AVAILABLE)
	{
		printf("\nNo iris image was captured. Start a capture first!\n");
	}
	else
	{
		printf("failed. ");
		handle_error(iRet);
	}
}


/********************************************************************************************
/*	This function is a helper one. It is used by iris_recognition to enroll current captured
/*	iris to device after making a capture
/*********************************************************************************************/
void enroll_capture()
{
	IddkResult iRet = IDDK_OK;
	IddkCaptureStatus captureStatus = IDDK_IDLE;
	int option = -1;
	char enrollID[255];
	IddkSystemRole userRole = IDDK_SYS_ROLE_ADMIN;
	bool isGrayZone = false;
	int numAcceptableEyes = 0;
	if (!check_image_quality(true, isGrayZone, numAcceptableEyes))
	{
		return;
	}

	if (g_isBinocular && g_selectEyeMode != IDDK_LEFT_EYE && g_selectEyeMode != IDDK_RIGHT_EYE && numAcceptableEyes == 1)
	{
		printf("User chose to capture both eyes but only one is qualified for the enrollment.\n");
	}
	if (isGrayZone) {
		printf("The captured image(s) is enrollable but not in sufficient quality to warrant the best accuracy."
			"\nThe subject is recommended to have his/her iris image recaptured with the eye opened widely.\n");
	}
	if ((g_isBinocular && g_selectEyeMode != IDDK_LEFT_EYE && g_selectEyeMode != IDDK_RIGHT_EYE && numAcceptableEyes == 1)
		|| isGrayZone)
	{
		printf("Do you want to proceed anyway?"
			"\n\t1. Yes.\n\t2. No (default).");
		printf("\nEnter your choice: ");
		option = choose_option(2);
		if (option == -1) printf("2\n");
		if (option == -1 || option == 2)
		{
			clear_capture();
			return;
		}
	}
	/* Ask some questions */
	printf("\nDo you want to enroll the current captured iris image(s) into the gallery? \n");
	printf("\t1. Yes (default)\n\t2. No\n");
	printf("Enter your choice: ");
	option = choose_option(2);
	if (option == -1) printf("1\n");
	if (option == -1 || option == 1)
	{
		printf("\nEnter enrollee ID (less than 32 characters): ");
		gets_s(enrollID);
	}
	else if (option == 2)
	{
		return;
	}

	/* Do main job here */
	printf("\nEnroll iris ... ");
	iRet = Iddk_EnrollCapture(g_hDevice, enrollID);
	if (iRet == IDDK_OK)
	{
		reset_error_level(iRet);
		printf("done.\n");

		printf("\nDo you want to set user role for the current captured iris? \n");
		printf("\t1. No (default)\n\t2. Yes\n");
		printf("Enter your choice: ");
		option = choose_option(2);
		if (option == -1) printf("1\n");
		if (option == -1 || option == 1)
		{
			//Do nothing here
		}
		else
		{
			printf("\nWhich user role? \n");
			printf("\t1. Superuser (default)\n\t2. User\n");
			printf("Enter your choice: ");
			option = choose_option(2);
			switch (option)
			{
			case -1:
				printf("1\n");
			case 1:
				userRole = IDDK_SYS_ROLE_SUPERUSER;
				break;
			case 2:
				userRole = IDDK_SYS_ROLE_USER;
			}

			iRet = Iddk_SetUserRole(g_hDevice, enrollID, userRole);
			if (iRet == IDDK_OK)
			{
				printf("%s is set to %s\n", enrollID, (userRole == IDDK_SYS_ROLE_SUPERUSER) ? "Superuser" : "User");
			}
			else
			{
				handle_error(iRet);
			}
			reset_error_level(iRet);
		}

		/* Commit to gallery */
		iRet = Iddk_CommitGallery(g_hDevice);
		if (iRet != IDDK_OK)
		{
			printf("\nCommitGallery... failed\n");
			handle_error(iRet);
		}
		reset_error_level(iRet);
		clear_capture();
	}
	else
	{
		printf("failed. ");
		if (iRet == IDDK_GAL_ID_SLOT_FULL || iRet == IDDK_GAL_ID_NOT_ENOUGH_SLOT)
		{
			printf("Maximum number of irises for this enrollee has been reached!\n");
		}
		else if (iRet == IDDK_GAL_FULL)
		{
			printf("Gallery is full now!\n");
		}
		else
		{
			handle_error(iRet);
		}
	}
}

/********************************************************************************************
/*	This function is a helper one. It is used by iris_recognition to enroll template
/*********************************************************************************************/
void enroll_template()
{
	IddkResult iRet = IDDK_OK;

	/* For EnrollTemplate */
	IddkDataBuffer pEnrollTemplate;
	char enrollTemplateFile[256];
	char enrollID[255];
	char temp[256];

	/* Init */
	pEnrollTemplate.data = NULL;
	pEnrollTemplate.dataSize = 0;

	printf("\nPlease specify the template file you want to enroll.");
	printf("\nTemplate file (enter empty path to quit): ");
	sprintf(enrollTemplateFile, "%s", "");
	gets_s(enrollTemplateFile);
	/* empty file name, end of template input */
#ifdef _MSC_VER
	if (strlen(enrollTemplateFile) <= 0)
#else
	if (strlen(enrollTemplateFile) <= 0 || enrollTemplateFile[0] == ENTER)
#endif
	{
		return;
	}
	if (strlen(enrollTemplateFile) > 2 && enrollTemplateFile[0] == '.')
	{
		memset(temp, 0x00, 256);
		sprintf(temp, "%s/%s", g_binDir, enrollTemplateFile);
		memset(enrollTemplateFile, 0x00, 256);
		strcpy(enrollTemplateFile, temp);
	}
	printf("Reading file %s...", enrollTemplateFile);
	/* read_file will allocate memory for the file data if needed, remember to release it after you are done with it */
	if (read_file(enrollTemplateFile, pEnrollTemplate.data, (unsigned long&)pEnrollTemplate.dataSize))
	{
		printf("done.\n");
	}
	else
	{
		printf("Cannot read the file !\n");
		return;
	}

	printf("\nEnter enrollee ID (less than 32 characters): ");
	gets_s(enrollID);

	/* We enroll templates now */
	printf("\nEnroll template ... ");
	iRet = Iddk_EnrollTemplate(g_hDevice, enrollID, &pEnrollTemplate);
	if (iRet == IDDK_OK)
	{
		reset_error_level(iRet);
		printf("done\n");
		/* Save the changes */
		printf("Commit gallery changes...");
		iRet = Iddk_CommitGallery(g_hDevice);
		if (iRet != IDDK_OK)
		{
			printf("failed.");
			handle_error(iRet);
		}
		else
		{
			printf("done.\n");
		}
	}
	else
	{
		printf("failed. ");
		if (iRet == IDDK_GAL_ID_SLOT_FULL || iRet == IDDK_GAL_ID_NOT_ENOUGH_SLOT)
		{
			printf("Maximum number of irises for this enrollee has been reached!\n");
		}
		else if (iRet == IDDK_GAL_FULL)
		{
			printf("Galley is full now!\n");
		}
		else
		{
			handle_error(iRet);
		}
	}

	/* Clean data */
	if (pEnrollTemplate.data != NULL)
	{
		free(pEnrollTemplate.data);
		pEnrollTemplate.data = NULL;
	}

	reset_error_level(iRet);
}

/********************************************************************************************
/*	This function is a helper one. It is used by iris_recognition to unenroll template(s)
/*********************************************************************************************/
void unenroll_templates()
{
	char* pUnenrolledID = NULL;
	int option = 0;
	IddkResult iRet = IDDK_OK;
	char enrollID[255];

	printf("Unenroll all templates in gallery?\n\t1. No (default)\n\t2. Yes\n");
	printf("Enter your choice: ");
	option = choose_option(2);
	if (option == -1) printf("1\n");
	if (option == 1 || option == -1)
	{
		/* don't accept default value */
		printf("\nEnter enrollee ID (less than 32 characters): ");
		gets_s(enrollID);
		pUnenrolledID = enrollID;
	}
	if (!pUnenrolledID)
		printf("\nUnenroll all templates ... ");
	else
		printf("\nUnenroll templates of enrollee %s ... ", enrollID);

	iRet = Iddk_UnenrollTemplate(g_hDevice, pUnenrolledID);
	if (iRet == IDDK_OK)
	{
		reset_error_level(iRet);
		printf("done.\n");
		printf("Commit gallery changes...");
		iRet = Iddk_CommitGallery(g_hDevice);
		if (iRet == IDDK_OK)
		{
			printf("done.\n");
		}
		else
		{
			printf("failed.\n");
			handle_error(iRet);
		}
	}
	else
	{
		printf("failed.\n");
		handle_error(iRet);
	}

	reset_error_level(iRet);
}

/********************************************************************************************
/*	This function is a helper one. It is used by iris_recognition to get gallery information
/*********************************************************************************************/
void get_gallery_information()
{
	char** pEnrollIds = NULL;
	unsigned int nEnrollIds = 0;
	unsigned int nUsedSlots = 0;
	unsigned int nMaxSlots = 0;
	IddkResult iRet = IDDK_OK;
	int option = 0;
	unsigned int i = 0;

	printf("Which gallery do you want to get information?\n\t1. User (default)\n\t2. Administrator\n");
	printf("Enter your choice: ");
	option = choose_option(2);
	if (option == -1) printf("1\n");
	if (option == 1 || option == -1)
	{
		printf("\nGet gallery information ... ");
		iRet = Iddk_GetGalleryInfo(g_hDevice, &pEnrollIds, &nEnrollIds, &nUsedSlots, &nMaxSlots);
		if (iRet == IDDK_OK)
		{
			printf("done.\n");
			printf("\nEnrollee IDs: ");
			for (i = 0; i < nEnrollIds; i++)
			{
				printf("'%s' ", pEnrollIds[i]);
			}
			printf("\nNumber of enrollees = %d\n", nEnrollIds);
			printf("Number of used slots = %d\n", nUsedSlots);
			printf("Maximum number of slots = %d\n", nMaxSlots);
		}
		else
		{
			handle_error(iRet);
		}
	}
	else
	{
		printf("\nGet Admin gallery information ... ");
		iRet = Iddk_GetAdminGalleryInfo(g_hDevice, &pEnrollIds, &nEnrollIds, &nUsedSlots, &nMaxSlots);
		if (iRet == IDDK_OK)
		{
			printf("done.\n");
			printf("\nAdmin IDs: ");

			for (i = 0; i < nEnrollIds; i++)
			{
				printf("'%s' ", pEnrollIds[i]);
			}

			printf("\n\nNumber of admins = %d", nEnrollIds);
			printf("\nNumber of used slots = %d\n", nUsedSlots);
			printf("Maximum number of slots = %d\n", nMaxSlots);
		}
		else
		{
			handle_error(iRet);
		}
	}

	reset_error_level(iRet);
}

void process_matching_result(float distance, char* enrollID)
{
	//Success case
	if (distance <= 1.0f)//Match
	{
		if (enrollID)
			printf("\nMatched with '%s'!!!\n", enrollID);
		else
			printf("\nMatched!!!\n");
	}
	else if (distance > 1.1f)// Non-match
	{
		printf("\nNo match found!\n");
	}
	else//greyzone
	{
		// Grey Zone: we are not sure about the result due to bad image quality, users should make a capture again ...
		printf("The quality of this image may be not good enough.\r\nPlease re-capture another image and try again!\n");
	}
}

/****************************************************************************************************
/*	This function is a helper one. It is used by iris_recognition to Iddk_Compare11 or Iddk_Compare1N
/*	or Iddk_Compare11WithTemplate
/****************************************************************************************************/
void compare_templates()
{
	float compareDis = 1000.0f;
	float minDis = 3.0f;
	int nComparisonResults = 0;
	IddkComparisonResult* pComparisonResults = NULL;
	IddkResult iRet = IDDK_OK;
	int option = 0;
	IddkCaptureStatus captureStatus = IDDK_IDLE;
	char enrollID[255];
	int i = 0;
	IddkDataBuffer pEnrollTemplate;
	char templateFile[256];
	char temp[256];
	char minEnrollID[32];
	/* Compare1N */
	char sMaxDistance[100];
	// default maxDistance = 0: return all distances
	float fMaxDistance = 0;
	nComparisonResults = 0;
	pComparisonResults = NULL;

	/* Init */
	pEnrollTemplate.data = NULL;
	pEnrollTemplate.dataSize = 0;

	/* Remember that before doing any identification or verification,
	we should check the quality of the current captured image */
	bool isGrayZone = false;
	int numAcceptableEyes = 0;
	if (!check_image_quality(false, isGrayZone, numAcceptableEyes))
	{
		return;
	}
	printf("\nSelect the kind of comparison: \n");
	printf("\t1. Compare11 (default)\n\t2. Compare1N\n\t3. Compare11WithTemplate\n");
	printf("Enter your choice: ");
	option = choose_option(3);
	switch (option)
	{
	case -1:
		printf("1\n");
	case 1:
		/* Compare11 */
		printf("\nEnter the enrollee ID (less than 32 characters): ");
		gets_s(enrollID);
		printf("Compare the captured iris images with the templates of the enrollee '%s'...", enrollID);
		iRet = Iddk_Compare11(g_hDevice, enrollID, &compareDis);
		if (iRet == IDDK_OK)
		{
			printf("done.\n");
			printf("\n\tCompare Distance = %f\n", compareDis);

			process_matching_result(compareDis, enrollID);

			clear_capture();
		}
		else
		{
			printf("failed.\n");
			handle_error(iRet);
		}
		break;
	case 2:
		printf("\nPlease specify max distance (Enter or value 0 for no maximum): ");
		gets_s(sMaxDistance);
		if (strlen(sMaxDistance) > 0)
		{
			sscanf(sMaxDistance, "%f", &fMaxDistance);
		}

		printf("Compare the captured iris images with all templates in the gallery...");
		iRet = Iddk_Compare1N(g_hDevice, fMaxDistance, &pComparisonResults, &nComparisonResults);

		if (iRet == IDDK_OK)
		{
			printf("done.\n");
			if (nComparisonResults > 0)
			{
				printf("\nComparison result:\n");
				for (i = 0; i < nComparisonResults; i++)
				{
					printf("\tEnrollee ID = %s, Distance = %f \n", pComparisonResults[i].enrollId, pComparisonResults[i].comparisonDistance);
					if (minDis > pComparisonResults[i].comparisonDistance)
					{
						minDis = pComparisonResults[i].comparisonDistance;
						strcpy(minEnrollID, pComparisonResults[i].enrollId);
					}
				}
			}

			process_matching_result(minDis, minEnrollID);

			clear_capture();
		}
		else
		{
			printf("failed.\n");
			handle_error(iRet);
		}
		break;
	case 3:
		/* Compare11WithTemplate */
		printf("\nPlease specify the template file you want to compare.");
		printf("\nTemplate file (enter empty path to quit): ");
		sprintf(templateFile, "%s", "");
		gets_s(templateFile);
		/* empty file name, end of template input */
#ifdef _MSC_VER
		if (strlen(templateFile) <= 0)
#else
		if (strlen(templateFile) <= 0 || templateFile[0] == ENTER)
#endif
		{
			return;
		}
		if (strlen(templateFile) > 2 && templateFile[0] == '.')
		{
			memset(temp, 0x00, 256);
			sprintf(temp, "%s/%s", g_binDir, templateFile);
			memset(templateFile, 0x00, 256);
			strcpy(templateFile, temp);
		}
		printf("Reading file %s...", templateFile);
		/* read_file will allocate memory for the file data if needed, remember to release it after you are done with it */
		if (read_file(templateFile, pEnrollTemplate.data, (unsigned long&)pEnrollTemplate.dataSize))
		{
			printf("done.\n");
		}
		else
		{
			printf("Cannot read the file !\n");
			return;
		}

		printf("Compare the captured iris image with the specified template...");
		iRet = Iddk_Compare11WithTemplate(g_hDevice, &pEnrollTemplate, &compareDis);
		if (iRet == IDDK_OK)
		{
			printf("done.");
			printf("\n\tCompare Distance = %f\n", compareDis);

			process_matching_result(compareDis, NULL);

			clear_capture();
		}
		else if (iRet == IDDK_GAL_EMPTY)
		{
			printf("No match found. The gallery is empty !\n");
		}
		else if (iRet == IDDK_SE_NO_QUALIFIED_FRAME)
		{
			printf("Failed. No qualified image!\n");
		}
		else
		{
			printf("Failed.\n");
			handle_error(iRet);
		}
	}
	reset_error_level(iRet);
}

bool compare_templates_custom(int dataSize, unsigned char* data)
{
	char templateFile[256];
	float compareDis = 1000.0f;
	float minDis = 3.0f;
	int nComparisonResults = 0;
	IddkComparisonResult* pComparisonResults = NULL;
	IddkResult iRet = IDDK_OK;
	int option = 0;
	IddkCaptureStatus captureStatus = IDDK_IDLE;
	char enrollID[255];
	int i = 0;
	IddkDataBuffer pEnrollTemplate;
	char temp[256];
	char minEnrollID[32];
	/* Compare1N */
	char sMaxDistance[100];
	// default maxDistance = 0: return all distances
	float fMaxDistance = 0;
	nComparisonResults = 0;
	pComparisonResults = NULL;

	/* Init */
	pEnrollTemplate.data = NULL;
	pEnrollTemplate.dataSize = 0;

	/* Remember that before doing any identification or verification,
	we should check the quality of the current captured image */
	bool isGrayZone = false;
	int numAcceptableEyes = 0;
	if (!check_image_quality(false, isGrayZone, numAcceptableEyes))
	{
		return false;
	}

	/* empty file name, end of template input */
#ifdef _MSC_VER
	if (strlen(templateFile) <= 0)
#else
	if (strlen(templateFile) <= 0 || templateFile[0] == ENTER)
#endif
	{
		return false;
	}
	pEnrollTemplate.data = data;
	pEnrollTemplate.dataSize = dataSize;

	printf("Compare the captured iris image with the specified template...");
	iRet = Iddk_Compare11WithTemplate(g_hDevice, &pEnrollTemplate, &compareDis);
	if (iRet == IDDK_OK)
	{
		printf("done.");
		printf("\n\tCompare Distance = %f\n", compareDis);

		process_matching_result(compareDis, NULL);

		clear_capture();
		return true;
	}
	else if (iRet == IDDK_GAL_EMPTY)
	{
		printf("No match found. The gallery is empty !\n");
		return false;
	}
	else if (iRet == IDDK_SE_NO_QUALIFIED_FRAME)
	{
		printf("Failed. No qualified image!\n");
		return false;
	}
	else
	{
		printf("Failed.\n");
		handle_error(iRet);
		return false;
	}
reset_error_level(iRet);
}


/*******************************************************************************************
/*	This function demonstrates how to use two important features of IDDK library: template
/*	generation and template matching. We try to cover some major functions in this
/*	category.
/*	These functions are taken into account:
/*	- Iddk_LoadGallery
/*	- Iddk_GetGalleryInfo
/*	- Iddk_GetResultTemplate
/*	- Iddk_GetTemplateInfo
/*	- Iddk_EnrollCapture
/*	- Iddk_EnrollTemplate
/*	- Iddk_UnenrollTemplate
/*	- Iddk_CommitGallery
/*	- Iddk_Compare11
/*	- Iddk_Compare1N
/*	Note: This function works only if attached device supports those two mentioned features
/********************************************************************************************/
void iris_recognition()
{
	_menu_list iris_recognition_menu = {
		"Main Menu",
		"Get Gallery Information",
		"Enroll Captured Iris",
		"Get Result Template",
		"Enroll Template",
		"Unenroll Templates",
		"Matching",
		"Exit"
	};

	/* Common params */
	bool exitProgram = false;
	IddkResult iRet = IDDK_OK;
	int option = -1;
	int times = 0;

	IddkCaptureStatus captureStatus;

	/* For LoadGallery */
	char** pEnrollIds = NULL;
	unsigned int nEnrollIds = 0;
	unsigned int nUsedSlots = 0;
	unsigned int nMaxSlots = 0;

	/* We load gallery first */
	printf("\nLoading gallery ... ");
	iRet = Iddk_LoadGallery(g_hDevice, &pEnrollIds, &nEnrollIds, &nUsedSlots, &nMaxSlots);
	if (iRet == IDDK_OK)
	{
		reset_error_level(iRet);
		printf("done.\n");
	}
	else
	{
		if (iRet == IDDK_UNSUPPORTED_COMMAND)
		{
			/* Device does not support template comparison
			* but it may support template generation.
			* so just continue with other features...
			*/
			printf("\t device doesn't support template comparison.\n");
		}
		else
		{
			handle_error(iRet);
			goto RETSEC;
		}
	}

	while (true)
	{
		/* We have a list of menu here */
		printf("\nIRIS RECOGNITION: Please select one menu item \n");
		option = display_menu(iris_recognition_menu, sizeof(iris_recognition_menu) / sizeof(iris_recognition_menu[0]), -1);

		if (option != 8 && option != -1)
		{
			printf("\n\n**************** %s ****************\n\n", iris_recognition_menu[option - 1]);
		}

		if (option == 3 || option == 4 || option == 7)
		{
			/* Check as if the capture finished */
			iRet = Iddk_GetCaptureStatus(g_hDevice, &captureStatus);
			if (iRet == IDDK_OK)
			{
				reset_error_level(iRet);
				if (captureStatus != IDDK_COMPLETE)
				{
					printf("\nThere is no captured iris image in the device, you need to capture your iris(es) first. Capturing process starts ... \n");
					capturing_process(true, false, false);
				}
			}
			else
			{
				handle_error(iRet);
				continue;
			}
		}

		if (option == -1) continue;
		else if (option == 8)
			/* Exit */
		{
			exitProgram = true;
			break;
		}
		else if (option == 1)
			/* Come back to main menu */
		{
			/* Remember to do that */
			clear_capture();

			break;
		}
		else if (option == 2)
			/* Get gallery information */
		{
			get_gallery_information();
		}
		else if (option == 3)
		{
			/* Enroll current captured iris to device */
			enroll_capture();
		}
		else if (option == 4)
			/* Get result template */
		{
			get_result_template(times);
		}
		else if (option == 5)
			/* Enroll templates */
		{
			enroll_template();
		}
		else if (option == 6) /* Unenroll templates */
		{
			unenroll_templates();
		}
		else if (option == 7) /* Compare templates */
		{
			compare_templates();
		}
		times++;

		/* Let's get back to menu */
		if (option != 8 && option != -1)
		{
			printf("\n\n****************************************************\n\n");
		}

		printf("\nPlease press ENTER to continue ...\n");
		getchar();
	}

RETSEC:

	if (exitProgram)
	{
		finalize_device();
		exit(SUCCESS);
	}
}

/*******************************************************************************************
/*	This function demonstrates how sleep the attached device
/*	This function is taken into account:
/*	- Iddk_SleepDevice
/********************************************************************************************/
void power_management()
{
	IddkResult iRet = IDDK_OK;
	IddkSleepCode action = IDDK_PM_STANDBY;
	bool wakeup = false;
	IddkRecoveryCode recoveryCode = IDDK_USB_RESET_PORT;

	/* Check as if the device is opened */
	if (g_hDevice == NULL) return;

	printf("\nSelect your action? \n\t1. Standby (default)\n\t2. Sleep\n\t3. Deepsleep\n");
	printf("Enter your choice: ");
	switch (choose_option(3))
	{
	case -1:
		printf("1\n");
	case 1:
		action = IDDK_PM_STANDBY;
		break;
	case 2:
		action = IDDK_PM_SLEEP;
		break;
	case 3:
		action = IDDK_PM_DEEPSLEEP;
		break;
	}

	/* The third arg is not used (for future use) */
	iRet = Iddk_SleepDevice(g_hDevice, action, 0);
	if (iRet == IDDK_OK)
	{
		reset_error_level(iRet);
		printf("Device is slept with mode: %s\n", (action == IDDK_PM_STANDBY) ? "Standby" : ((action == IDDK_PM_SLEEP) ? "Sleep" : "Deepsleep"));
		if (action == IDDK_PM_SLEEP)
		{
			g_deviceSleep = true;
		}
		if (action == IDDK_PM_DEEPSLEEP)
		{
			g_deviceDeepSleep = true;

			if (g_isUsbDevice)
			{
				printf("\nDevice is in Deepsleep. Deepsleep device only wakes up by hardware signal. Device needs scanning and opening again after wake-up\n");
			}
			else
			{
				printf("\nDevice is in Deepsleep. Waking it up for the next usage ! ");
			}
		}
	}
	else
	{
		handle_error(iRet);
	}

	printf("\nPlease press ENTER to continue ...\n");
	getchar();
}

/*******************************************************************************************
/*	This function just finalizes device in a safe way. It supports other functions in this
/*	sample code.
/********************************************************************************************/
void finalize_device()
{
	IddkResult iRet = IDDK_OK;

	if (g_hDevice != NULL)
	{
		printf("\nClose device ... ");
		if ((iRet = Iddk_CloseDevice(g_hDevice)) == IDDK_OK)
		{
			printf("done.\n");
		}
		else
		{
			printf("failed.\n");
			handle_error(iRet);
			exit(ERROR);
		}
		g_hDevice = NULL;
	}
}

/*******************************************************************************************
/*	This function handles errors that may occur. Function Iddk_Recovery is called whenever
/*  IDDK_DEVICE_IO_FAILED or IDDK_DEVICE_IO_TIMEOUT or IDDK_DEVICE_IO_DATA_INVALID happens
/********************************************************************************************/
void handle_error(IddkResult error)
{
	char errorMsg[50];
	bool recovery = false;
	IddkRecoveryCode recCode = IDDK_USB_CANCEL_IO;
	IddkResult result = IDDK_OK;
	switch (error)
	{
	case IDDK_FAILED:
		sprintf(errorMsg, "IDDK_FAILED\n");
		break;
	case IDDK_DEVICE_NOT_FOUND:
		sprintf(errorMsg, "IDDK_DEVICE_NOT_FOUND\n");
		break;
	case IDDK_DEVICE_OPEN_FAILED:
		sprintf(errorMsg, "IDDK_DEVICE_OPEN_FAILED\n");
		break;
	case IDDK_DEVICE_NOT_OPEN:
		sprintf(errorMsg, "IDDK_DEVICE_NOT_OPEN\n");
		break;
	case IDDK_DEVICE_ALREADY_OPEN:
		sprintf(errorMsg, "IDDK_DEVICE_ALREADY_OPEN\n");
		break;
	case IDDK_DEVICE_ACCESS_DENIED:
		sprintf(errorMsg, "IDDK_DEVICE_ACCESS_DENIED\n");
		break;
	case IDDK_TOO_MANY_OPEN_DEVICES:
		sprintf(errorMsg, "IDDK_TOO_MANY_OPEN_DEVICES\n");
		break;
	case IDDK_DEVICE_IO_FAILED:
		sprintf(errorMsg, "IDDK_DEVICE_IO_FAILED\n");
		break;
	case IDDK_DEVICE_IO_TIMEOUT:
		sprintf(errorMsg, "IDDK_DEVICE_IO_TIMEOUT\n");
		break;
	case IDDK_DEVICE_IO_DATA_INVALID:
		sprintf(errorMsg, "IDDK_DEVICE_IO_DATA_INVALID\n");
		break;
	case IDDK_UNSUPPORTED_IMAGE_FORMAT:
		sprintf(errorMsg, "IDDK_UNSUPPORTED_IMAGE_FORMAT\n");
		break;
	case IDDK_MEMORY_ALLOCATION_FAILED:
		sprintf(errorMsg, "IDDK_MEMORY_ALLOCATION_FAILED\n");
		break;
	case IDDK_INVALID_MEMORY:
		sprintf(errorMsg, "IDDK_INVALID_MEMORY\n");
		break;
	case IDDK_INVALID_HANDLE:
		sprintf(errorMsg, "IDDK_INVALID_HANDLE\n");
		break;
	case IDDK_INVALID_PARAMETER:
		sprintf(errorMsg, "IDDK_INVALID_PARAMETER\n");
		break;
	case IDDK_AUTHEN_FAILED:
		sprintf(errorMsg, "IDDK_AUTHEN_FAILED\n");
		break;
	case IDDK_NOT_ENOUGH_BUFFER:
		sprintf(errorMsg, "IDDK_NOT_ENOUGH_BUFFER\n");
		break;
	case IDDK_VERSION_INCOMPATIBLE:
		sprintf(errorMsg, "IDDK_VERSION_INCOMPATIBLE\n");
		break;

	case IDDK_THREAD_FAILED:
		sprintf(errorMsg, "IDDK_THREAD_FAILED\n");
		break;
	case IDDK_UNSUPPORTED_COMMAND:
		sprintf(errorMsg, "IDDK_UNSUPPORTED_COMMAND\n");
		break;
	case IDDK_IMAGE_CORRUPTED:
		sprintf(errorMsg, "IDDK_IMAGE_CORRUPTED\n");
		break;
	case IDDK_WRONG_EYE_SUBTYPE:
		sprintf(errorMsg, "IDDK_WRONG_EYE_SUBTYPE\n");
		break;
	case IDDK_UNKNOWN_DEVICE:
		sprintf(errorMsg, "IDDK_UNKNOWN_DEVICE\n");
		break;
	case IDDK_UNEXPECTED_ERROR:
		sprintf(errorMsg, "IDDK_UNEXPECTED_ERROR\n");
		break;

	case IDDK_DEV_OUTOFMEMORY:
		sprintf(errorMsg, "IDDK_DEV_OUTOFMEMORY\n");
		break;
	case IDDK_DEV_NOT_ENOUGH_MEMORY:
		sprintf(errorMsg, "IDDK_DEV_NOT_ENOUGH_MEMORY\n");
		break;
	case IDDK_DEV_INSUFFICIENT_BUFFER:
		sprintf(errorMsg, "IDDK_DEV_INSUFFICIENT_BUFFER\n");
		break;
	case IDDK_DEV_INVALID_LICENSE:
		sprintf(errorMsg, "IDDK_DEV_INVALID_LICENSE\n");
		break;
	case IDDK_DEV_IO_FAILED:
		sprintf(errorMsg, "IDDK_DEV_IO_FAILED\n");
		break;
	case IDDK_DEV_MODULE_NOT_FOUND:
		sprintf(errorMsg, "IDDK_DEV_MODULE_NOT_FOUND\n");
		break;
	case IDDK_DEV_PROC_NOT_FOUND:
		sprintf(errorMsg, "IDDK_DEV_PROC_NOT_FOUND\n");
		break;
	case IDDK_DEV_BAD_DATA:
		sprintf(errorMsg, "IDDK_DEV_BAD_DATA\n");
		break;
	case IDDK_DEV_FUNCTION_DISABLED:
		sprintf(errorMsg, "IDDK_DEV_FUNCTION_DISABLED\n");
		break;
	case IDDK_DEV_LOCKED:
		sprintf(errorMsg, "IDDK_DEV_LOCKED\n");
		break;
	case IDDK_DEV_BUSY:
		sprintf(errorMsg, "IDDK_DEV_BUSY\n");
		break;
	case IDDK_DEV_RUNTIME_EXCEPTION:
		sprintf(errorMsg, "IDDK_DEV_RUNTIME_EXCEPTION\n");
		break;

	case IDDK_SEC_FAILED:
		sprintf(errorMsg, "IDDK_SEC_FAILED\n");
		break;
	case IDDK_SEC_INIT_FAILED:
		sprintf(errorMsg, "IDDK_SEC_INIT_FAILED\n");
		break;
	case IDDK_SEC_WRONG_PASSWORD:
		sprintf(errorMsg, "IDDK_SEC_WRONG_PASSWORD\n");
		break;
	case IDDK_SEC_BAD_LEN:
		sprintf(errorMsg, "IDDK_SEC_BAD_LEN\n");
		break;
	case IDDK_SEC_BAD_DATA:
		sprintf(errorMsg, "IDDK_SEC_BAD_DATA\n");
		break;
	case IDDK_SEC_BAD_ALG:
		sprintf(errorMsg, "IDDK_SEC_BAD_ALG\n");
		break;
	case IDDK_SEC_BAD_KEY:
		sprintf(errorMsg, "IDDK_SEC_BAD_KEY\n");
		break;
	case IDDK_SEC_BAD_SIG:
		sprintf(errorMsg, "IDDK_SEC_BAD_SIG\n");
		break;
	case IDDK_SEC_ENCRYPT_ERROR:
		sprintf(errorMsg, "IDDK_SEC_ENCRYPT_ERROR\n");
		break;
	case IDDK_SEC_DECRYPT_ERROR:
		sprintf(errorMsg, "IDDK_SEC_DECRYPT_ERROR\n");
		break;
	case IDDK_SEC_IMPORT_ERROR:
		sprintf(errorMsg, "IDDK_SEC_IMPORT_ERROR\n");
		break;
	case IDDK_SEC_EXPORT_ERROR:
		sprintf(errorMsg, "IDDK_SEC_EXPORT_ERROR\n");
		break;
	case IDDK_SEC_KEYGEN_ERROR:
		sprintf(errorMsg, "IDDK_SEC_KEYGEN_ERROR\n");
		break;
	case IDDK_SEC_HASH_ERROR:
		sprintf(errorMsg, "IDDK_SEC_HASH_ERROR\n");
		break;
	case IDDK_SEC_SIG_ERROR:
		sprintf(errorMsg, "IDDK_SEC_SIG_ERROR\n");
		break;
	case IDDK_SEC_PRIVILEGE_RESTRICTED:
		sprintf(errorMsg, "IDDK_SEC_PRIVILEGE_RESTRICTED\n");
		break;

	case IDDK_SE_NOT_INIT:
		sprintf(errorMsg, "IDDK_SE_NOT_INIT\n");
		break;
	case IDDK_SE_NO_CAM:
		sprintf(errorMsg, "IDDK_SE_NO_CAM\n");
		break;
	case IDDK_SE_STARTSTOP_CAPTURE_FAILED:
		sprintf(errorMsg, "IDDK_SE_STARTSTOP_CAPTURE_FAILED\n");
		break;
	case IDDK_SE_QM_FAILED:
		sprintf(errorMsg, "IDDK_SE_QM_FAILED\n");
		break;
	case IDDK_SE_NO_FRAME_AVAILABLE:
		sprintf(errorMsg, "IDDK_SE_NO_FRAME_AVAILABLE\n");
		break;
	case IDDK_SE_NO_QUALIFIED_FRAME:
		sprintf(errorMsg, "IDDK_SE_NO_QUALIFIED_FRAME\n");
		break;
	case IDDK_SE_RIGHT_FRAME_UNQUALIFIED:
		sprintf(errorMsg, "IDDK_SE_RIGHT_FRAME_UNQUALIFIED\n");
		break;
	case IDDK_SE_LEFT_FRAME_UNQUALIFIED:
		sprintf(errorMsg, "IDDK_SE_LEFT_FRAME_UNQUALIFIED\n");
		break;
	case IDDK_SE_COMPRESSION_FAILED:
		sprintf(errorMsg, "IDDK_SE_COMPRESSION_FAILED\n");
		break;

	case IDDK_GAL_NOT_INITIALIZED:
		sprintf(errorMsg, "IDDK_GAL_NOT_INITIALIZED\n");
		break;
	case IDDK_GAL_LOAD_FAILED:
		sprintf(errorMsg, "IDDK_GAL_LOAD_FAILED\n");
		break;
	case IDDK_GAL_EMPTY:
		sprintf(errorMsg, "IDDK_GAL_EMPTY\n");
		break;
	case IDDK_GAL_FULL:
		sprintf(errorMsg, "IDDK_GAL_FULL\n");
		break;
	case IDDK_GAL_ID_NOT_EXIST:
		sprintf(errorMsg, "IDDK_GAL_ID_NOT_EXIST\n");
		break;
	case IDDK_GAL_ID_NOT_ENOUGH_SLOT:
		sprintf(errorMsg, "IDDK_GAL_ID_NOT_ENOUGH_SLOT\n");
		break;
	case IDDK_GAL_NOT_ENOUGH_SLOT:
		sprintf(errorMsg, "IDDK_GAL_NOT_ENOUGH_SLOT\n");
		break;
	case IDDK_GAL_ID_SLOT_FULL:
		sprintf(errorMsg, "IDDK_GAL_ID_SLOT_FULL\n");
		break;
	case IDDK_GAL_ENROLL_DUPLICATED:
		sprintf(errorMsg, "IDDK_GAL_ENROLL_DUPLICATED\n");
		break;

	case IDDK_TPL_UNAVAILABLE:
		sprintf(errorMsg, "IDDK_TPL_UNAVAILABLE\n");
		break;
	case IDDK_TPL_CORRUPTED:
		sprintf(errorMsg, "IDDK_TPL_CORRUPTED\n");
		break;
	case IDDK_TPL_GENERATION_FAILED:
		sprintf(errorMsg, "IDDK_TPL_GENERATION_FAILED\n");
		break;
	case IDDK_TPL_COMPARISON_FAILED:
		sprintf(errorMsg, "IDDK_TPL_COMPARISON_FAILED\n");
		break;
	case IDDK_TPL_TYPE_INVALID:
		sprintf(errorMsg, "IDDK_TPL_TYPE_INVALID\n");
		break;

	case IDDK_ALG_VERSION_INVALID:
		sprintf(errorMsg, "IDDK_ALG_VERSION_INVALID\n");
		break;
	case IDDK_ALG_FAILED:
		sprintf(errorMsg, "IDDK_ALG_FAILED\n");
		break;
	case IDDK_ALG_NOT_INIT:
		sprintf(errorMsg, "IDDK_ALG_NOT_INIT\n");
		break;
	default:
		sprintf(errorMsg, "IDDK_UNKNOWN_ERROR\n");
	}
	printf("\nError: %s\n", errorMsg);

	/* Communication error!!!! Many reasons for these.
	Next function call may be failed too
	if we do not recover the communication from this error */

	if (error == IDDK_DEVICE_IO_FAILED || error == IDDK_DEVICE_IO_TIMEOUT || error == IDDK_DEVICE_IO_DATA_INVALID)
	{
		/** if device is in IDDK_PW_SLEEP mode,
		any access to device results in IDDK_DEVICE_IO_TIMEOUT or IDDK_DEVICE_IO_FAILED **/
		if (g_deviceSleep)
		{
			printf("Device may be in sleep mode ! Please wake it up !\n");
			error_level = -1;
			return;
		}

		if (g_deviceDeepSleep)
		{
			printf("Device may be in deepsleep mode ! Please wake it up !\n");
			error_level = -1;
			return;
		}

		/* if device is not in sleep, the communication should be recovered.
		User better chooses yes */

		printf("\nDo you want to recover from error?\n");
		printf("\t1. Yes (default)\n\t2. No\n");
		printf("Enter your choice: ");
		switch (choose_option(2))
		{
		case -1:
			printf("1\n");
		case 1:
			recovery = true;
			break;
		case 2:
			recovery = false;
		}

		if (recovery)
		{

			/* data invalid may be because of corrupted underlying IO buffers.
			We should cancel pending IOs and clear buffers before reset USB pipes.
			*/
			if (error != IDDK_DEVICE_IO_DATA_INVALID && error_level == -1) error_level = 0;

			/* If these errors occur repeatedly,
			we have to increase the severity level of recovery */
			error_level++;
			switch (error_level)
			{
			case 0:
				printf("\nFirst time the error is detected. We suggest to flush invalid data from IO buffer and abort any pending IO operations !\n");
				recCode = IDDK_USB_CANCEL_IO;
				break;
			case 1:
				/* clear stall condition and reset endpoints */
				printf("\nWe suggest to reset USB pipes!\n");
				recCode = IDDK_USB_RESET_PIPES;
				break;
#if _MSC_VER
			case 2:
				/* perform a port reset */
				printf("\nWe suggest to reset USB port!\n");
				recCode = IDDK_USB_RESET_PORT;
				break;
			case 3:

				printf("\nWe suggest to cycle port! Note that users should scan device and open device again !\n");
				recCode = IDDK_USB_CYCLE_PORT;
				break;
			case 4:
				printf("\nWe suggest to soft reset device!\n");
				recCode = IDDK_SOFT_RESET;
				break;
			case 5:
				// We donot support this level recovery at the moment ...
				error_level = -1;
#else
			case 2:
				printf("\nWe suggest to cycle port in this time ! Note that users should scan device and open device again !\n");
				recCode = IDDK_USB_CYCLE_PORT;
				break;
			case 3:
				printf("\nWe suggest to soft reset device in this time !\n");
				recCode = IDDK_SOFT_RESET;
				break;
			case 4:
				// We donot support this level recovery at the moment ...
				error_level = -1;
#endif
			}

			result = Iddk_Recovery(g_hDevice, recCode);
			if (result == IDDK_OK)
			{
				printf("\nRecovery OK !\n");

				if (recCode == IDDK_USB_CYCLE_PORT || recCode == IDDK_SOFT_RESET)
				{
					/* Wait for sure ... */
					printf("Please wait for a while ...\n");
					wait(10000);
					get_device_handle();
				}
			}
			else
			{
				printf("\nRecovery failed !\n");
			}
		}
	}
}

void capturing_process_custom(bool bDefaultParams, bool bMultiple, bool bProcessResult)
{
	/* For streaming images */
	IddkImage* pImages = NULL;
	int nMaxEyeSubtypes = 0;
	IddkEyeSubtype eyeSubtype = IDDK_UNKNOWN_EYE;

	/* Parameters for capturing */
	IddkCaptureMode captureMode = IDDK_TIMEBASED;
	IddkQualityMode qualityMode = IDDK_QUALITY_VERY_HIGH;
	bool autoLeds = true;
	bool bStreamMode = true;
	int iCount = 3;

	/* Other params */
	IddkResult iRet = IDDK_OK;
	IddkCaptureStatus captureStatus = IDDK_IDLE;
	bool bRun = true;
	int imageWidth = 640, imageHeight = 480;
	int i = 0;
	bool eyeDetected = false;
	int option = -1;
	int times = 0;

	IddkIrisQuality* pQualities;
	int nQualityCount = 0;

	/* We have to init camera first */
	printf("\nInit Camera: \n");
	iRet = Iddk_InitCamera(g_hDevice, &imageWidth, &imageHeight);
	if (iRet != IDDK_OK)
	{
		printf("\nFailed to initialize camera\n");
		handle_error(iRet);
		return;
	}
	printf("\n\tImage width: %d\n", imageWidth);
	printf("\tImage height: %d\n", imageHeight);

	/* OK, we capture many times until user exits */
	while (true)
	{
		/* Init variables in inner loop */
		i = 0;
		bRun = true;
		eyeDetected = false;
		times++;

		/* Ask user to fill in all parameters again */
//		if (!bDefaultParams)
//		{
//			prepare_param_for_capturing(captureMode, qualityMode, eyeSubtype, autoLeds, iCount);
//		}

		/* Now, we capture user's eyes */
		printf("\nPut your eyes in front of the camera\n");
		iRet = Iddk_StartCapture(g_hDevice, captureMode, iCount, qualityMode, IDDK_AUTO_CAPTURE, eyeSubtype, autoLeds, NULL, NULL);

#ifdef _MSC_VER
		/******************************************************************************************/
		/* Below is an example how to use the Iddk_StartCapture with a callback function to get stream images*/
		/*
		HANDLE hEvent = CreateEvent(0, FALSE, FALSE, 0);
		iRet = Iddk_StartCapture(g_hDevice, captureMode, iCount, qualityMode, IDDK_AUTO_CAPTURE, eyeSubtype, autoLeds, capture_proc, hEvent);
		if(iRet != IDDK_OK)
		{
			handle_error(iRet);
		}

		WaitForSingleObject(hEvent, 30000);

		Iddk_StopCapture(g_hDevice);

		return;
		*/
#endif

		if (iRet != IDDK_OK)
		{
			/* Remember to deinit camera */
			handle_error(iRet);
			goto RETSEC;
		}
		reset_error_level(iRet);
		/*
		Start a loop to check the device status.
		(You can use the callback function capture_proc instead of this while loop).
		*/
		printf("\tScanning for eyes");
		while (bRun)
		{
			if (bStreamMode)
			{
				iRet = Iddk_GetStreamImage(g_hDevice, &pImages, &nMaxEyeSubtypes, &captureStatus);
				if (iRet == IDDK_OK)
				{
					//TODO/////////////////////////////////////////////////////////////////
					//
					// Your code to process stream image.
					//
					///////////////////////////////////////////////////////////////////////

				}
				//If the stream image is not allowed by device configuration
				else if (iRet == IDDK_DEV_FUNCTION_DISABLED)
				{
					bStreamMode = false;
					iRet = Iddk_GetCaptureStatus(g_hDevice, &captureStatus);
				}
				else if (iRet == IDDK_SE_NO_FRAME_AVAILABLE)
				{
					// when Iddk_GetStreamImage returns IDDK_SE_NO_FRAME_AVAILABLE, there are 2 possibilities:
					// 1. The capturing process ended.
					// 2. The capturing process has not started yet
					//So try to check capturing status to know which above possiblity is:
					iRet = Iddk_GetCaptureStatus(g_hDevice, &captureStatus);
				}
			}
			else
			{
				iRet = Iddk_GetCaptureStatus(g_hDevice, &captureStatus);
				wait(100);
			}
			/* If GetStreamImage and GetCaptureStatus cause no error, process the capture status.*/
			if (iRet == IDDK_OK)
			{
				if (captureStatus == IDDK_CAPTURING)
				{
					if (!eyeDetected)
					{
						printf("\n\tEyes are detected.\n");
						eyeDetected = true;
					}
				}
				else if (captureStatus == IDDK_COMPLETE)
				{
					/* capture has finished */
					bRun = false;
				}
				else if (captureStatus == IDDK_ABORT)
				{
					/* capture has been aborted */
					printf("\n\tCapture aborted\n");
					bRun = false;
				}
				else
				{
					printf(".");
					/* We set up a counter to break the loop if user doesn't place the eyes in front of the camera */
					i++;
					if (i > 300)
					{
						bRun = false;
						printf("\n\tOops! No eye detected for so long. The capture process aborted.\n");
					}
				}

				//reset_error_level(iRet);
			}
			else
			{
				/* handle error and terminate this capture */
				//handle_error(iRet);
				bRun = false;
			}
		}

		/* Try to stop capturing*/
		iRet = Iddk_StopCapture(g_hDevice);
		if (iRet != IDDK_OK)
		{
			//handle_error(iRet);
			//goto RETSEC;
		}

		// check liveness of captured images.
		//std::vector<IriLivenessDetector::LivenessResult> livenessResults;
  //      float decisionValue = 0;
  //      int ret = Iddk_CheckLivenessResultImage(g_hDevice, livenessResults);
  //      if (ret == IRI_LN_OK || 
  //          ret == IddkResult::IDDK_SE_RIGHT_FRAME_UNQUALIFIED || 
  //          ret == IddkResult::IDDK_SE_LEFT_FRAME_UNQUALIFIED)
  //      {
		//	// check liveness OK
  //      }
  //      else
  //      {
  //          printf("\nCheck Liveness Result Image: %d ", ret);
  //          printf("\n");
  //      }
		//~check liveness.


		printf("The capture process completed\n");
		//reset_error_level(iRet);
		iRet = Iddk_GetResultQuality(g_hDevice, &pQualities, &nMaxEyeSubtypes);
		if (iRet == IDDK_OK)
		{
			if (nMaxEyeSubtypes == 1)//monocular device model
			{
				printf("Quality of the current captured image:\n\t1. Total score: %d\n\t2. Usable area: %d\n\t",
					pQualities[0].totalScore, pQualities[0].usableArea);
			}
			else// binocular device model (nMaxEyeSubtypes == 2)
			{
				printf("Quality of the current captured images:\n\t");
				printf("1. Total score of the right eye: %d\n\t2. Usable area of the right eye: %d\n\t",
					pQualities[RIGHT_EYE_IDX].totalScore, pQualities[RIGHT_EYE_IDX].usableArea);
				printf("4. Total score of the left eye: %d\n\t5. Usable area of the left eye: %d\n\t",
					pQualities[LEFT_EYE_IDX].totalScore, pQualities[LEFT_EYE_IDX].usableArea);
			}
		}
		/*
		else if (iRet == IDDK_SE_LEFT_FRAME_UNQUALIFIED)
		{
			// binocular device model
			printf("Left eye image is not qualified. Quality of the current captured image:\n\t");
			printf("1. Total score of right eye: %d\n\t2. Usable area of right eye: %d\n\t",
				pQualities[RIGHT_EYE_IDX].totalScore, pQualities[RIGHT_EYE_IDX].usableArea);
		}
		else if (iRet == IDDK_SE_RIGHT_FRAME_UNQUALIFIED)
		{
			// binocular device model
			printf("Right eye image is not qualified. Quality of the current captured image:\n\t");
			printf("1. Total score of left eye: %d\n\t2. Usable area of left eye: %d\n\t",
				pQualities[LEFT_EYE_IDX].totalScore, pQualities[LEFT_EYE_IDX].usableArea);
		}
		*/
		else
		{
			bRun = true;
			continue;
			//handle_error(iRet);
			//goto RETSEC;
		}
		//reset_error_level(iRet);
		if (bProcessResult && captureStatus == IDDK_COMPLETE)
		{
			/* Get the result image */
			get_result_image(times);

			/* Get result ISO image */
			//get_result_ISO_image(times);

			/* Get the result template */
			get_result_template(times);
			return;
		}

		/* iris_recognition calls, we break the loop */
		if (!bMultiple)
		{
			goto RETSEC;
		}

		/* Let's try another capturing? */
		/**
		printf("\nTry another capture?\n\t1. Yes (default)\n\t2. No");
		printf("\nEnter your choice: ");
		option = choose_option(2);

		if (option == -1) printf("1\n");
		if (option == 2)
		{
			goto RETSEC;
		}
		*/

		bRun = true;

		/* Number of capturing */
		times++;
	}

RETSEC:
	/* Finally, we deinit the camera */
	iRet = Iddk_DeinitCamera(g_hDevice);
	if (iRet != IDDK_OK)
	{
		handle_error(iRet);
	}

	reset_error_level(iRet);
}

void recovery()
{
	IddkResult iRet = IDDK_OK;
	IddkRecoveryCode recoveryCode = IDDK_USB_CANCEL_IO;
	bool isReenumerateDevice = false;

	/* Check as if the device is opened */
	if (g_hDevice == NULL) return;

	printf("\nSelect your action? \n\t1. Cancel IO (default)\n\t2. Reset pipes\n\t3. Reset port\n\t4. Cycle port\n\t5. Soft reset\n\t6. Reopen device\n\t7. Main menu\n");
	printf("Enter your choice: ");
	switch (choose_option(7))
	{
	case -1:
		printf("1\n");
	case 1:
		recoveryCode = IDDK_USB_CANCEL_IO;
		break;
	case 2:
		recoveryCode = IDDK_USB_RESET_PIPES;
		break;
	case 3:
		recoveryCode = IDDK_USB_RESET_PORT;
		g_deviceSleep = false;
		break;
	case 4:
		recoveryCode = IDDK_USB_CYCLE_PORT;
		g_deviceSleep = false;
		break;
	case 5:
		recoveryCode = IDDK_SOFT_RESET;
		break;
	case 6:
		get_device_handle();
		g_deviceDeepSleep = false;
		return;
	case 7:
		return;
	}

	iRet = Iddk_Recovery(g_hDevice, recoveryCode);
	if (iRet == IDDK_OK)
	{
		printf("\nRecovery OK !\n");

		if (recoveryCode == IDDK_USB_CYCLE_PORT || recoveryCode == IDDK_SOFT_RESET)
		{
			/* Wait for sure ... */
			printf("Please wait for a while ...\n");
			wait(10000);
			get_device_handle();
		}
	}
	else
	{
		handle_error(iRet);

		printf("\nRecovery failed !\n");
	}
}
