#import "CSStreamHandle.h"

#include <bzlib.h>

#define CSBzip2Handle XADBzip2Handle

extern NSString *const CSBzip2Exception;

@interface CSBzip2Handle:CSStreamHandle
{
	CSHandle *parent;
	off_t startoffs;
	bz_stream bzs;
	BOOL inited,checksumcorrect;

	uint8_t inbuffer[16*1024];
}

+(CSBzip2Handle *)bzip2HandleWithHandle:(CSHandle *)handle;
+(CSBzip2Handle *)bzip2HandleWithHandle:(CSHandle *)handle length:(off_t)length;

// Initializers.
-(instancetype)initWithHandle:(CSHandle *)handle length:(off_t)length name:(NSString *)descname;

// Implemented by this class.
-(void)resetStream;
-(int)streamAtMost:(int)num toBuffer:(void *)buffer;

// Checksum functions for XADMaster.
@property (NS_NONATOMIC_IOSONLY, readonly) BOOL hasChecksum;
@property (NS_NONATOMIC_IOSONLY, readonly, getter=isChecksumCorrect) BOOL checksumCorrect;

// Internal methods.
-(void)_raiseBzip2:(int)error NS_SWIFT_UNAVAILABLE("Call throws");

@end
