#include <stdlib.h>
#include <math.h>
#include "colorblending.h"

//  +-----------------------------------------------------------------------+
//  |                     Fun with Channel Blending                         |
//  |                                  c++                                  |
//  |                              Version 1.0                              |
//  +-----------------------------------------------------------------------+
//  |                     Released to the Public Domain                     |
//  |                           January 29th 2016                           |
//  +-----------------------------------------------------------------------+
//  |                         fyngyrz / AA7AS / Ben                         |
//  +-----------------------------------------------------------------------+
//  | Website: http://fyngyrz.com                                           |
//  | Contact: fyngyrz,64,0x67,0x6D,0x61,0x69,0x6C,056,0x63,0x6F,0x6D       |
//  +-----------------------------------------------------------------------+
//  | AboutMe: Cats (and flies; summer only. Flies die in winter here)      |
//  | QuoteMe: I was born under the sign "Obstetrics."                      |
//  | Because: Pizza. NY style. Otherwise it's just a pizza-like substance. |
//  +-----------------------------------------------------------------------+
//
// Whys and What-fors:
// -------------------
// Channel intensity from a camera is output as the square root of the actual
// sensor input. This has been true since video cameras have been widely in use.
//
// So cameras basically take the square root of what they see, then send that on.
// This represention has wormed its way into the usual color handing done in
// software. When that is so, it must be taken into consideration when blending.
//
// To blend such values, you square them back into their original domain, do the
// blend, then take the square root of the result so you're dealing again with the
// compressed channel range. Formally, with all values in domain of 0 ... 1:
//
//                    _______________________________________________
//                   /         2                           2
//   mixedResult =  / (channelA^ x (1 - blend)) + (channelB^ x blend)
//                \/
//
// Alpha values, on the other hand, are purely linear, so no rooting around is required:
//
//   mixedResult = (alphaA x (1 - blend)) + (alphaB x blend)
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// I've provided both inline macros and callable functions:
// --------------------------------------------------------

// The Macros:
// ===========
// You can use the class macros to inline blending functionality for a little extra
// speed (recommended.) There are macros for reals (slow, but good to any precision),
// and for 8-bit data (very fast) optimized two different ways. If you use the latter,
// then you can benchmark the two methods in the target machine (do a large number of blends
// one way, the do a large number the other way, time each of them, and then in your actual
// code, use the one that is faster.) Note that depending on which macro you use, they
// are used slightly differently.
//
// If you have real data (slow, no significant benefit from macros, but here they are anyway):
// -------------------------------------------------------------------------------------------
// #include "colorblending.h"
// #include <math.h>
//
// blended_channel = CHANNEL_BLENDR(CHANNELA,CHANNELB,BLEND);	// BLEND is 0...1
//   blended_alpha = ALPHA_BLENDR(ALPHAA,ALPHAB,BLEND);		// BLEND is 0...1
//
// The following macros require the class object be passed to them. For instance,
// my Qt MainWindow class has this in its class definition (.h) file:
//
//     colorblending blender;
//
// I have a global pointer to my MainWindow class available to all...
//
// #include "colorblending.h"
//
//     MainWindow *tmw;
//
// ...that I assign first thing in my MainWindow::MainWindow() function:
//
// MainWindow::MainWindow(QWidget *parent) :
//     QMainWindow(parent),
//     ui(new Ui::MainWindow)
// {
//     tmw = self;
//     if (blender.m_bAvailable == false) 	qApp->exit(0); // exit if blender failed to init
//     ...
// }
//
// Then in a subordinate class in which I use the multiply version of the channel
// blend macro, I do an RGB blend on my unsigned character (8-bit) data this way:
//
// #include "mainwindow.h"
// #include "colorblending.h"
//
// extern MainWindow *tmw;
//
//    ...
//    blended_r = CHANNEL_BLENDM(tmw->blender,rsourceA,rsourceB,blend);
//    blended_g = CHANNEL_BLENDM(tmw->blender,gsourceA,gsourceB,blend);
//    blended_b = CHANNEL_BLENDM(tmw->blender,bsourceA,bsourceB,blend);
//    ...
//
// If you have 8-bit data, and the target machine has a very fast integer multiply:
// --------------------------------------------------------------------------------
// #include "colorblending.h"
//
// blended_channel = CHANNEL_BLENDM(OBJECT,CHANNELA,CHANNELB,BLEND);	// A, B, BLEND 0...255
//   blended_alpha = ALPHA_BLENDM(OBJECT,ALPHAA,ALPHAB,BLEND);			// A, B, BLEND 0...255
//
// If you have 8-bit data, and the target machine has a slow integer multiply:
// note that you also need to provide a variable X to which the output will be
// assigned in this version:
// ---------------------------------------------------------------------------
// #include "colorblending.h"
//
// CHANNEL_BLENDX(OBJECT,CHANNELA,CHANNELB,BLEND,X);	// A, B, BLEND 0...255
// ALPHA_BLENDX(OBJECT,ALPHAA,ALPHAB,BLEND,X);			// A, B, BLEND 0...255
//
// Example:
//
// #include "mainwindow.h"
// #include "colorblending.h"
//
// extern MainWindow *tmw;
//
//    ...
//    CHANNEL_BLENDX(tmw->blender,rsourceA,rsourceB,blend,blended_r);
//    CHANNEL_BLENDX(tmw->blender,gsourceA,gsourceB,blend,blended_g);
//    CHANNEL_BLENDX(tmw->blender,bsourceA,bsourceB,blend,blended_b);
//    ...


