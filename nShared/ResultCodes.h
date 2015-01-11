/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ResultCodes.h
 *  The nModules Project
 *
 *  Custom HRESULT codes used by the nModules.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"

// Creates a customer-defined HRESULT
#define MAKE_CHRESULT(severity, facility, code) \
    ((HRESULT) (((unsigned long)(severity)<<31) | 0x20000000 | ((unsigned long)(facility)<<16) | ((unsigned long)(code))) )

// Facility codes
#define FACILITY_NCORECOM 0
#define FACILITY_NCORE 1

// nCoreCom
#define E_NCORECOM_CORE_NOT_FOUND   MAKE_CHRESULT(SEVERITY_ERROR, FACILITY_NCORECOM, 0)
#define E_NCORECOM_MAJOR_VERSION    MAKE_CHRESULT(SEVERITY_ERROR, FACILITY_NCORECOM, 1)
#define E_NCORECOM_MINOR_VERSION    MAKE_CHRESULT(SEVERITY_ERROR, FACILITY_NCORECOM, 2)
#define E_NCORECOM_PATCH_VERSION    MAKE_CHRESULT(SEVERITY_ERROR, FACILITY_NCORECOM, 3)
#define E_NCORECOM_FUNC_INIT        MAKE_CHRESULT(SEVERITY_ERROR, FACILITY_NCORECOM, 4)
