/********************						***********************/
//
//	Player PRO 5.9 - DRIVER SOURCE CODE -
//
//	Library Version 5.9
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	20 Micheli-Du-Crest
//	1205 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998, 1999, 2000, 2001, 2002
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#include "RDriver.h"
#include "RDriverInt.h"
#include "MADPrivate.h"
#include "VSTFunctions.h"

static inline void PrepareInline(int *VolInter, int* rVolInter, double p2, double v1, double v2)
{
	double temp = (v2 - v1) / p2;
	
	*VolInter = temp * (1 << BYTEDIV);
	
	*rVolInter = ((int)v1 << BYTEDIV);
}

void MADCreateOverShoot(MADDriverRec *intDriver)
{
	long i, x;
	
	switch(intDriver->DriverSettings.outPutBits) {
		case 16:
			intDriver->DASCBuffer = (int*)calloc((intDriver->ASCBUFFER * 8) + intDriver->MDelay * 2 * 8, 1);
			
			for (i = 0; i < MAXCHANEFFECT; i++)
				intDriver->DASCEffectBuffer[i]	= (int*)calloc((intDriver->ASCBUFFER * 8) + intDriver->MDelay * 2 * 8, 1);
			
			break;
			
		case 8:
			intDriver->DASCBuffer8 = (short*)calloc((intDriver->ASCBUFFER * 4) + intDriver->MDelay * 2 * 4, 1);
			intDriver->OverShoot = (char*)calloc(256L * 32L, 1);
			
			//for (i = 0; i < 256L * 16L; i++) intDriver->OverShoot[i] = 0;
			memset(intDriver->OverShoot, 0, 256 * 16);
			for (i = 256L * 16L, x = 0; i < 256L * 17L; i++, x++) intDriver->OverShoot[i] = x;
			//memset(&intDriver->OverShoot[256 * 17], 0xff, 256 * 32 - 256 * 17);
			for (i = 256L * 17L; i < 256L * 32L; i++) intDriver->OverShoot[i] = 0xFF;
			
			intDriver->OverShoot += 256L*16L;
			intDriver->OverShoot += 128L;
			break;
	}
}

void MADKillOverShoot(MADDriverRec *intDriver)
{
	int i;
	
	switch(intDriver->DriverSettings.outPutBits) {
		case 16:
			if (intDriver->DASCBuffer != NULL) {
				free(intDriver->DASCBuffer);
				intDriver->DASCBuffer = NULL;
			}
			
			for (i = 0; i < MAXCHANEFFECT; i++) {
				if (intDriver->DASCEffectBuffer[i] != NULL) {
					free(intDriver->DASCEffectBuffer[i]);
					intDriver->DASCEffectBuffer[i] = NULL;
				}
			}
			break;
			
		case 8:
			if (intDriver->OverShoot != NULL) {
				intDriver->OverShoot -= 128L;
				intDriver->OverShoot -= 256L*16L;
				free(intDriver->OverShoot);
				intDriver->OverShoot = NULL;
			}
			
			if (intDriver->DASCBuffer8 != NULL) {
				free(intDriver->DASCBuffer8);
				intDriver->DASCBuffer8 = NULL;
			}
			break;
	}
}

void Sampler16AddDelay(MADChannel *curVoice, int *ASCBuffer, MADDriverRec *intDriver)
{
	int		chnVol = 0, chnVol2 = 0;
	int		chnVolS = 0, chnVol2S = 0;
	int		chnVolD = 0, chnVol2D = 0;
	int		i = intDriver->ASCBUFFER;
	char	tByte = 0;
	int		*ASCBuffer1, *ASCBuffer2;
	bool	killSample = false;
	bool	aa = true;
	int		VolInter1, VolInter2, rVolInter1, rVolInter2;
	int		aDD, aCC = curVoice->lAC, off = 0;
	
#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
	{
		long long finalperiod;
		long double temp;
		
		if (intDriver->XMLinear) {
			finalperiod = getfrequency(curVoice->period);
			finalperiod = (8363L*1712L)/(finalperiod?finalperiod:1);
			
			finalperiod = (finalperiod * 8363L) / (long) curVoice->fineTune;
		} else
			finalperiod = curVoice->period;
		
		finalperiod = ((long long) finalperiod * (long long) intDriver->base.FreqExt) / (long long) 8000;
		
		temp = ((long double)AMIGA_CLOCKFREQ2) / ((long double)finalperiod * (long double)(intDriver->DriverSettings.outPutRate) * (long double)intDriver->DriverSettings.oversampling);
		
		if (temp >= 1)
			aa = false;
		
		aDD = temp * (1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate) * intDriver->DriverSettings.oversampling);
#endif
	
	if (curVoice->pingpong == true && curVoice->loopType == MADLoopTypePingPong)
		aDD = -aDD;	// PINGPONG
	
	chnVol2S	= DoVolPanning256(0, curVoice, intDriver, true);
	chnVolS		= DoVolPanning256(1, curVoice, intDriver, true);
	
	curVoice->volEnvInter += i;
	
	chnVol2D	= DoVolPanning256(0, curVoice, intDriver, true);
	chnVolD		= DoVolPanning256(1, curVoice, intDriver, true);
	
	if ((curVoice->TrackID % 2) == 0) {
		ASCBuffer1 = ASCBuffer;
		ASCBuffer2 = ASCBuffer +1L + intDriver->MDelay*2L;
	} else {
		ASCBuffer2 = ASCBuffer +1L;
		ASCBuffer1 = ASCBuffer + intDriver->MDelay*2L;
	}
	
	//*****************************************
	if (intDriver->DriverSettings.TickRemover)
		MADTickRemoverStart8(curVoice, ASCBuffer1, ASCBuffer2, intDriver);
	//*****************************************
	
	if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
		return;
	
	{
		char	*SndBuffer = curVoice->curPtr;
		int		RightWeight, LeftWeight, preOff = curVoice->preOff;
  		char	preVal = curVoice->preVal, preVal2 = curVoice->preVal2;
  		
  		PrepareInline(&VolInter1, &rVolInter1, intDriver->ASCBUFFER, chnVolS, chnVolD);
  		PrepareInline(&VolInter2, &rVolInter2, intDriver->ASCBUFFER, chnVol2S, chnVol2D);
  		
		while (i-- > 0) {
			rVolInter2 += VolInter2;
			chnVol2 = rVolInter2 >> BYTEDIV;
			
			rVolInter1 += VolInter1;
			chnVol = rVolInter1 >> BYTEDIV;
			
			RightWeight = aCC & ((1 << BYTEDIV) - 1);
			LeftWeight = (1 << BYTEDIV) - RightWeight;
			off = (aCC>>BYTEDIV);
			
			if (preOff != off) {
				if (curVoice->loopType == MADLoopTypePingPong && curVoice->loopSize > 0) {		// PINGPONG
					preOff = off;
					if ((SndBuffer + off +1 >= curVoice->maxPtr && !curVoice->pingpong) ||
					   (SndBuffer + off +1 <= curVoice->begPtr + curVoice->loopBeg && curVoice->pingpong)) {
						curVoice->pingpong = !curVoice->pingpong;
						aDD = -aDD; aCC += aDD;
						RightWeight = aCC & ((1 << BYTEDIV) - 1);
						LeftWeight = (1 << BYTEDIV) - RightWeight; off = (aCC>>BYTEDIV);
					}
					preVal = *(SndBuffer + off);
				} else {
					preVal = preVal2;
					preOff = off;
					
					if (SndBuffer + off + 1 >= curVoice->maxPtr) {
						if (curVoice->loopSize > 0) {
							aCC = aCC & ((1 << BYTEDIV) - 1);
							RightWeight = aCC & ((1 << BYTEDIV) - 1);
							LeftWeight = (1 << BYTEDIV) - RightWeight;
							off = (aCC>>BYTEDIV);
							preOff = off;
							
							SndBuffer = curVoice->begPtr + curVoice->loopBeg;
							SndBuffer--;
						} else {	// If TICK remove
							MADTickLoopFill8(curVoice, ASCBuffer1, ASCBuffer2, i, (tByte * chnVol) >> 8, (tByte * chnVol2) >> 8);
							killSample = true;
							break;
						}
					}
					preVal2 = *(SndBuffer + off + 1);
				}
			}
			
			if (aa == false) {
				LeftWeight = (1 << BYTEDIV);
				RightWeight = 0;
			}
			
			tByte = (LeftWeight * preVal + RightWeight * *(SndBuffer + off + 1)) >> BYTEDIV;
			aCC += aDD;
			
			*ASCBuffer1   += (tByte * chnVol) >> 8;
			ASCBuffer1 += 2;
			*ASCBuffer2   += (tByte * chnVol2) >> 8;
			ASCBuffer2 += 2;
		}
		
		if (killSample) {
			curVoice->samplePtr	= NULL;
			curVoice->curPtr	= curVoice->maxPtr;
		} else {
			if ((aCC>>BYTEDIV) == preOff) curVoice->preOff = 0;
			else curVoice->preOff = 0xFFFFFFFF;	//(long) (aCC>>BYTEDIV);
			curVoice->preVal = preVal;
			curVoice->preVal2 = *(SndBuffer + off + 1);
			curVoice->curPtr = SndBuffer + (aCC>>BYTEDIV);
		}
		
		curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
		
		curVoice->lastWordL = (tByte * chnVol) >> 8;
		curVoice->lastWordR = (tByte * chnVol2) >> 8;
	}
}

