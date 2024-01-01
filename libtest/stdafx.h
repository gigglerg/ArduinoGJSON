// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "targetver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <new>          // std::nothrow


#define snprintf(d_,sd_,f_,...)	_snprintf(d_,sd_,(f_), __VA_ARGS__)
