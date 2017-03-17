#import "CSHandle.h"

#define CSMultiHandle XADMultiHandle

extern NSString *const CSSizeOfSegmentUnknownException;

@interface CSMultiHandle:CSHandle
{
	NSArray<CSHandle*> *handles;
	NSInteger currhandle;
}

+(CSMultiHandle *)multiHandleWithHandleArray:(NSArray<CSHandle*> *)handlearray;
+(CSMultiHandle *)multiHandleWithHandles:(CSHandle *)firsthandle,... NS_REQUIRES_NIL_TERMINATION;

// Initializers
-(instancetype)initWithHandles:(NSArray<CSHandle*> *)handlearray;
-(instancetype)initAsCopyOf:(CSMultiHandle *)other;

// Public methods
@property (readonly, retain) NSArray<CSHandle*> *handles;
@property (readonly) CSHandle *currentHandle;

// Implemented by this class
@property (NS_NONATOMIC_IOSONLY, readonly) off_t fileSize;
@property (NS_NONATOMIC_IOSONLY, readonly) off_t offsetInFile;
@property (NS_NONATOMIC_IOSONLY, readonly) BOOL atEndOfFile;

-(void)seekToFileOffset:(off_t)offs;
-(void)seekToEndOfFile;
-(int)readAtMost:(int)num toBuffer:(void *)buffer;

// Internal methods
-(void)_raiseSizeUnknownForSegment:(long)i;

@end
