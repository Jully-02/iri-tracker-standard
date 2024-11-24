#include "IriTracker.h"
#include <QString>
#include <QIODevice>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <utility>
#include "Iddk2000_features.h"
#include "IriLivenessBase.h"

IriTracker::IriTracker()
{
}

void IriTracker::get_divice() {
	get_device_handle_custom();
}

void IriTracker::run()
{
	bool bDefaultParams = false;
	bool bMultiple = true;
	bool bProcessResult = true;
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

	qDebug() << "hello CC nè";
	/* We have to init camera first */
	printf("\nInit Camera: \n");
	iRet = Iddk_InitCamera(g_hDevice, &imageWidth, &imageHeight);
	qDebug() << "IRet: " << iRet;
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
					// 230523
					//////////////////////////////////////////////////////////////////////
					emit imageProcessed(pImages->imageData, pImages->imageDataLen, pImages->imageWidth, pImages->imageHeight);
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
			qint64 timestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
			/* Get the result image */
			IddkImage* result = get_result_image_custom(timestamp);
			emit imageResult(result->imageData, result->imageDataLen, result->imageWidth, result->imageHeight);
			//get_result_ISO_image(times);

			/* Get the result template */
			QString pathTemplate = get_result_template_custom(timestamp);
			emit resultTemplate(pathTemplate);
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