#ifndef MAIN_H
#define MAIN_H

#define CRASH_SYSTEM_INTEGRATE

void crash_init();
void crash_main();

#ifdef CRASH_SYSTEM_INTEGRATE

#include "crash/entry.h"
#include "goolengine.h"
#include "objectengine.h"
#include "cameraengine.h"
#include "integ.h"

#endif

#endif
