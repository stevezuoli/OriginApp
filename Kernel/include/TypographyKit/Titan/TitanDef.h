//===========================================================================
// Summary:
//		公共结构定义
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-08-18
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef	__TITANDEF_HEADERFILE__
#define __TITANDEF_HEADERFILE__

//===========================================================================

#include <math.h>
#include <string>
#include <vector>
using namespace std;

//===========================================================================

#define TITANEXPORT
#define TITANSTDMETHODCALLTYPE		__stdcall
#define TITANAPI					extern "C" TITANEXPORT bool
#define TITANAPI_(type)				extern "C" TITANEXPORT type

//===========================================================================

#undef TITAN_NATIVE_WIDE_CHAR_SUPPORT
#ifdef _ANDROID_LINUX
#define TITAN_NATIVE_WIDE_CHAR_SUPPORT
#endif

//===========================================================================

#ifdef TITAN_NATIVE_WIDE_CHAR_SUPPORT
typedef int TP_WCHAR;
typedef std::basic_string<TP_WCHAR, std::char_traits<TP_WCHAR>, std::allocator<TP_WCHAR> > TP_WSTRING;
#else
typedef wchar_t TP_WCHAR;
typedef std::wstring TP_WSTRING;
#endif

//===========================================================================

typedef void*	TP_HANDLE;

//===========================================================================

// 排版后状态
enum TP_LAYOUT_STATE
{
	TP_LAYOUT_SUCCEED		= 0x01,			// 所有对象排版成功
	TP_LAYOUT_FOLLOWAFTER	= 0x02,			// 排版部分成功，需要后续继续排版
	TP_LAYOUT_FAIL			= 0X03,			// 排版失败
	TP_LAYOUT_INIT_FAIL		= 0x04,			// 初始化失败
	TP_LAYOUT_OUTOFROOM		= 0x05,			// 空间不够
	TP_LAYOUT_INPUT_NO_TEXT = 0x06			// 没有传入文字（待排串长度为0）
};

// 字体编码集
enum TP_CHARSET_TYPE
{
	TP_CHARSET_ANSI,
	TP_CHARSET_PRC,
	TP_CHARSET_BIG5	
};

// 对齐方式
enum TP_ALIGN_TYPE
{
	TP_ALIGN_LEFT,
	TP_ALIGN_RIGHT,
	TP_ALIGN_CENTER,
	TP_ALIGN_JUSTIFY
};

// 度量单位
enum TP_MEASURE_TYPE
{
	TP_MEASURE_PIXEL,						// 像素点
	TP_MEASURE_POINT,						// 磅，定义为1/72英寸
	TP_MEASURE_CM,							// 厘米
	TP_MEASURE_MM,							// 毫米
	TP_MEASURE_INCH,						// 英寸
	TP_MEASURE_CUSTOM						// 用户自定义
};

// Piece类型
enum TP_PIECE_TYPE
{
	TP_PIECE_TEXT,							// 文字块
	TP_PIECE_NEWLINE,						// 强制换行块
	TP_PIECE_NEWPARA,						// 换段块，目前只用于HTML处理器
    TP_PIECE_PARAEND,                       // 段结束
	TP_PIECE_GRAPH,							// 图形块
	TP_PIECE_IMAGE,							// 图像块
	TP_PIECE_FIGURENOTE,					// 图注
	TP_PIECE_RUBY,							// 拼注音
	TP_PIECE_HORINVER,						// 纵中横
	TP_PIECE_COMBINELINES,					// 分行缩排
};

// 图文关系
enum TP_WRAPPING_TYPE
{
	TP_WRAPPING_AROUND,						// 文字环绕
	TP_WRAPPING_ALONE,						// 独占
	TP_WRAPPING_FOLLOW						// 随文
};

enum TP_FOLLOWIMAGE_TYPE
{
    TP_FOLLOWIMAGE_IMAGE,                   // 随文图
    TP_FOLLOWIMAGE_NOTE,                    // 文内注
    TP_FOLLOWIMAGE_CHARACTER                // 造字
};

// 空行处理策略
enum TP_BLANKLINE_STRATEGY
{
	TP_BLANKLINE_COPY,						// 和数据源保持一致
	TP_BLANKLINE_SINGLE,					// 多个连续空行压缩为一个空行
	TP_BLANKLINE_IGNOREALL					// 忽略所有空行
};

