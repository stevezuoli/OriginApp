#include <gtest/gtest.h>
#include "Framework/INativeMessageQueue.h"

using namespace DkFormat;
TEST(Message, MakeEndCachePaintMessage)
{
    SNativeMessage msg;
    MakeEndCachePaintMessage(&msg);
    ASSERT_EQ(KMessageEndCachePaint, msg.iType);
}

TEST(Message, MakePaintMessage)
{
    SNativeMessage msg;
    bool result;

    {
        result = MakePaintMessage(2, 3, 4, 5, PAINT_FLAG_REPAINT | PAINT_FLAG_CACHABLE, &msg);
        ASSERT_TRUE(result);
        ASSERT_EQ(DkRect::FromLeftTopWidthHeight(2, 3, 4, 5), DkRectFromPaintMessage(msg));
        ASSERT_EQ(PAINT_FLAG_REPAINT | PAINT_FLAG_CACHABLE, PaintFlagFromPaintMessage(msg));
    }

    {
        result = MakePaintMessage(-2, 3, 4, 5, PAINT_FLAG_REPAINT | PAINT_FLAG_CACHABLE, &msg);
        ASSERT_FALSE(result);
        ASSERT_EQ(KMessageNothing, msg.iType);
    }
    {
        result = MakePaintMessage(2, 65536, 4, 5, PAINT_FLAG_REPAINT | PAINT_FLAG_CACHABLE, &msg);
        ASSERT_FALSE(result);
        ASSERT_EQ(KMessageNothing, msg.iType);
    }
    {
        result = MakePaintMessage(1, 2, 3, 4, PAINT_FLAG_NONE, NULL);
        ASSERT_FALSE(result);
    }
    {
        result = MakePaintMessage(10, 0, 0, 400, PAINT_FLAG_NONE, &msg);
        ASSERT_TRUE(result);
        ASSERT_EQ(DkRect::FromLeftTopWidthHeight(10, 0, 0, 400), DkRectFromPaintMessage(msg));
    }
}