// The Functions:
// ==============

// If you have all reals then:
// ---------------------------
// #include "colorblending.h"
//
// blended_channel = dBlendChannel(channelA,channelB,blend)
//   blended_alpha = dBlendAlpha(alphaA,alphaB,blend)
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// If you have 8-bit greyscale and/or 24-bit color, and 8-bit alpha, and 8-bit blend,
// you can use whichever of these is faster on the target machine:
//
// hardcoded for speed using CPU multiply:
// ---------------------------------------
// #include "colorblending.h"
//
// blended_channel = ucBlendChannel8m(channelA,channelB,blend)
//   blended_alpha = ucBlendAlpha8m(alphaA,alphaB,blend)
//
// hardcoded for speed using bytewise index construction:
// ------------------------------------------------------
// #include "colorblending.h"
//
// blended_channel = ucBlendChannel8x(channelA,channelB,blend)
//   blended_alpha = ucBlendAlpha8x(alphaA,alphaB,blend)
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// If you have 8-bit greyscale and/or 24-bit color, and 8-bit alpha, and real (0...1) blend,
// you can use whichever of these is faster on the target machine:
//
// hardcoded for speed using CPU multiply:
// ---------------------------------------
// #include "colorblending.h"
//
//		ucBlendChannel8mr(channelA,channelB,blend)
//		ucBlendAlpha8mr(alphaA,alphaB,blend)
//
// hardcoded for speed using bytewise index construction:
// ------------------------------------------------------
// #include "colorblending.h"
//
//		ucBlendChannel8xr(channelA,channelB,blend)
//		ucBlendAlpha8xr(alphaA,alphaB,blend)
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// The Code (tab spacing=4 for sanity)
// ===================================

double colorblending::dBlendChannel(	double channelA,	// 0 ... n, where n is max channel intensity
										double channelB,	// 0 ... n, where n is max channel intensity
										double blend)		// 0 ... 1
{
	return(sqrt(((1.0 - blend) * (channelA * channelA)) + (blend * (channelB * channelB))));
}

double colorblending::dBlendAlpha(	double alphaA,	// 0 ... n, where n is max alpha
									double alphaB,	// 0 ... n, where n is max alpha
									double blend)	// 0 ... 1
{
	return(((1.0-blend)*alphaA)+(blend*alphaB));
}

unsigned char colorblending::ucBlendChannel8m(	unsigned char channelA,	// 0 ... 255
												unsigned char channelB,	// 0 ... 255
												unsigned char blend)	// 0 ... 255
{
	return(iRoots8[ iBlendsPrime8[(int)blend * 256 + (int)channelA] + iBlends8[(int)blend * 256 + (int)channelB] ]);
}

