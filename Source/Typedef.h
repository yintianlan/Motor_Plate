/******************************************************************************
*
* Yinqiao Electronic Technology Co., LTD.
* (c) Copyright 2021 Yinqiao Electronic Technology Co., LTD.
* ALL RIGHTS RESERVED.
*
* @file Typedef.h
*
* @date Sep. 16, 2021
*
* @brief Provide some type definitions which are not CMSIS style. 
***************************************************************************/
#ifndef __TYPEDEF_H_
#define __TYPEDEF_H_

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef Bool
typedef enum {FALSE = 0, TRUE = !FALSE} Bool;
typedef Bool BOOL;
#endif

#ifndef uchar
#define	uchar unsigned char
#define	uint  unsigned int
#define	ulong unsigned long
#endif

#ifndef s8
#define s8		char
#define s16		short
#define s32		long
//#define s64		long long
#endif

#ifndef BYTE
typedef unsigned char  BYTE;
typedef unsigned int   UINT;
typedef unsigned short WORD;
typedef unsigned short UINT16;
typedef unsigned int  UINT32;
typedef unsigned int  DWORD;
typedef unsigned int  ULONG;
#endif

#ifndef u8
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int  u32;
//typedef unsigned long long  u64;
#endif

#ifndef U8
typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int  U32;
//typedef unsigned long long  U64;
#endif

#ifndef Uint8
typedef unsigned char  Uint8;
typedef unsigned short Uint16;
typedef unsigned int  Uint32;
//typedef unsigned long long  Uint64;
#endif

#ifndef uint8
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
//typedef unsigned long long  uint64;
#endif

#ifndef uint8_t
typedef unsigned char  uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
//typedef unsigned long long  uint64_t;
#endif


#ifdef __cplusplus
}
#endif

#endif	/* __TYPEDEF_H_ */