void Sampler16AddDelayStereo(MADChannel *curVoice, int *ASCBuffer, MADDriverRec *intDriver)
{
	int		chnVol = 0, chnVol2 = 0;
	int		i = intDriver->ASCBUFFER;
	int		chnVolS, chnVol2S;
	int		chnVolD, chnVol2D;
	char	tByteL = 0, tByteR = 0;
	int		*ASCBuffer1, *ASCBuffer2;
	bool	killSample = false;
	bool	aa = true;
	int		VolInter1, VolInter2, rVolInter1, rVolInter2;
	int		aDD, aCC = curVoice->lAC, off = 0;
	
#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
	{
		long long finalperiod;
		long double	temp;
		
		if (intDriver->XMLinear) {
			finalperiod = getfrequency(curVoice->period);
			finalperiod = (8363L*1712L)/(finalperiod?finalperiod:1);
			
			finalperiod = (finalperiod * 8363L) / (long) curVoice->fineTune;
		} else
			finalperiod = curVoice->period;
		
		finalperiod = ((long long) finalperiod * (long long) intDriver->base.FreqExt) / (long long) 8000;
		
		temp = ((long double)AMIGA_CLOCKFREQ2) / ((long double)finalperiod * (long double)(intDriver->DriverSettings.outPutRate) * (long double)intDriver->DriverSettings.oversampling);
				
		if (temp >= 1)
			aa = false;
		
		aDD = temp * (1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate) * intDriver->DriverSettings.oversampling);
#endif
	
	
	if (curVoice->pingpong == true && curVoice->loopType == MADLoopTypePingPong)
		aDD = -aDD;	// PINGPONG
	
	chnVol2S	= DoVolPanning256(0, curVoice, intDriver, true);
	chnVolS		= DoVolPanning256(1, curVoice, intDriver, true);
	
	curVoice->volEnvInter += i;
	
	chnVol2D	= DoVolPanning256(0, curVoice, intDriver, true);
	chnVolD		= DoVolPanning256(1, curVoice, intDriver, true);
	
	if ((curVoice->TrackID % 2) == 0) {
		ASCBuffer1 = ASCBuffer;
		ASCBuffer2 = ASCBuffer +1L + intDriver->MDelay*2L;
	} else {
		ASCBuffer2 = ASCBuffer +1L;
		ASCBuffer1 = ASCBuffer + intDriver->MDelay*2L;
	}
	
	//*****************************************
	if (intDriver->DriverSettings.TickRemover)
		MADTickRemoverStart8(curVoice, ASCBuffer1, ASCBuffer2, intDriver);
	//*****************************************
	
	if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
		return;
	
	{
		char	*SndBuffer = curVoice->curPtr;
		int		RightWeight, LeftWeight, preOff = curVoice->preOff;
  		char	preVal = curVoice->preVal, preValR = curVoice->preValR, preVal2 = curVoice->preVal2, preVal2R = curVoice->preVal2R;
		
		PrepareInline(&VolInter1, &rVolInter1, intDriver->ASCBUFFER, chnVolS, chnVolD);
  		PrepareInline(&VolInter2, &rVolInter2, intDriver->ASCBUFFER, chnVol2S, chnVol2D);
  		
		while (i-- > 0) {
			rVolInter2 += VolInter2;
			chnVol2 = rVolInter2 >> BYTEDIV;
			
			rVolInter1 += VolInter1;
			chnVol = rVolInter1 >> BYTEDIV;
			
			RightWeight = aCC & ((1 << BYTEDIV) - 1);
			LeftWeight = (1 << BYTEDIV) - RightWeight;
			off = (int)(2 * (aCC >> BYTEDIV));
			
			if (preOff != off) {
				if (curVoice->loopType == MADLoopTypePingPong && curVoice->loopSize > 0) { // PINGPONG
					preOff = off;
					if ((SndBuffer + off +3 >= curVoice->maxPtr && !curVoice->pingpong) ||
					   (SndBuffer + off +2 <= curVoice->begPtr + curVoice->loopBeg && curVoice->pingpong)) {
						curVoice->pingpong = !curVoice->pingpong;
						aDD = -aDD; aCC += aDD; RightWeight = aCC & ((1 << BYTEDIV) - 1);
						LeftWeight = (1 << BYTEDIV) - RightWeight;
						off = (int) 2 * (aCC >> BYTEDIV);
					}
					preVal = *(SndBuffer + off);
					preValR = *(SndBuffer + off + 1);
				} else {
					preVal = preVal2;
					preValR = preVal2R;
					preOff = off;
					
					if (SndBuffer + off + 3 >= curVoice->maxPtr) {
						if (curVoice->loopSize > 0) {
							aCC = aCC & ((1 << BYTEDIV) - 1);
							RightWeight = aCC & ((1 << BYTEDIV) - 1);
							LeftWeight = (1 << BYTEDIV) - RightWeight;
							off = (int)2 * (aCC >> BYTEDIV);
							preOff = off;
							
							SndBuffer = curVoice->begPtr + curVoice->loopBeg;
							SndBuffer-=2;
						} else {	// If TICK remove
							MADTickLoopFill8(curVoice, ASCBuffer1, ASCBuffer2, i, (tByteL * chnVol) >> 8, (tByteR * chnVol2) >> 8);
							killSample = true;
							break;
						}
					}
					preVal2 = *(SndBuffer + off + 2);
					preVal2R = *(SndBuffer + off + 3);
				}
			}
			
			if (aa == false) {
				LeftWeight = (1 << BYTEDIV);
				RightWeight = 0;
			}
			
			tByteL = (LeftWeight * preVal + RightWeight * *(SndBuffer + off + 2)) >> BYTEDIV;
			*ASCBuffer1 += (tByteL * chnVol)>>8;
			ASCBuffer1 += 2;
			
			tByteR = (LeftWeight * preValR + RightWeight * *(SndBuffer + off + 3)) >> BYTEDIV;
			*ASCBuffer2 += (tByteR * chnVol2)>>8;
			ASCBuffer2 += 2;
			
			aCC += aDD;
		}
		
		if (killSample) {
			curVoice->samplePtr	= NULL;
			curVoice->curPtr	= curVoice->maxPtr;
		} else {
			if (2 * (aCC >> BYTEDIV) == preOff)
				curVoice->preOff = 0;
			else
				curVoice->preOff = 0xFFFFFFFF;
			curVoice->preVal = preVal;
			curVoice->preValR = preValR;
			curVoice->preVal2 = *(SndBuffer + off + 2);
			curVoice->preVal2R = *(SndBuffer + off + 3);
			curVoice->curPtr = SndBuffer + 2 * (aCC >> BYTEDIV);
		}
		
		curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
		
		curVoice->lastWordL = (tByteL * chnVol)>>8;
		curVoice->lastWordR = (tByteR * chnVol2)>>8;
	}
}


