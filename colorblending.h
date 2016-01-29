#ifndef COLORBLENDING_H
#define COLORBLENDING_H

// Comment this #define out if you will only be using the REAL versions of
// the macros and/or functions, it'll save you about a megabyte and a half
// of memory:
// --------------------------------------------------------------------
#define USING_8BIT 1




#define CHANNEL_BLENDR(CHANNELA,CHANNELB,BLEND) ( \
	sqrt(((1.0 - BLEND) * (CHANNELA * CHANNELA)) + (BLEND * (CHANNELB * CHANNELB))) \
)

#define ALPHA_BLENDR(ALPHAA,ALPHAB,BLEND) ( \
	((1.0-BLEND) * ALPHAA) + (BLEND * ALPHAB) \
)

#define CHANNEL_BLENDM(OBJECT,CHANNELA,CHANNELB,BLEND) ( \
	OBJECT.iRoots8[ OBJECT.iBlendsPrime8[(int)BLEND * 256 + (int)CHANNELA] + OBJECT.iBlends8[(int)BLEND * 256 + (int)CHANNELB] ] \
)

#define ALPHA_BLENDM(OBJECT,ALPHAA,ALPHAB,BLEND) ( \
	OBJECT.iFactorsPrime8[(int)BLEND * 256 + (int)ALPHAA] + OBJECT.iFactors8[(int)BLEND * 256 + (int)ALPHAB] \
)

#define CHANNEL_BLENDX(OBJECT,CHANNELA,CHANNELB,BLEND,X) ( \
	OBJECT.cha.c[0] = CHANNELA, \
	OBJECT.chb.c[0] = CHANNELB, \
	OBJECT.cha.c[1] = OBJECT.chb.c[1] = BLEND, \
	X = OBJECT.iRoots8[ OBJECT.iBlendsPrime8[OBJECT.cha.s] + OBJECT.iBlends8[OBJECT.chb.s] ] \
)

#define ALPHA_BLENDX(OBJECT,X,ALPHAA,ALPHAB,BLEND) ( \
	OBJECT.cha.c[0] = ALPHAA, \
	OBJECT.chb.c[0] = ALPHAB, \
	OBJECT.cha.c[1] = OBJECT.chb.c[1] = BLEND, \
	X = OBJECT.iFactorsPrime8[OBJECT.cha.s] + CLASS.iFactors8[OBJECT.chb.s] \
)

class colorblending
{

public:
    colorblending();
	~colorblending();

	unsigned char	ucBlendChannel8m(	unsigned char channelA,
										unsigned char channelB,
										unsigned char blend);

	unsigned char	ucBlendChannel8x(	unsigned char channelA,
										unsigned char channelB,
										unsigned char blend);

	unsigned char	ucBlendChannel8mr(	unsigned char channelA,
										unsigned char channelB,
										double blend);

	unsigned char	ucBlendChannel8xr(	unsigned char channelA,
										unsigned char channelB,
										double blend);

	unsigned char	ucBlendAlpha8m(	unsigned char alphaA,
									unsigned char alphaB,
									unsigned char blend);

	unsigned char	ucBlendAlpha8x(	unsigned char alphaA,
									unsigned char alphaB,
									unsigned char blend);

	unsigned char	ucBlendAlpha8mr(	unsigned char alphaA,
									unsigned char alphaB,
									double blend);

	unsigned char	ucBlendAlpha8xr(unsigned char alphaA,
									unsigned char alphaB,
									double blend);

	double			dBlendChannel(	double channelA,
									double channelB,
									double blend);

	double			dBlendAlpha(	double alphaA,
									double alphaB,
									double blend);

	void stomp();

	bool m_bAvailable;				// TRUE if memory allocations succeeded
	unsigned int m_uiMemLoad;		// class working memory load in bytes

	unsigned int *iFactorsPrime8;	// These are public so code can be inlined with the macros
	unsigned int *iBlendsPrime8;
	unsigned int *iFactors8;
	unsigned int *iBlends8;
	unsigned int *iRoots8;

	union {	unsigned short s; unsigned char c[2]; } cha,chb;

private:
	bool m_bLittleEndian;			// memory organization for union use
	bool initColors();
	void closeColors();

};

#endif // COLORBLENDING_H
