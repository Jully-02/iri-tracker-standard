#pragma once
#ifndef _IRI_LIVENESS_BASE_H
#define _IRI_LIVENESS_BASE_H

namespace IriLivenessDetector
{
    /* common error codes which returns from function checkLiveness -------------------------------------------------*/
    /* everything OK */
    #define IRI_LN_OK 0
    /* Invalid parameter */
    #define IRI_LN_INVALID_PARAM 0x7001
    /* The function initialize already was called (Resource already initialized) */
    #define IRI_LN_ALREADY_INITIALIZED 0x7002
    /* Data cannot fit in buffer */
    #define IRI_LN_NOT_INITIALIZE 0x7003
    /* Internal error */
    #define IRI_LN_INTERNAL_ERR 0x7004

    /* The status return from function checkLiveness, which check liveness status of an input */
    enum LIVENESS_STATUS { EYE_FAKE = 0x0, EYE_REAL, EYE_CONTACT_LENS };

    typedef enum _LN_EyeSide_
    {
        UNKNOWN_EYE = 0x0,
        RIGHT_EYE,
        LEFT_EYE
    } LN_EyeSide;

    typedef struct _LivenessResult_
    {
        LIVENESS_STATUS livenessStatus;
        LN_EyeSide eyeSide;
    } LivenessResult;
}

#endif 