void Sampler16Addin16Delay(MADChannel *curVoice, int *ASCBuffer, MADDriverRec *intDriver)
{
	int		i = intDriver->ASCBUFFER;
	int		chnVol = 0, chnVol2 = 0;
	int		chnVolS = 0, chnVol2S = 0;
	int		chnVolD = 0, chnVol2D = 0;
	int		off = 0, tShort = 0;
	int		*ASCBuffer1, *ASCBuffer2;
	bool	killSample = false;
	bool	aa = true;
	int		aDD, aCC = curVoice->lAC;
	int		VolInter1, VolInter2, rVolInter1, rVolInter2;
	
#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
	{
		long long finalperiod;
		long double	temp;
		
		if (intDriver->XMLinear) {
			finalperiod = getfrequency(curVoice->period);
			finalperiod = (8363L*1712L)/(finalperiod?finalperiod:1);
			
			finalperiod = (finalperiod * 8363L) / (long)curVoice->fineTune;
		} else
			finalperiod = curVoice->period;
		
		finalperiod = ((long long) finalperiod * (long long) intDriver->base.FreqExt) / (long long)8000;
		
		temp = ((long double)AMIGA_CLOCKFREQ2) / (long double) ((long double)finalperiod * (long double)(intDriver->DriverSettings.outPutRate) * (long double) intDriver->DriverSettings.oversampling);
		
		if (temp >= 1) aa = false;
		
		aDD = temp * (1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate) * intDriver->DriverSettings.oversampling);
#endif
	
	if (curVoice->pingpong == true && curVoice->loopType == MADLoopTypePingPong)
		aDD = -aDD;	// PINGPONG
	
	chnVol2S	= DoVolPanning256(0, curVoice, intDriver, true);
	chnVolS		= DoVolPanning256(1, curVoice, intDriver, true);
	
	curVoice->volEnvInter += i;
	
	chnVol2D	= DoVolPanning256(0, curVoice, intDriver, true);
	chnVolD		= DoVolPanning256(1, curVoice, intDriver, true);
	
	if ((curVoice->TrackID % 2) == 0) {
		ASCBuffer1 = ASCBuffer;
		ASCBuffer2 = ASCBuffer + 1 + intDriver->MDelay * 2;
	} else {
		ASCBuffer2 = ASCBuffer + 1;
		ASCBuffer1 = ASCBuffer + intDriver->MDelay * 2;
	}
	
	//*****************************************
	if (intDriver->DriverSettings.TickRemover)
		MADTickRemoverStart8(curVoice, ASCBuffer1, ASCBuffer2, intDriver);
	//*****************************************
	
	if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
		return;
	
	{
		short	*SndBuffer = (short*)curVoice->curPtr;
		int		RightWeight, LeftWeight, preOff = curVoice->preOff;
		short	spreVal = curVoice->spreVal, spreVal2 = curVoice->spreVal2;
		
		PrepareInline(&VolInter1, &rVolInter1, intDriver->ASCBUFFER, chnVolS, chnVolD);
		PrepareInline(&VolInter2, &rVolInter2, intDriver->ASCBUFFER, chnVol2S, chnVol2D);
		
		while (i-- > 0) {
			rVolInter2 += VolInter2;
			chnVol2 = rVolInter2 >> BYTEDIV;
			
			rVolInter1 += VolInter1;
			chnVol = rVolInter1 >> BYTEDIV;
			
			RightWeight = aCC & ((1 << BYTEDIV) - 1);
			LeftWeight = (1 << BYTEDIV) - RightWeight;
			off = (aCC >> BYTEDIV);
			
			if (preOff != off) {
				if (curVoice->loopType == MADLoopTypePingPong && curVoice->loopSize > 0) {		// PINGPONG
					preOff = off;
					if (((SndBuffer + off +1 >= (short*) curVoice->maxPtr) && !curVoice->pingpong) ||
						(SndBuffer + off +1 <= (short*) (curVoice->begPtr + curVoice->loopBeg) && curVoice->pingpong)) {
						curVoice->pingpong = !curVoice->pingpong;
						aDD = -aDD;
						aCC += aDD;
						RightWeight = aCC & ((1 << BYTEDIV) - 1);
						LeftWeight = (1 << BYTEDIV) - RightWeight;
						off = (aCC >> BYTEDIV);
					}
					
					spreVal = *(SndBuffer + off);
				} else {
					spreVal = spreVal2;
					preOff = off;
					
					if (SndBuffer + off + 1 >= (short*) curVoice->maxPtr) {
						if (curVoice->loopSize > 0) {
							aCC = aCC & ((1 << BYTEDIV) - 1);
							RightWeight = aCC & ((1 << BYTEDIV) - 1);
							LeftWeight = (1 << BYTEDIV) - RightWeight;
							off = (aCC >> BYTEDIV);
							preOff = off;
							
							SndBuffer = (short*)(curVoice->begPtr + curVoice->loopBeg);
							SndBuffer--;
						} else {	// If TICK remove
							MADTickLoopFill8(curVoice, ASCBuffer1, ASCBuffer2, i, (tShort * chnVol) >> 16, (tShort * chnVol2) >> 16);
							killSample = true;
							break;
						}
					}
					spreVal2 = *(SndBuffer + off + 1);
				}
			}
			
			if (aa == false) {
				LeftWeight = (1 << BYTEDIV);
				RightWeight = 0;
			}
			
			tShort = (LeftWeight * spreVal + RightWeight * *(SndBuffer + off + 1)) >> BYTEDIV;
			aCC += aDD;
			
			*ASCBuffer1 += (tShort * chnVol) >> 16;
			ASCBuffer1 += 2;
			
			*ASCBuffer2 += (tShort * chnVol2) >> 16;
			ASCBuffer2 += 2;
		}
		
		if (killSample) {
			curVoice->samplePtr	= NULL;
			curVoice->curPtr	= curVoice->maxPtr;
		} else {
			if ((aCC>>BYTEDIV) == preOff)
				curVoice->preOff = 0;
			else
				curVoice->preOff = 0xFFFFFFFF;
			curVoice->spreVal = spreVal;
			curVoice->spreVal2 = *(SndBuffer + off + 1);
			curVoice->curPtr = (char*)(SndBuffer + (aCC>>BYTEDIV));
		}
		
		curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
		
		curVoice->lastWordL = (tShort * chnVol) >> 16;
		curVoice->lastWordR = (tShort * chnVol2) >> 16;
	}
}