// 字体定义方式
enum TP_FONTDEFINE_TYPE
{
	TP_FONTDEFINE_FACENAME,					// 字体名
	TP_FONTDEFINE_FILENAME,					// 字体文件路径
	TP_FONTDEFINE_HANDLE					// 自定义句柄
};

// 行间距定义方式
enum TP_LINEGAP_TYPE
{
	TP_LINEGAP_MULTIPLE,					// 多倍行距
	TP_LINEGAP_FIXEDVALUE					// 采用固定值
};

// 字体支持的字符集（可以为组合值）
enum TP_FONTCHARSET_TYPE
{
	TP_FONTCHARSET_LATIN	= 0x1,			// 西文字符集
	TP_FONTCHARSET_GB		= 0x2,			// 中文字符集
};

// 字体风格
enum TP_FONTSTYLE_TYPE
{
	TP_FONTSTYLE_USERDEFINED,				// 用户自定义风格，当使用MP_FONTDEFINE_HANDLE时可用
	TP_FONTSTYLE_NORMAL,					// 普通
	TP_FONTSTYLE_ITALIC						// 倾斜
};

// 字体浓淡
enum TP_FONTWEIGHT_TYPE
{
	TP_FONTWEIGHT_USERDEFINED,				// 用户自定义浓淡，当使用MP_FONTDEFINE_HANDLE时可用
	TP_FONTWEIGHT_NORMAL,					// 普通
	TP_FONTWEIGHT_BOLD						// 加粗
};

// 文本片段的类型，目前只适用于HTML处理器
enum TP_TEXTPIECE_TYPE
{
    TP_TEXTPIECE_NORMAL,                    // 正常的文本，即采用标准HTML空白字符处理策略后的文本
    TP_TEXTPIECE_PRE                        // pre标签内的文本，即保留HTML文本原格式的文本
};

// 标点符号竖排时候的样式
enum TP_PUNCTUATION_VERTICAL_STYLE
{
	TP_PUNCTUATION_VERTICAL_STYLE_CHS = 0,	// 简体中文样式
	TP_PUNCTUATION_VERTICAL_STYLE_CHT,		// 繁体中文样式
	TP_PUNCTUATION_VERTICAL_STYLE_JP		// 日文样式
};

// Hyphenation使用的语言
enum TP_HYPHENATION_LANGUAGE
{
	TP_HYPHENATION_LANGUAGE_UNDEFINED = 0,	// 未定义，不做Hyphenation操作
	TP_HYPHENATION_LANGUAGE_AUTOMATION,		// 排版引擎根据字符串所属的语言自动设置Hyphen语言选项。
	TP_HYPHENATION_LANGUAGE_EN,				// 英文
	TP_HYPHENATION_LANGUAGE_DE,				// 德文
	TP_HYPHENATION_LANGUAGE_DE_TRADITIONAL	// 传统德文	
};

// 拼注音对齐方式
enum TP_RUBY_ALIGN_TYPE
{
	TP_RUBY_ALIGN_CENTER = 0,				// 音标居中显示
	TP_RUBY_ALIGN_LEFT,						// 音标居左显示
	TP_RUBY_ALIGN_RIGHT,					// 音标居右显示
	TP_RUBY_ALIGN_DISTRIBUTE_LETTER,		// 音标两端对齐
	TP_RUBY_ALIGN_DISTRIBUTE_SPACE			// 音标首尾字符外侧有空位，空位宽度为音标文字字符间距的一半
};

// 拼注音位置
enum TP_RUBY_POSITION
{
	TP_RUBY_POSITION_BEFORE = 0,			// 音标文字位于正文文字之前
	TP_RUBY_POSITION_AFTER					// 音标文字位于正文文字之后
};

// 排版方向类型
enum TP_DIRECTION_TYPE
{
	TP_DIRECTION_LTR_TTB,					// 从左往右，从上往下横排。
	TP_DIRECTION_TTB_RTL					// 从上往下，从右往左竖排。
};

// 排版字符重力方向
enum TP_GRAVITY_DIRECTION
{
	TP_GRAVITY_DOWN = 0,        			// 字符重力方向向下，此为默认状态
	TP_GRAVITY_LEFT,						// 字符重力方向向左
	TP_GRAVITY_UP,							// 字符重力方向向上
	TP_GRAVITY_RIGHT,						// 字符重力方向向右
	TP_GRAVITY_AUTO
};

