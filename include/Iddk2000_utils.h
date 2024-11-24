#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ctime>

#if _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#define ENTER 10 /* User presses enter */
#endif

#include "Iddk2000Apis.h"

#ifdef WIN32
#pragma warning( disable : 4996 )
#endif

#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif

#ifdef SUCCESS
#undef SUCCESS
#endif

#ifdef ERROR
#undef ERROR
#endif

#ifdef NULL
#undef NULL
#endif

#define SUCCESS				0
#define ERROR				-1

#define NULL				0x00

typedef char _menu_list[][64];
extern IddkEyeSubtype g_selectEyeMode;
extern bool g_isBinocular;

static float read_float(const char* message, float limitMin, float limitMax, float defaultValue)
{
	char buffer[256];
	float result = 1.0f;
	bool rightChoice = false;
	int i = 0;
	unsigned int dotCount = 0;

	printf("%s", message);
	while(!rightChoice)
	{
		dotCount = 0;
		rightChoice = true;
		sprintf(buffer, "%s", "");
		gets_s(buffer);
#ifdef _MSC_VER
		if(strlen(buffer) == 0)
#else
		if(strlen(buffer) == 0 || buffer[0] == ENTER)
#endif
		{
			if(defaultValue != 0.0f)
			{
				printf("%f\n", defaultValue);
				return defaultValue;
			}
			else 
			{
				rightChoice = false;
				continue;
			}
		}
		for(i = 0;i < (int)strlen(buffer); i++)
		{
			if(!isdigit(buffer[i])) 
			{
				if(buffer[i] != '.')
				{
					rightChoice = false;
					break;
				}
				else dotCount++;
			}
			if(dotCount > 1)
			{
				rightChoice = false;
				break;
			}
		}
		if(dotCount == 1 && (strlen(buffer) == 1))
		{
			rightChoice = false;
		}
		if(rightChoice)
		{
			sscanf(buffer, "%f", &result);
			if(result < limitMin || result > limitMax) rightChoice = false;
		}
		if(!rightChoice)
		{
			printf("Invalid value. It should be in the range [%.1f, %.1f]\n", limitMin, limitMax);
			printf("%s", message);
		}
	}
	return result;
}

static unsigned int read_uint(const char* message, unsigned int limitMin, unsigned int limit, unsigned int defaultValue)
{
	char buffer[256];
	unsigned int result = 0;
	bool rightChoice = false;
	int i = 0;

	printf("%s", message);
	while(!rightChoice)
	{
		rightChoice = true;
		sprintf(buffer, "%s", "");
		gets_s(buffer);
#ifdef _MSC_VER
		if(strlen(buffer) == 0)
#else
		if(strlen(buffer) == 0 || buffer[0] == ENTER)
#endif
		{
			if(defaultValue != 0)
			{
				printf("%u\n", defaultValue);
				return defaultValue;
			}
			else 
			{
				rightChoice = false;
				continue;
			}
		}
		for(i = 0;i < (int)strlen(buffer); i++)
		{
			if(!isdigit(buffer[i])) 
			{
				rightChoice = false;
				break;
			}
		}
		if(rightChoice)
		{
			sscanf(buffer, "%u", &result);
			if(result < limitMin || result > limit) rightChoice = false;
		}
		if(!rightChoice)
		{
			printf("Invalid value. It should be in [%u; %u]\n", limitMin, limit);
			printf("%s", message);
		}
	}
	return result;
}

static unsigned int read_uint(const char* message, unsigned int limitMin, unsigned int limit, unsigned int except, unsigned int defaultValue)
{
	char buffer[256];
	unsigned int result = 0;
	bool rightChoice = false;
	int i = 0;

	printf("%s", message);
	while(!rightChoice)
	{
		rightChoice = true;
		sprintf(buffer, "%s", "");
		gets_s(buffer);
#ifdef _MSC_VER
		if(strlen(buffer) == 0)
#else
		if(strlen(buffer) == 0 || buffer[0] == ENTER)
#endif
		{
			printf("%u\n", defaultValue);
			return defaultValue;
		}
		for(i = 0;i < (int)strlen(buffer); i++)
		{
			if(!isdigit(buffer[i])) 
			{
				rightChoice = false;
				break;
			}
		}
		if(rightChoice)
		{
			sscanf(buffer, "%u", &result);
			if((result < limitMin || result > limit) && result != except) rightChoice = false;
		}
		if(!rightChoice)
		{
			printf("Invalid value. It should be in [%u; %u] or %u\n", limitMin, limit, except);
			printf("%s", message);
		}
	}
	return result;
}

static int choose_option(int numOfOpts)
{
	char buffer[256];

	int option = -1;
	bool rightChoice = false;
	while(!rightChoice)
	{
		sprintf(buffer, "%s", "");
		gets_s(buffer);
		if(strlen(buffer) == 1 && isdigit(buffer[0]))
		{
			buffer[1] = '\0';
			sscanf(buffer, "%d", &option);
			if(option > 0 && option <= numOfOpts) rightChoice = true;
		}
		else 
#ifdef _MSC_VER
		if(strlen(buffer) == 0)
#else
		if(strlen(buffer) == 0 || buffer[0] == ENTER)
#endif
		{
			option = -1;
			return option;
		}
		if(!rightChoice)
		{
			printf("Invalid number. Please try again !\n");
			printf("Select one option: ");
		}
	}
	return option;
}