void Sampler16Addin16DelayStereo(MADChannel *curVoice, int *ASCBuffer, MADDriverRec *intDriver)
{
	int		chnVolS = 0, chnVol2S = 0;
	int		chnVolD = 0, chnVol2D = 0;
	int		i = intDriver->ASCBUFFER;
	int		chnVol = 0, chnVol2 = 0;
	int		off = 0, tShortL = 0, tShortR = 0;
	int		*ASCBuffer1, *ASCBuffer2;
	bool	killSample = false;
	bool	aa = true;
	int		aDD, aCC = curVoice->lAC;
	int		VolInter1, VolInter2, rVolInter1, rVolInter2;
	
#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
	{
		long long finalperiod;
		long double	temp;
		
		if (intDriver->XMLinear) {
			finalperiod = getfrequency(curVoice->period);
			finalperiod = (8363LL * 1712LL) / (finalperiod ? finalperiod : 1);
			
			finalperiod = (finalperiod * 8363LL) / curVoice->fineTune;
		} else
			finalperiod = curVoice->period;
		
		finalperiod = ((long long)finalperiod * (long long)intDriver->base.FreqExt) / (long long)8000;
		
		temp = ((long double)AMIGA_CLOCKFREQ2) / ((long double)finalperiod * (long double)(intDriver->DriverSettings.outPutRate) * (long double)intDriver->DriverSettings.oversampling);
		
		if (temp >= 1)
			aa = false;
		
		aDD = temp * (1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate) * intDriver->DriverSettings.oversampling);
#endif
	
	if (curVoice->pingpong == true && curVoice->loopType == MADLoopTypePingPong)
		aDD = -aDD;	// PINGPONG
	
	chnVol2S	= DoVolPanning256(0, curVoice, intDriver, true);
	chnVolS		= DoVolPanning256(1, curVoice, intDriver, true);
	
	curVoice->volEnvInter += i;
	
	chnVol2D	= DoVolPanning256(0, curVoice, intDriver, true);
	chnVolD		= DoVolPanning256(1, curVoice, intDriver, true);
	
	if ((curVoice->TrackID % 2) == 0) {
		ASCBuffer1 = ASCBuffer;
		ASCBuffer2 = ASCBuffer + 1 + intDriver->MDelay * 2;
	} else {
		ASCBuffer2 = ASCBuffer + 1;
		ASCBuffer1 = ASCBuffer + intDriver->MDelay * 2;
	}
	
	//*****************************************
	if (intDriver->DriverSettings.TickRemover)
		MADTickRemoverStart8(curVoice, ASCBuffer1, ASCBuffer2, intDriver);
	//*****************************************
	
	if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
		return;
	
	{
		short	*SndBuffer = (short*)curVoice->curPtr;
		int		RightWeight, LeftWeight, preOff = curVoice->preOff;
		short	spreVal = curVoice->spreVal, spreValR = curVoice->spreValR, spreVal2 = curVoice->spreVal2, spreVal2R = curVoice->spreVal2R;
		
		PrepareInline(&VolInter1, &rVolInter1, intDriver->ASCBUFFER, chnVolS, chnVolD);
		PrepareInline(&VolInter2, &rVolInter2, intDriver->ASCBUFFER, chnVol2S, chnVol2D);
		
		while (i-- > 0) {
			rVolInter2 += VolInter2;
			chnVol2 = rVolInter2 >> BYTEDIV;
			
			rVolInter1 += VolInter1;
			chnVol = rVolInter1 >> BYTEDIV;
			
			RightWeight = aCC & ((1 << BYTEDIV) - 1);
			LeftWeight = (1 << BYTEDIV) - RightWeight;
			off = 2 * (aCC >> BYTEDIV);
			
			if (preOff != off) {
				if (curVoice->loopType == MADLoopTypePingPong && curVoice->loopSize > 0) {		// PINGPONG
					preOff = off;
					if ((SndBuffer + off +3 >= (short*) curVoice->maxPtr && !curVoice->pingpong) ||
						(SndBuffer + off +2 <= (short*) (curVoice->begPtr + curVoice->loopBeg) && curVoice->pingpong)) {
						curVoice->pingpong = !curVoice->pingpong;
						aDD = -aDD;
						aCC += aDD;
						RightWeight = aCC & ((1 << BYTEDIV) - 1);
						LeftWeight = (1 << BYTEDIV) - RightWeight;
						off = 2 * (aCC >> BYTEDIV);
					}
					spreVal = *(SndBuffer + off);
					spreValR = *(SndBuffer + off + 1);
				} else {
					spreVal = spreVal2;
					spreValR = spreVal2R;
					preOff = off;
					
					if (SndBuffer + off + 3 >= (short*) curVoice->maxPtr) {
						if (curVoice->loopSize > 0) {
							aCC = aCC & ((1 << BYTEDIV) - 1);
							RightWeight = aCC & ((1 << BYTEDIV) - 1);
							LeftWeight = (1 << BYTEDIV) - RightWeight;
							off = 2 * (aCC >> BYTEDIV);
							preOff = off;
							
							SndBuffer = (short*) (curVoice->begPtr + curVoice->loopBeg);
							SndBuffer-=2;
						} else {	// If TICK remove
							MADTickLoopFill8(curVoice, ASCBuffer1, ASCBuffer2, i, (tShortL * chnVol) >> 16, (tShortR * chnVol2) >> 16);
							killSample = true;
							break;
						}
					}
					spreVal2 = *(SndBuffer + off + 2);
					spreVal2R = *(SndBuffer + off + 3);
				}
			}
			
			if (aa == false) {
				LeftWeight = (1 << BYTEDIV);
				RightWeight = 0;
			}
			
			tShortL = (LeftWeight * spreVal + RightWeight * *(SndBuffer + off + 2)) >> BYTEDIV;
			*ASCBuffer1 += (tShortL * chnVol) >> 16;
			ASCBuffer1 += 2;
			tShortR = (	LeftWeight * spreValR + RightWeight * *(SndBuffer + off + 3)) >> BYTEDIV;
			*ASCBuffer2 += (tShortR * chnVol2) >> 16;
			ASCBuffer2 += 2;
			
			aCC += aDD;
		}
		
		if (killSample) {
			curVoice->samplePtr	= NULL;
			curVoice->curPtr	= curVoice->maxPtr;
		} else {
			if (2*(aCC>>BYTEDIV) == preOff)
				curVoice->preOff = 0;
			else
				curVoice->preOff = 0xFFFFFFFF;
			curVoice->spreVal = spreVal;
			curVoice->spreValR = spreValR;
			curVoice->spreVal2 = *(SndBuffer + off + 2);
			curVoice->spreVal2R = *(SndBuffer + off + 3);
			curVoice->curPtr = (char*)(SndBuffer + 2 * (aCC >> BYTEDIV));
		}
		
		curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
		
		aCC -= aDD;
		curVoice->lastWordL = (tShortL * chnVol) >> 16;
		curVoice->lastWordR = (tShortR * chnVol2) >> 16;
	}
}