// 盒子垂直方向的对齐方式
enum TP_BOX_VALIGN_TYPE
{
    TP_BOX_VALIGN_BASELINE = 0, // 与文字基线
    TP_BOX_VALIGN_TOPLINE,      // 与文字矩形上边界
    TP_BOX_VALIGN_MIDDLELINE,   // 与文字矩形中线
    TP_BOX_VALIGN_BOTTOMLINE    // 与文字矩形下边界
};

// 盒子大小的单位
enum TP_LENGTH_UNIT
{
    TP_LENGTH_UNKNOWN,
    TP_LENGTH_PERCENT,
    TP_LENGTH_PX
};

// CJK排版环境设置（可以为组合值）
enum TP_CJK_LAYOUT_ENV
{
    ENV_AUTO_ADJUST_CJK_LATIN_NUM_DISTANCE  = 0x01, // 自动调整中文和西文、数字的间距
    ENV_ALLOW_PUNCTUATION_OVERFLOW          = 0x02, // 允许CJK标点溢出边界
    ENV_SHRINK_CJK_PUNCTUATION              = 0x04, // 压缩CJK标点
    ENV_SHRINK_LINE_BEGIN_PUNCTUATION       = 0x08, // 允许行首中文标点压缩，只在设置了SHRINK_CJK_PUNCTUATION的情况下才能设置这个值
    ENV_ALLOW_USE_KERNING                   = 0x10, // 允许处理Kerning
    ENV_ALLOW_USE_HYPHEN                    = 0x20, // 允许处理Hyphen
    ENV_SHRINK_LINE_END_PUNCTUATION         = 0x40, // 强制行尾中文标点压缩，只在设置了SHRINK_CJK_PUNCTUATION的情况下才能设置这个值

    ENV_NONE                                = 0x00, 
    ENV_DEFAULT                             = ENV_AUTO_ADJUST_CJK_LATIN_NUM_DISTANCE 
                                            //| ENV_ALLOW_PUNCTUATION_OVERFLOW 
                                            | ENV_SHRINK_CJK_PUNCTUATION 
                                            | ENV_SHRINK_LINE_BEGIN_PUNCTUATION 
                                            | ENV_ALLOW_USE_KERNING 
                                            | ENV_ALLOW_USE_HYPHEN 
                                            //| ENV_SHRINK_LINE_END_PUNCTUATION
};

#define TP_AUTO_ADJUST_CJK_LATIN_NUM_DISTANCE(env)  (0 != ((env) & ENV_AUTO_ADJUST_CJK_LATIN_NUM_DISTANCE))
#define TP_ALLOW_PUNCTUATION_OVERFLOW(env)          (0 != ((env) & ENV_ALLOW_PUNCTUATION_OVERFLOW))
#define TP_SHRINK_CJK_PUNCTUATION(env)              (0 != ((env) & ENV_SHRINK_CJK_PUNCTUATION))
#define TP_SHRINK_LINE_BEGIN_PUNCTUATION(env)       (0 != ((env) & ENV_SHRINK_LINE_BEGIN_PUNCTUATION))
#define TP_ALLOW_USE_KERNING(env)                   (0 != ((env) & ENV_ALLOW_USE_KERNING))
#define TP_ALLOW_USE_HYPHEN(env)                    (0 != ((env) & ENV_ALLOW_USE_HYPHEN))
#define TP_SHRINK_LINE_END_PUNCTUATION(env)         (0 != ((env) & ENV_SHRINK_LINE_END_PUNCTUATION))

//===========================================================================

// 判断两个浮点数是否相等
#define TP_DELTA		0.0001
#define D_EQUAL(x, y)	(fabs((x) - (y)) <= TP_DELTA)

// 文件路径长度最大值
#ifdef MAX_PATH
#define TP_MAX_PATH	MAX_PATH
#else
#define TP_MAX_PATH	260
#endif

// 位置坐标数据类型
struct TP_POS 
{
	double X;
	double Y;

	TP_POS()
		: X(0.0), Y(0.0)
	{}

	TP_POS(double x, double y)
		: X(x), Y(y)
	{}

	bool operator==(const TP_POS& pos) const
	{
		return D_EQUAL(X, pos.X) && D_EQUAL(Y, pos.Y);
	}

