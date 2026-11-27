#include "iot_common.h"
#include "iot_altivar.h"


void IOT_INIT()
    {
    }

void IOT_EVALUATE()
    {
#ifdef WIN_OS
    G_ALTIVAR_MANAGER()->evaluate();
#endif // WIN_OS

#ifdef LINUX_OS
    G_ALTIVAR_MANAGER()->evaluate();
#endif
    }

void IOT_FINAL()
    {
    }