void Sample16BufferAddDelay(MADChannel *curVoice, int *ASCBuffer, MADDriverRec *intDriver)
{
	if (curVoice->amp == 16) {
		if (curVoice->stereo)
			Sampler16Addin16DelayStereo(curVoice, ASCBuffer, intDriver);
		else
			Sampler16Addin16Delay(curVoice, ASCBuffer, intDriver);
	} else if (curVoice->amp == 8) {
		if (curVoice->stereo)
			Sampler16AddDelayStereo(curVoice, ASCBuffer, intDriver);
		else
			Sampler16AddDelay(curVoice, ASCBuffer, intDriver);
	}
}

void Play16StereoDelay(MADDriverRec *intDriver)
{
	int		i;
	int		ii;
	short	chanCounter = 0, find, trackID;
	
	for (i = 0; i < MAXCHANEFFECT; i++) {
		intDriver->EffectBufferID[i] = -1;
		intDriver->EffectBufferRealID[i] = -1;
	}
	
	for (i = 0 ; i < intDriver->MultiChanNo; i++) {	//intDriver->DriverSettings.numChn
		//TODO: VST Channel effect
		if (IsVSTChanEffect(intDriver, i) && chanCounter < MAXCHANEFFECT) {
			trackID = intDriver->base.curMusic->header->chanBus[intDriver->base.chan[i].TrackID].copyId;
			// Try to find a DASCEffectBuffer with the same ID
			
			find = -1;
			for (ii = 0; ii < MAXCHANEFFECT; ii++) {
				if (intDriver->EffectBufferID[ii] == trackID)
					find = ii;
			}
			
			if (find == -1) {
				find = chanCounter;
				chanCounter++;
			}
			Sample16BufferAddDelay(&intDriver->base.chan[i], intDriver->DASCEffectBuffer[find], intDriver);
			
			intDriver->EffectBufferID[find] = trackID;
			intDriver->EffectBufferRealID[find] = i;
		} else
			Sample16BufferAddDelay(&intDriver->base.chan[i], intDriver->DASCBuffer, intDriver);
	}
}