	bool operator!=(const TP_POS& pos) const
	{
		return !(*this == pos);
	}
};

// 区域坐标数据类型
struct TP_BOX 
{
	double X0;
	double Y0;	// 左上角坐标
	double X1;
	double Y1;	// 右下角坐标

	TP_BOX()
		:X0(0.0), Y0(0.0), X1(0.0), Y1(0.0)
	{}

	TP_BOX(double x0, double y0, double x1, double y1)
		: X0(x0), Y0(y0), X1(x1), Y1(y1)
	{}

	TP_BOX& operator=(const TP_BOX& box)
	{
		X0 = box.X0;
		Y0 = box.Y0;
		X1 = box.X1;
		Y1 = box.Y1;

		return *this;
	}

	bool operator==(const TP_BOX& box) const
	{
		return D_EQUAL(X0, box.X0) && D_EQUAL(Y0, box.Y0) && D_EQUAL(X1, box.X1) && D_EQUAL(Y1, box.Y1);
	}

	bool operator!=(const TP_BOX& box) const
	{
		return !(*this == box);
	}

	double Width() const
	{
		return fabs(X1 - X0);
	}

	double Height() const
	{
		return fabs(Y1 - Y0);
	}

	// 判断点是否位于BOX内
	bool PosInBox(TP_POS pos) const
	{
		return (pos.X >= X0 && pos.X <= X1 && pos.Y >= Y0 && pos.Y <= Y1);
	}

	// 判断两个BOX是否有交集
	bool IsIntersect(const TP_BOX& box) const
	{
		return (X0 <= box.X1 && X1 >= box.X0 && Y0 <= box.Y1 && Y1 >= box.Y0);
	}
};

// 字形度量
struct  TP_GLYPHMETRICS
{
	double  dWidth;
	double  dHeight;

	double  dHoriBearingX;
	double  dHoriBearingY;
	double  dHoriAdvance;

	double  dVertBearingX;
	double  dVertBearingY;
	double  dVertAdvance;

	TP_GLYPHMETRICS()
		:dWidth(0.0), dHeight(0.0), dHoriBearingX(0.0), dHoriBearingY(1.0), dHoriAdvance(0.0), dVertBearingX(0.0), dVertBearingY(0.0), dVertAdvance(0.0)
	{}

};

// 坐标变换矩阵
// 假设变换前的坐标(x_user, y_user)，变换后的坐标(x_device, y_device)，则有如下公式：
// x_device = x_user * matrix->M11 + y_user * matrix->M12 + matrix->M13;
// y_device = x_user * matrix->M21 + y_user * matrix->M22 + matrix->M23;
struct  TP_MATRIX
{
	double M11;
	double M21;
	double M12;
	double M22;
	double M13;
	double M23;

	TP_MATRIX()
		:M11(1.0), M21(0.0), M12(0.0), M22(1.0), M13(0.0), M23(0.0)
	{}

	TP_MATRIX(double a, double b, double c, double d, double e, double f)
		:M11(a), M21(b), M12(c), M22(d), M13(e), M23(f)
	{}

	TP_MATRIX& operator=(const TP_MATRIX& matrix)
	{
		M11 = matrix.M11;
		M21 = matrix.M21;
		M12 = matrix.M12;
		M22 = matrix.M22;
		M13 = matrix.M13;
		M23 = matrix.M23;

		return *this;
	}

	void TransformPos(TP_POS& pos)
	{
		double x = pos.X;
		double y = pos.Y;
		pos.X = x * M11 + y * M12 + M13;
		pos.Y = x * M21 + y * M22 + M23;
	}

	void TransformBox(TP_BOX& box)
	{
		double x = box.X0;
		double y = box.Y0;
		box.X0 = x * M11 + y * M12 + M13;
		box.Y0 = x * M21 + y * M22 + M23;
		x = box.X1;
		y = box.Y1;
		box.X1 = x * M11 + y * M12 + M13;
		box.Y1 = x * M21 + y * M22 + M23;

		if (box.X0 > box.X1)
		{
			x = box.X1;
			box.X1 = box.X0;
			box.X0 = x;
		}
		if (box.Y0 > box.Y1)
		{
			y = box.Y1;
			box.Y1 = box.Y0;
			box.Y0 = y;
		}
	}
};

//===========================================================================

#endif	//#ifndef __TITANDEF_HEADERFILE__
