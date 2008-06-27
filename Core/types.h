// types.h
//
// Author: Eric Nivel
//
//

#ifndef mBrane_types_h
#define mBrane_types_h

#include	<stddef.h>

#include	"config.h"

namespace	mBrane{

#if defined	ARCH_32

	typedef	unsigned	int				word32;
	typedef	unsigned	short			word16;

	typedef	char						int8;
	typedef	unsigned	char			uint8;
	typedef	short						int16;
	typedef	unsigned	short			uint16;
	typedef	int							int32;
	typedef	unsigned	int				uint32;
	typedef	long		long			int64;
	typedef	unsigned	long	long	uint64;
	typedef	float						float32;
	typedef	double						float64;

	typedef	word32						word;
	typedef	word16						half_word;

	#define	HALF_WORD_SHIFT				16
	#define	HALF_WORD_HIGH_MASK			0xFFFF0000
	#define	HALF_WORD_LOW_MASK			0x0000FFFF

#elif defined	ARCH_64

	typedef	unsigned	int				word32;
	typedef	unsigned	long			word64;

	typedef	char						int8;
	typedef	unsigned	char			uint8;
	typedef	short						int16;
	typedef	unsigned	short			uint16;
	typedef	int							int32;
	typedef	unsigned	int				uint32;
	typedef	long						int64;
	typedef	unsigned	long			uint64;
	typedef	float						float32;
	typedef	double						float64;

	typedef	word64						word;
	typedef	word32						half_word;

	#define	HALF_WORD_SHIFT				32
	#define	HALF_WORD_HIGH_MASK			0xFFFFFFFF00000000
	#define	HALF_WORD_LOW_MASK			0x00000000FFFFFFFF

#endif
}


#endif