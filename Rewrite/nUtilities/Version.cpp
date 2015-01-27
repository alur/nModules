//-------------------------------------------------------------------------------------------------
// /nUtilities/Version.cpp
// The nModules Project
//
// Functions for dealing with versioning.
//-------------------------------------------------------------------------------------------------
#include "Version.h"

/// <summary>
/// Gets a particular part of the version number.
/// </summary>
#define GET_VERSION_PART(version, part) \
  VERSION_PART(((version) & (VERSION(VERSION_PART(-1)) << 8*sizeof(VERSION_PART)*(part))) >> 8*sizeof(VERSION_PART)*(part))


VERSION MakeVersion(VERSION_PART major, VERSION_PART minor, VERSION_PART patch, VERSION_PART build) {
  return VERSION(major) << 48 | VERSION(minor) << 32 | VERSION(patch) << 16 | VERSION(build);
}


VERSION_PART GetMajorVersion(VERSION version) {
  return GET_VERSION_PART(version, 3);
}


VERSION_PART GetMinorVersion(VERSION version) {
  return GET_VERSION_PART(version, 2);
}


VERSION_PART GetPatchVersion(VERSION version) {
  return GET_VERSION_PART(version, 1);
}


VERSION_PART GetBuildVersion(VERSION version) {
  return GET_VERSION_PART(version, 0);
}
