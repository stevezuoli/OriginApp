#include "Common/WindowsMetrics.h"

namespace WindowsMetrics
{
static int metrics[][DEVICE_NUMBER] =
{
#define MAKE_METRICS(x, y, z) {y, z},
    METRICS_LIST(MAKE_METRICS)
#undef MAKE_METRICS
};

static int fontSizes[][DEVICE_NUMBER] =
{
#define MAKE_FONTSIZES(x, y ,z) {y, z},
    FONTSIZE_LIST(MAKE_FONTSIZES)
#undef MAKE_FONTSIZES
};

static DeviceIndex s_deviceIndex = DI_DEFAULT;

void SetDeviceIndex(DeviceIndex deviceIndex)
{
    s_deviceIndex = deviceIndex;
}

int GetWindowMetrics(int index)
{
    if (index >= 0 && index < UIMetricsCounterIndex)
    {
        return metrics[index][s_deviceIndex];
    }
    else
    {
        return -1;
    }
}

int GetWindowFontSize(int index)
{
    if (index >= 0 && index < UIFontSizeCounterIndex)
    {
        return fontSizes[index][s_deviceIndex];
    }
    else
    {
        return -1;
    }
}
};