void Sampler8in8AddDelay(MADChannel *curVoice, short *ASCBuffer, MADDriverRec *intDriver)
{
	char	tByte;
	int		i = intDriver->ASCBUFFER;
	int		chnVol, chnVol2, off = 0;
	short	*ASCBuffer1, *ASCBuffer2;
	bool	killSample = false;
	int		aDD, aCC = curVoice->lAC;
	
#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
	{
		long long finalperiod;
		long double	temp;
		
		if (intDriver->XMLinear) {
			finalperiod = getfrequency(curVoice->period);
			finalperiod = (8363L * 1712L) / (finalperiod ? finalperiod : 1);
			
			finalperiod = (finalperiod * 8363) / (long) curVoice->fineTune;
		} else
			finalperiod = curVoice->period;
		
		finalperiod = ((long long) finalperiod * (long long) intDriver->base.FreqExt) / (long long)8000;
		
		temp = ((long double)AMIGA_CLOCKFREQ2) / ((long double)finalperiod * (long double)(intDriver->DriverSettings.outPutRate) * (long double)intDriver->DriverSettings.oversampling);
		
		aDD = temp * (1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate) * intDriver->DriverSettings.oversampling);
#endif
	
	if (curVoice->pingpong == true && curVoice->loopType == MADLoopTypePingPong)
		aDD = -aDD;	// PINGPONG
	
	if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
		return;
	
	chnVol2	= DoVolPanning256(0, curVoice, intDriver, true)/256;
	chnVol	= DoVolPanning256(1, curVoice, intDriver, true)/256;
	
	if ((curVoice->TrackID % 2) == 0) {
		ASCBuffer1 = ASCBuffer;
		ASCBuffer2 = ASCBuffer +1L + intDriver->MDelay*2L;
	} else {
		ASCBuffer2 = ASCBuffer +1L;
		ASCBuffer1 = ASCBuffer + intDriver->MDelay*2L;
	}
	
	{
		char	*SndBuffer = curVoice->curPtr;
		int		RightWeight, LeftWeight, preOff = curVoice->preOff;
		char	preVal = curVoice->preVal, preVal2 = curVoice->preVal2;
		
		while (i-- > 0) {
			RightWeight = aCC & ((1 << BYTEDIV) - 1);
			LeftWeight = (1 << BYTEDIV) - RightWeight;
			off = aCC>>BYTEDIV;
			
			if (preOff != off) {
				if (curVoice->loopType == MADLoopTypePingPong && curVoice->loopSize > 0) {		// PINGPONG
					preOff = off;
					if ((SndBuffer + off +1 >= curVoice->maxPtr && !curVoice->pingpong) ||
						(SndBuffer + off +1 <= curVoice->begPtr + curVoice->loopBeg && curVoice->pingpong)) {
						curVoice->pingpong = !curVoice->pingpong;
						aDD = -aDD;
						aCC += aDD;
						RightWeight = aCC & ((1 << BYTEDIV) - 1);
						LeftWeight = (1 << BYTEDIV) - RightWeight;
						off = (int)(aCC>>BYTEDIV);
					}
					preVal = *(SndBuffer + off);
				} else {
					preVal = preVal2;
					preOff = off;
					
					if (SndBuffer + off + 1 >= curVoice->maxPtr) {
						if (curVoice->loopSize > 0) {
							aCC = aCC & ((1 << BYTEDIV) - 1);
							RightWeight = aCC & ((1 << BYTEDIV) - 1);
							LeftWeight = (1 << BYTEDIV) - RightWeight;
							off = (int)(aCC >> BYTEDIV);
							preOff = off;
							
							SndBuffer = curVoice->begPtr + curVoice->loopBeg;
							SndBuffer--;
						} else {	// If TICK remove
							killSample = true;
							break;
						}
					}
					preVal2 = *(SndBuffer + off + 1);
				}
			}
			
			tByte = (LeftWeight * preVal + RightWeight * *(SndBuffer + off + 1)) >> BYTEDIV;
			aCC += aDD;
			
			*ASCBuffer1 += (chnVol * tByte) >> 8;
			ASCBuffer1 += 2;
			*ASCBuffer2 += (chnVol2 * tByte) >> 8;
			ASCBuffer2 += 2;
		}
		
		
		if (killSample) {
			curVoice->samplePtr	= NULL;	curVoice->curPtr	= curVoice->maxPtr;
		} else {
			if ((aCC>>BYTEDIV) == preOff)
				curVoice->preOff = 0;
			else
				curVoice->preOff = 0xFFFFFFFF;
			curVoice->preVal = preVal;
			curVoice->preVal2 = *(SndBuffer + off + 1);
			curVoice->curPtr = SndBuffer + (aCC>>BYTEDIV);
		}
	}
	
	curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
}

void Sampler8in16AddDelay(MADChannel *curVoice, short *ASCBuffer, MADDriverRec *intDriver)
{
	char	tByte;
	int		chnVol, chnVol2;
	int		i = intDriver->ASCBUFFER, off = 0;
	short	*ASCBuffer1, *ASCBuffer2;
	bool	killSample = false;
	int		aDD, aCC = curVoice->lAC;
	
#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
	{
		long long finalperiod;
		long double	temp;
		
		if (intDriver->XMLinear) {
			finalperiod = getfrequency(curVoice->period);
			finalperiod = (8363 * 1712) / (finalperiod ? finalperiod : 1);
			
			finalperiod = (finalperiod * 8363) / curVoice->fineTune;
		} else
			finalperiod = curVoice->period;
		
		finalperiod = ((long long)finalperiod * (long long)intDriver->base.FreqExt) / (long long)8000;
		
		temp = ((long double)AMIGA_CLOCKFREQ2) / ((long double)finalperiod * (long double)(intDriver->DriverSettings.outPutRate) * (long double)intDriver->DriverSettings.oversampling);
		aDD = temp * (1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate) * intDriver->DriverSettings.oversampling);
#endif
	
	if (curVoice->pingpong == true && curVoice->loopType == MADLoopTypePingPong)
		aDD = -aDD;	// PINGPONG
	
	if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
		return;
	
	chnVol2	= DoVolPanning256(0, curVoice, intDriver, true)/256;
	chnVol	= DoVolPanning256(1, curVoice, intDriver, true)/256;
	
	if ((curVoice->TrackID % 2) == 0) {
		ASCBuffer1 = ASCBuffer;
		ASCBuffer2 = ASCBuffer +1L + intDriver->MDelay*2L;
	} else {
		ASCBuffer2 = ASCBuffer +1L;
		ASCBuffer1 = ASCBuffer + intDriver->MDelay*2L;
	}
	
	{
		char	*SndBuffer = curVoice->curPtr;
		int		RightWeight, LeftWeight, preOff = curVoice->preOff;
		char	preVal = curVoice->preVal, preVal2 = curVoice->preVal2;
		
#ifdef WIN32
		SndBuffer++;
#endif
		
		while (i-- > 0) {
			RightWeight = aCC & ((1 << BYTEDIV) - 1);
			LeftWeight = (1 << BYTEDIV) - RightWeight;
			off = aCC >> BYTEDIV;
			
			if (preOff != off) {
				if (curVoice->loopType == MADLoopTypePingPong && curVoice->loopSize > 0) {		// PINGPONG
					preOff = off;
					if((SndBuffer + 2*off +2 >= curVoice->maxPtr && !curVoice->pingpong) ||
					   (SndBuffer + 2*off +2 <= (curVoice->begPtr + curVoice->loopBeg) && curVoice->pingpong)) {
						curVoice->pingpong = !curVoice->pingpong;
						aDD = -aDD;
						aCC += aDD;
						RightWeight = aCC & ((1 << BYTEDIV) - 1);
						LeftWeight = (1 << BYTEDIV) - RightWeight;
						off = (int)(aCC >> BYTEDIV);
					}
					preVal = *(SndBuffer + 2*off);
				} else {
					preVal = preVal2;
					preOff = off;
					
					if (SndBuffer + 2*off +2 >= curVoice->maxPtr) {
						if (curVoice->loopSize > 0) {
							aCC = aCC & ((1 << BYTEDIV) - 1);
							RightWeight = aCC & ((1 << BYTEDIV) - 1);
							LeftWeight = (1 << BYTEDIV) - RightWeight;
							off = (int)(aCC >> BYTEDIV);
							preOff = off;
							
							SndBuffer = (curVoice->begPtr + curVoice->loopBeg);
							SndBuffer -= 2;
						} else {	// If TICK remove
							killSample = true;
							break;
						}
					}
					preVal2 = *(SndBuffer + 2*off + 2);
				}
			}
			
			tByte = (LeftWeight * preVal + RightWeight * *(SndBuffer + 2*off + 2)) >> BYTEDIV;
			aCC += aDD;
			
			*ASCBuffer1 += (chnVol * tByte) >> 8;
			ASCBuffer1 += 2;
			*ASCBuffer2 += (chnVol2 * tByte) >> 8;
			ASCBuffer2 += 2;
		}
		
		if (killSample) {
			curVoice->samplePtr	= NULL;	curVoice->curPtr	= curVoice->maxPtr;
		} else {
			if ((aCC>>BYTEDIV) == preOff)
				curVoice->preOff = 0;
			else
				curVoice->preOff = 0xFFFFFFFF;
			curVoice->preVal = preVal;
			curVoice->preVal2 = *(SndBuffer + 2*off + 2);
			curVoice->curPtr = (SndBuffer + 2 * (aCC >> BYTEDIV));
		}
	}
	
	curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
}