unsigned char colorblending::ucBlendChannel8mr(	unsigned char channelA,	// 0 ... 255
												unsigned char channelB,	// 0 ... 255
												double blend)			// 0 ... 1
{
	blend *= 255.0;
	return(iRoots8[ iBlendsPrime8[(int)blend * 256 + (int)channelA] + iBlends8[(int)blend * 256 + (int)channelB] ]);
}

unsigned char colorblending::ucBlendChannel8x(	unsigned char channelA,	// 0 ... 255
												unsigned char channelB,	// 0 ... 255
												unsigned char blend)	// 0 ... 255
{
	cha.c[0] = channelA;
	chb.c[0] = channelB;
	cha.c[1] = chb.c[1] = blend;
	return( iRoots8[ iBlendsPrime8[cha.s] + iBlends8[chb.s] ] );
}

unsigned char colorblending::ucBlendChannel8xr(	unsigned char channelA,	// 0 ... 255
												unsigned char channelB,	// 0 ... 255
												double blend)			// 0 ... 1
{
	blend *= 255.0;
	cha.c[0] = channelA;
	chb.c[0] = channelB;
	cha.c[1] = chb.c[1] = blend;
	return( iRoots8[ iBlendsPrime8[cha.s] + iBlends8[chb.s] ] );
}


unsigned char colorblending::ucBlendAlpha8m(unsigned char alphaA,
											unsigned char alphaB,
											unsigned char blend)
{
	return( iFactorsPrime8[(int)blend * 256 + (int)alphaA] + iFactors8[(int)blend * 256 + (int)alphaB] );
}

unsigned char colorblending::ucBlendAlpha8x(unsigned char alphaA,
											unsigned char alphaB,
											unsigned char blend)
{
	cha.c[0] = alphaA;
	chb.c[0] = alphaB;
	cha.c[1] = chb.c[1] = blend;
	return( iFactorsPrime8[cha.s] + iFactors8[chb.s] );
}

unsigned char colorblending::ucBlendAlpha8mr(	unsigned char alphaA,	// 0...255
												unsigned char alphaB,	// 0...255
												double blend)			// 0...1
{
	blend *= 255.0;
	return( iFactorsPrime8[(int)blend * 256 + (int)alphaA] + iFactors8[(int)blend * 256 + (int)alphaB] );
}

unsigned char colorblending::ucBlendAlpha8xr(	unsigned char alphaA,	// 0...255
												unsigned char alphaB,	// 0...255
												double blend)			// 0...1
{
	blend *= 255;
	cha.c[0] = alphaA;
	chb.c[0] = alphaB;
	cha.c[1] = chb.c[1] = blend;
	return( iFactorsPrime8[cha.s] + iFactors8[chb.s] );
}


// The key to speed is to precalculate everything you possibly
// can, and then arrange that you can get at those precalculated
// values as fast as possible. The ONLY circumstance under which
// you shouldn't do this is when the calculation itself will be
// faster than going after pre-calculated data. This can be the
// case with a multiply, for instance; some CPUs have very slow
// multiply instructions (or none at all) but others can do a
// multiply in one instruction cycle. So you have to consider
// your target to maximize your execution speed.
//
// Why am I so hot on speed? Here's how I see it: The CPU cycles
// available in someone's machine are a hard-limited resource.
// When one program uses them, another can be starved for them.
// Unlike memory, you can't "just add more."
//
// Therefore, the respectful thing to do is to minimize the
// the demand for CPU cycles as much as possible.
//
// When working with 8-bit channel and alpha values, we can
// precalculate the most costly operations and make the 8-bit
// version much, much faster than the version that deals with
// real numbers. The cost is a little memory, which is cheap,
// and the gain is in CPU cycles, which are expensive, consume
// more energy, and considerably more hard-limited by the technology
// ceiling than is memory.
//
// You should only ever have one instance of this class in
// your topmost class, therefore, it runs once on startup,
// and that's it. Also, if USING_8BIT is defined (not commented out)
// in colorblending.h, then after the containing class initializes,
// check that...
//
//     m_bAvailable == true
//
// ...if it is false, then the colorblender class failed to obtain the
// memory it requires for 8-bit operations, and you can only use the
// macros and functions that operate upon real data. Although if you're
// so tight on memory you can't get what this class needs, you're
// probably better off just exiting your application entirely.
//
// If USING_8BIT is *not* defined, then you cannot use the 8-bit versions
// of color and alpha blending, only the real versions. But it'll save you
// some memory:
//
// Working memory load on a machine with 4-byte unsigned ints is 1,572,864 bytes
//
// One last thing: Why pointers instead of just declaring the
// the arrays required in the class? Because for programs
// that use a lot of memory, there can be limits to how much
// memory you get to just gobble up as part of the program.
// Whereas if you allocate it this way, there generally isn't
// any problematic limit, other than how much memory you
// actually have. Which you can generally improve upon if you
// so choose. :)
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define IMSIZE (sizeof(unsigned int) * ((256 * 256)))			// squared, factored numbers
#define IRSIZE (256 * 256 * 2)									// square root of 0 ... 130050
#define IRMSIZE (sizeof(unsigned int) * IRSIZE)					// square root of 0 ... 130050

