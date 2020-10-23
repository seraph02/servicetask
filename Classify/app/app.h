#ifndef APP_H
#define APP_H
#include "fbm/fbm.h"
#include "voxer/voxer.h"
#include "imo/imo.h"
#include "telegram/telegram.h"
#include "skype/skype.h"

REGISTER(FBM);
REGISTER(IMO);
REGISTER(VOXER);
REGISTER(TELEGRAM);
REGISTER(SKYPE);
#endif // APP_H