void Sampler8in8AddDelayStereo(MADChannel *curVoice, short *ASCBuffer, MADDriverRec *intDriver)
{
	char	tByteL, tByteR;
	int		i = intDriver->ASCBUFFER;
	int		chnVol, chnVol2;
	int		off = 0;
	short	*ASCBuffer1, *ASCBuffer2;
	bool	killSample = false;
	int		aDD, aCC = curVoice->lAC;
	
#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
	{
		long long finalperiod;
		long double	temp;
		
		if (intDriver->XMLinear) {
			finalperiod = getfrequency(curVoice->period);
			finalperiod = (8363 * 1712) / (finalperiod ? finalperiod : 1);
			
			finalperiod = (finalperiod * 8363L) / curVoice->fineTune;
		} else
			finalperiod = curVoice->period;
		
		finalperiod = ((long long)finalperiod * (long long)intDriver->base.FreqExt) / (long long)8000;
		
		temp = ((long double)AMIGA_CLOCKFREQ2) / ((long double)finalperiod * (long double)(intDriver->DriverSettings.outPutRate) * (long double)intDriver->DriverSettings.oversampling);
		aDD = temp * (1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate) * intDriver->DriverSettings.oversampling);
#endif
	
	if (curVoice->pingpong == true && curVoice->loopType == MADLoopTypePingPong)
		aDD = -aDD;	// PINGPONG
	
	if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
		return;
	
	chnVol2	= DoVolPanning256(0, curVoice, intDriver, true)/256;
	chnVol	= DoVolPanning256(1, curVoice, intDriver, true)/256;
	
	if ((curVoice->TrackID % 2) == 0) {
		ASCBuffer1 = ASCBuffer;
		ASCBuffer2 = ASCBuffer +1L + intDriver->MDelay * 2;
	} else {
		ASCBuffer2 = ASCBuffer +1L;
		ASCBuffer1 = ASCBuffer + intDriver->MDelay * 2;
	}
	
	{
		char	*SndBuffer = curVoice->curPtr;
		int		RightWeight, LeftWeight, preOff = curVoice->preOff;
		char	preVal = curVoice->preVal, preValR = curVoice->preValR, preVal2 = curVoice->preVal2, preVal2R = curVoice->preVal2R;
		
		while (i-- > 0) {
			RightWeight = aCC & ((1 << BYTEDIV) - 1);
			LeftWeight = (1 << BYTEDIV) - RightWeight;
			off = 2 * (aCC >> BYTEDIV);
			
			if (preOff != off) {
				if (curVoice->loopType == MADLoopTypePingPong && curVoice->loopSize > 0) {		// PINGPONG
					preOff = off;
					if((SndBuffer + off +3 >= curVoice->maxPtr && !curVoice->pingpong) ||
					   (SndBuffer + off +2 <= curVoice->begPtr + curVoice->loopBeg && curVoice->pingpong)) {
						curVoice->pingpong = !curVoice->pingpong;
						aDD = -aDD;
						aCC += aDD;
						RightWeight = aCC & ((1 << BYTEDIV) - 1);
						LeftWeight = (1 << BYTEDIV) - RightWeight;
						off = 2 * (aCC >> BYTEDIV);
					}
					preVal = *(SndBuffer + off);
					preValR = *(SndBuffer + off + 1);
				} else {
					preVal = preVal2;
					preValR = preVal2R;
					preOff = off;
					
					if (SndBuffer + off + 3 >= curVoice->maxPtr) {
						if (curVoice->loopSize > 0) {
							aCC = aCC & ((1 << BYTEDIV) - 1);
							RightWeight = aCC & ((1 << BYTEDIV) - 1);
							LeftWeight = (1 << BYTEDIV) - RightWeight;
							off = 2 * (aCC >> BYTEDIV);
							preOff = off;
							
							SndBuffer = curVoice->begPtr + curVoice->loopBeg;
							SndBuffer-=2;
						} else {	// If TICK remove
							killSample = true;
							break;
						}
					}
					preVal2 = *(SndBuffer + off + 2);
					preVal2R = *(SndBuffer + off + 3);
				}
			}
			
			tByteL = (LeftWeight * preVal + RightWeight * *(SndBuffer + off + 2)) >> BYTEDIV;
			*ASCBuffer1 += (chnVol * tByteL) >> 8;
			ASCBuffer1 += 2;
			tByteR = (LeftWeight * preValR + RightWeight * *(SndBuffer + off + 3)) >> BYTEDIV;
			*ASCBuffer2 += (chnVol2 * tByteR) >> 8;
			ASCBuffer2 += 2;
			
			aCC += aDD;
		}
		
		if (killSample) {
			curVoice->samplePtr	= NULL;
			curVoice->curPtr	= curVoice->maxPtr;
		} else {
			if (2*(aCC>>BYTEDIV) == preOff)
				curVoice->preOff = 0;
			else
				curVoice->preOff = 0xFFFFFFFF;
			
			curVoice->preVal = preVal;
			curVoice->preValR = preValR;
			curVoice->preVal2 = *(SndBuffer + off + 2);
			curVoice->preVal2R = *(SndBuffer + off + 3);
			curVoice->curPtr = SndBuffer + 2 * (aCC >> BYTEDIV);
		}
	}
	
	curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
}