bool colorblending::initColors() // returns false on FAILURE
{
unsigned int channel,blend,theSum;
double fchannel,fblend;

	m_uiMemLoad = 0;
	if ((iBlendsPrime8 = (unsigned int*)malloc(IMSIZE)))
	{
		m_uiMemLoad += IMSIZE;
		if ((iBlends8 = (unsigned int*)malloc(IMSIZE)))
		{
			m_uiMemLoad += IMSIZE;
			if ((iFactors8 = (unsigned int*)malloc(IMSIZE)))
			{
				m_uiMemLoad += IMSIZE;
				if ((iFactorsPrime8 = (unsigned int*)malloc(IMSIZE)))
				{
					m_uiMemLoad += IMSIZE;
					if ((iRoots8 = (unsigned int*)malloc(IRMSIZE)))
					{
						m_uiMemLoad += IRMSIZE;
						for (channel=0; channel<256; channel++)
						{
							for (blend=0; blend<256; blend++)
							{
								if (m_bLittleEndian)
								{
									cha.c[0] = (unsigned char)channel;
									cha.c[1] = (unsigned char)blend;
								}
								else
								{
									cha.c[1] = (unsigned char)channel;
									cha.c[0] = (unsigned char)blend;
								}

								fchannel = (double)channel / 255.0;
								fblend   = (double)blend   / 255.0;

								iBlendsPrime8[cha.s]  = (unsigned int)(fchannel * fchannel * (1.0 - fblend) * 65535.0);
								iBlends8[cha.s]       = (unsigned int)(fchannel * fchannel * fblend * 65535.0);

								iFactorsPrime8[cha.s] = (unsigned int)((1.0 - fblend) * fchannel * 255.0);
								iFactors8[cha.s]      = (unsigned int)(fblend * fchannel * 255.0);
							}
						}
						for (theSum=0; theSum < IRSIZE; theSum++) // IRSIZE
						{
							iRoots8[theSum] = (int)sqrt((double)theSum);
						}
						return(true);
					}
				}
			}
		}
	}
	closeColors(); // clean up anything that did get allocated
	return(false);
}

void colorblending::stomp()
{
	iFactorsPrime8	= NULL;
	iBlendsPrime8	= NULL;
	iFactors8		= NULL;
	iBlends8		= NULL;
	iRoots8			= NULL;
}

void colorblending::closeColors()
{
	if (iFactorsPrime8)		free(iFactorsPrime8);
	if (iBlendsPrime8)		free(iBlendsPrime8);
	if (iFactors8)			free(iFactors8);
	if (iBlends8)			free(iBlends8);
	if (iRoots8)			free(iRoots8);
	stomp();
}

colorblending::colorblending()
{
	stomp();
#ifdef USING_8BIT
	cha.c[0] = 0;	// test CPU memory organization
	cha.c[1] = 1;
	if (cha.s == 1)		m_bLittleEndian = false;	// high byte first
	else				m_bLittleEndian = true;		// low byte first
	m_bAvailable = initColors();
#else
	m_bAvailable = false;
	m_uiMemLoad = 0;
#endif
}

colorblending::~colorblending()
{
	closeColors();
}