static int display_menu(_menu_list menus, int menu_size, int default_idx, const char* special_comment = NULL)
{
	for(int i = 0; i < menu_size; i++)
	{
		if(i == default_idx)
			printf("\t%d. %s (default)\n", i + 1, (menus)[i]);
		else
			printf("\t%d. %s\n", i + 1, (menus)[i]);
	}
	if(special_comment)
		printf("Enter your choice (%s): ", special_comment);
	else
		printf("Enter your choice: ");
	return choose_option(menu_size);
}

static bool check_supported_baudrate(unsigned int baudrate)
{
	bool supported = false;
#ifndef _MSC_VER
	switch(baudrate)
	{
	case 57600:
	case 115200:
	case 230400:
	case 460800:
	case 500000:
	case 576000:
		supported = true;
	}
#else
	if(baudrate >= 56000) supported = true;
#endif
	return supported;
}

//Define it for portability. Otherwise, Sleep() can be used.
static void wait(int milliSeconds)
{
#ifdef _MSC_VER
	Sleep(milliSeconds);
#else
	usleep(milliSeconds*1000);
#endif
}

#ifdef _MSC_VER
static bool save_file(char* filename, PBYTE pData, DWORD nData)
{

	if(filename == NULL || pData == NULL || nData <= 0)
	{
		return false;
	}
	FILE *fp = NULL;
	bool iRet = true;
	__try
	{
		fp = fopen(filename, "wb");
		if(!fp)
		{
			//bRet = FALSE;
			iRet = false;
			__leave;
		}
		if(fwrite(pData, sizeof(BYTE), nData, fp) < nData)
		{			
			//bRet = FALSE;
			iRet = false;
			__leave;
		}
	}
	__finally
	{
		if(fp)
		{
			fclose(fp);
		}
	}
	return iRet;
}
#else
static bool save_file(const char* filename, unsigned char* pData, unsigned long nData)
{
	FILE *fp = NULL;
	bool iRet = true;
	if(filename == NULL || pData == NULL || nData <= 0)
	{
		return false;
	}
	try
	{
		fp = fopen(filename, "wb");
		if(!fp)
		{
			iRet = false;
			printf("\nCannot open file ...\n");
			throw;
		}
		if(fwrite(pData, sizeof(unsigned char), nData, fp) < nData)
		{			
			iRet = false;
			printf("\nCannot write to file ...\n");
			throw;
		}
		if(fp)
		{
			fclose(fp);
		}
	}
	catch(...)
	{
		if(fp)
		{
			fclose(fp);
		}
	}
	return iRet;
}
#endif

static bool read_file(const char* filename, unsigned char* &pData, unsigned long &nData)
{
	unsigned long nKey;
	FILE *fp = fopen(filename, "rb");
	if(!fp)
	{
		return false;
	}
	fseek(fp, 0, SEEK_END);
	nKey = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if(pData && (nKey > nData)) 
	{
		free(pData);
		pData = NULL;
	}
	if(!pData)
	{
		pData = (unsigned char *) malloc(nKey);
		if(!pData)
		{
			return false;
		}
		nData = nKey;
	}
	if(fread(pData, sizeof(unsigned char), nKey, fp) < nKey)
	{
		nData = 0;
		fclose(fp);
		return false;
	}
	nData = nKey;
	fclose(fp);
	return true;
}

static void prepare_param_for_capturing(IddkCaptureMode& captureMode, IddkQualityMode& qualityMode,
										IddkEyeSubtype& eyeSubtype, bool& bAutoLeds, int& iCount)
{
	IddkConfig config;
	char buffer[256];
	buffer[0] = '\0';
	Iddk_GetSdkConfig(&config);

	printf("\nParameters for capturing process\n");
	printf("Capture mode: \n\t1. IDDK_TIMEBASED (default) \n\t2. IDDK_FRAMEBASED\n");
	printf("Enter your choice: ");
	switch(choose_option(2))
	{
	case -1:
		printf("1\n");
	case 1:
		captureMode = IDDK_TIMEBASED;
		iCount = read_uint("\nEnter the duration since iris detected (from 1 to 600 seconds, enter for 3): ", 1, 600, 3);
		break;
	case 2:
		captureMode = IDDK_FRAMEBASED;
		iCount = read_uint("\nEnter the number of frames since iris detected(from 1 to 600 frames, enter for 3): ", 1, 600, 3);
	}

	printf("\nQuality mode: \n\t1. Normal (default)\n\t2. High \n\t3. Very High\n");
	printf("Enter your choice: ");
	switch(choose_option(3))
	{
	case -1:
		printf("1\n");
	case 1:
		qualityMode = IDDK_QUALITY_NORMAL;
		break;
	case 2:
		qualityMode = IDDK_QUALITY_HIGH;
		break;
	case 3:
		qualityMode = IDDK_QUALITY_VERY_HIGH;
	}

	printf("\nEnable auto led? \n\t1. Yes (default)\n\t2. No\n");
	printf("Enter your choice: ");
	switch(choose_option(2))
	{
	case -1:
		printf("1\n");
	case 1:
		bAutoLeds = true;
		break;
	case 2:
		bAutoLeds = false;
	}

	eyeSubtype = IDDK_UNKNOWN_EYE;
	if(g_isBinocular)
	{
		printf("\nSpecify eye subtype to capture: \n\t1. Left Eye \n\t2. Right Eye \n\t3. Both eye(default)\n");
		printf("Enter your choice: ");
		switch(choose_option(3))
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
	}

	g_selectEyeMode = eyeSubtype;
}