void Sampler8in16AddDelayStereo(MADChannel *curVoice, short *ASCBuffer, MADDriverRec *intDriver)
{
	char	tByteL = 0, tByteR = 0;
	int		chnVol, chnVol2;
	int		i = intDriver->ASCBUFFER;
	int		off = 0;
	short	*ASCBuffer1, *ASCBuffer2;
	bool	killSample = false;
	int		aDD, aCC = curVoice->lAC;
	
#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
	{
		long long finalperiod;
		long double	temp;
		
		if (intDriver->XMLinear) {
			finalperiod = getfrequency(curVoice->period);
			finalperiod = (8363 * 1712) / (finalperiod ? finalperiod : 1);
			
			finalperiod = (finalperiod * 8363) / curVoice->fineTune;
		} else
			finalperiod = curVoice->period;
		
		finalperiod = (finalperiod * (long long) intDriver->base.FreqExt) / (long long)8000;
		
		temp = ((long double)AMIGA_CLOCKFREQ2) / ((long double)finalperiod * (long double)(intDriver->DriverSettings.outPutRate) * (long double)intDriver->DriverSettings.oversampling);
		aDD = temp * (1 << BYTEDIV);
	}
#else
	aDD = (AMIGA_CLOCKFREQ2 << BYTEDIV) / (curVoice->period * (intDriver->DriverSettings.outPutRate) * intDriver->DriverSettings.oversampling);
#endif
	
	if (curVoice->pingpong == true && curVoice->loopType == MADLoopTypePingPong)
		aDD = -aDD;	// PINGPONG
	
	if (curVoice->curPtr >= curVoice->maxPtr && curVoice->loopSize == 0)
		return;
	
	chnVol2	= DoVolPanning256(0, curVoice, intDriver, true)/256;
	chnVol	= DoVolPanning256(1, curVoice, intDriver, true)/256;
	
	if ((curVoice->TrackID % 2) == 0) {
		ASCBuffer1 = ASCBuffer;
		ASCBuffer2 = ASCBuffer +1L + intDriver->MDelay*2L;
	} else {
		ASCBuffer2 = ASCBuffer +1L;
		ASCBuffer1 = ASCBuffer + intDriver->MDelay*2L;
	}
	
	{
		char	*SndBuffer = curVoice->curPtr;
		int		RightWeight, LeftWeight, preOff = curVoice->preOff;
		char	preVal = curVoice->preVal, preValR = curVoice->preValR, preVal2 = curVoice->preVal2, preVal2R = curVoice->preVal2R;
		
#ifdef WIN32
		SndBuffer++;
#endif
		
		while (i-- > 0) {
			RightWeight = aCC & ((1 << BYTEDIV) - 1);
			LeftWeight = (1 << BYTEDIV) - RightWeight;
			off = 2*(aCC>>BYTEDIV);
			
			if (preOff != off) {
				if (curVoice->loopType == MADLoopTypePingPong && curVoice->loopSize > 0) {		// PINGPONG
					preOff = off;
					if ((SndBuffer + 2*off +6 >= curVoice->maxPtr && !curVoice->pingpong) ||
					   (SndBuffer + 2*off +4 <= curVoice->begPtr + curVoice->loopBeg && curVoice->pingpong)) {
						curVoice->pingpong = !curVoice->pingpong;
						aDD = -aDD;
						aCC += aDD;
						RightWeight = aCC & ((1 << BYTEDIV) - 1);
						LeftWeight = (1 << BYTEDIV) - RightWeight;
						off = (int)(2 * (aCC >> BYTEDIV));
					}
					preVal = *(SndBuffer + 2*off);
					preValR = *(SndBuffer + 2*off + 2);
				} else {
					preVal = preVal2;
					preValR = preVal2R;
					preOff = off;
					
					if (SndBuffer + 2*off + 6 >= curVoice->maxPtr) {
						if (curVoice->loopSize > 0) {
							aCC = aCC & ((1 << BYTEDIV) - 1);
							RightWeight = aCC & ((1 << BYTEDIV) - 1);
							LeftWeight = (1 << BYTEDIV) - RightWeight;
							off = (int)(2 * (aCC >> BYTEDIV));
							preOff = off;
							
							SndBuffer = curVoice->begPtr + curVoice->loopBeg;
							SndBuffer-=4;
						} else {	// If TICK remove
							killSample = true;
							break;
						}
					}
					preVal2 = *(SndBuffer + 2*off + 4);
					preVal2R = *(SndBuffer + 2*off + 6);
				}
			}
			
			tByteL = (LeftWeight * preVal + RightWeight * *(SndBuffer + 2 * off + 4)) >> BYTEDIV;
			*ASCBuffer1 += (chnVol * tByteL) >> 8;
			ASCBuffer1 += 2;
			tByteR = (LeftWeight * preValR + RightWeight * *(SndBuffer + 2 * off + 6)) >> BYTEDIV;
			*ASCBuffer2 += (chnVol2 * tByteR) >> 8;
			ASCBuffer2 += 2;
			aCC += aDD;
		}
		
		if (killSample) {
			curVoice->samplePtr	= NULL;
			curVoice->curPtr	= curVoice->maxPtr;
		} else {
			if (2*(aCC>>BYTEDIV) == preOff)
				curVoice->preOff = 0;
			else
				curVoice->preOff = 0xFFFFFFFF;
			curVoice->preVal = preVal;
			curVoice->preValR = preValR;
			curVoice->preVal2 = *(SndBuffer + 2*off + 4);
			curVoice->preVal2R = *(SndBuffer + 2*off + 6);
			curVoice->curPtr = SndBuffer + 4 * (aCC >> BYTEDIV);
		}
	}
	
	curVoice->lAC = aCC & ((1 << BYTEDIV) - 1);
}

void Sample8BufferAddDelay(MADChannel *curVoice, short *ASCBuffer, MADDriverRec *intDriver)
{
	if (curVoice->stereo) {
		if (curVoice->amp == 16)
			Sampler8in16AddDelayStereo(curVoice, ASCBuffer, intDriver);
		else
			Sampler8in8AddDelayStereo(curVoice, ASCBuffer, intDriver);
	} else {
		if (curVoice->amp == 16)
			Sampler8in16AddDelay(curVoice, ASCBuffer, intDriver);
		else
			Sampler8in8AddDelay(curVoice, ASCBuffer, intDriver);
	}
}

void Play8StereoDelay(MADDriverRec *intDriver)
{
	int i;
	
	for (i = 0 ; i < intDriver->DriverSettings.numChn; i++) {
		Sample8BufferAddDelay(&intDriver->base.chan[i], intDriver->DASCBuffer8, intDriver);
	}
}
