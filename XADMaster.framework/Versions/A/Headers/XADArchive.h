#import <Foundation/Foundation.h>

#import "XADArchiveParser.h"
#import "XADUnarchiver.h"
#import "XADException.h"

#ifdef __has_feature
#  if __has_feature(modules)
#    define XAD_NO_DEPRECATED
#  endif
#endif

typedef NS_ENUM(int, XADAction) {
	XADAbortAction = 0,
	XADRetryAction = 1,
	XADSkipAction = 2,
	XADOverwriteAction = 3,
	XADRenameAction = 4
};

//typedef off_t xadSize; // deprecated


extern NSString *const XADResourceDataKey;
extern NSString *const XADResourceForkData UNAVAILABLE_ATTRIBUTE;
extern NSString *const XADFinderFlags;


@class UniversalDetector;
@class XADArchive;
@protocol XADArchiveDelegate <NSObject>
@optional

-(NSStringEncoding)archive:(XADArchive *)archive encodingForData:(NSData *)data guess:(NSStringEncoding)guess confidence:(float)confidence;
-(XADAction)archive:(XADArchive *)archive nameDecodingDidFailForEntry:(NSInteger)n data:(NSData *)data;

-(BOOL)archiveExtractionShouldStop:(XADArchive *)archive;
-(BOOL)archive:(XADArchive *)archive shouldCreateDirectory:(NSString *)directory;
-(XADAction)archive:(XADArchive *)archive entry:(NSInteger)n collidesWithFile:(NSString *)file newFilename:(NSString **)newname;
-(XADAction)archive:(XADArchive *)archive entry:(NSInteger)n collidesWithDirectory:(NSString *)file newFilename:(NSString **)newname;
-(XADAction)archive:(XADArchive *)archive creatingDirectoryDidFailForEntry:(NSInteger)n;

-(void)archiveNeedsPassword:(XADArchive *)archive;

-(void)archive:(XADArchive *)archive extractionOfEntryWillStart:(NSInteger)n;
-(void)archive:(XADArchive *)archive extractionProgressForEntry:(NSInteger)n bytes:(off_t)bytes of:(off_t)total;
-(void)archive:(XADArchive *)archive extractionOfEntryDidSucceed:(NSInteger)n;
-(XADAction)archive:(XADArchive *)archive extractionOfEntryDidFail:(NSInteger)n error:(XADError)error;
-(XADAction)archive:(XADArchive *)archive extractionOfResourceForkForEntryDidFail:(NSInteger)n error:(XADError)error;

-(void)archive:(XADArchive *)archive extractionProgressBytes:(off_t)bytes of:(off_t)total;

@optional
-(void)archive:(XADArchive *)archive extractionProgressFiles:(NSInteger)files of:(NSInteger)total;

@optional
// Deprecated
-(NSStringEncoding)archive:(XADArchive *)archive encodingForName:(const char *)bytes guess:(NSStringEncoding)guess confidence:(float)confidence DEPRECATED_ATTRIBUTE;
-(XADAction)archive:(XADArchive *)archive nameDecodingDidFailForEntry:(NSInteger)n bytes:(const char *)bytes DEPRECATED_ATTRIBUTE;

@end

@interface XADArchive:NSObject <XADArchiveDelegate, XADUnarchiverDelegate, XADArchiveParserDelegate>
{
	XADArchiveParser *parser;
	XADUnarchiver *unarchiver;

	NSTimeInterval update_interval;
	XADError lasterror;

	NSMutableArray *dataentries,*resourceentries;
	NSMutableDictionary *namedict;

	off_t extractsize,totalsize;
	NSInteger extractingentry;
	BOOL extractingresource;
	NSString *immediatedestination;
	BOOL immediatesubarchives,immediatefailed;
	off_t immediatesize;
	XADArchive *parentarchive;
#if !__OBJC2__
	__unsafe_unretained id<XADArchiveDelegate> delegate;
#endif
}

+(instancetype)archiveForFile:(NSString *)filename;
+(instancetype)recursiveArchiveForFile:(NSString *)filename;



-(instancetype)init NS_DESIGNATED_INITIALIZER;
-(instancetype)initWithFile:(NSString *)file NS_SWIFT_UNAVAILABLE("Call throws on failure");
-(instancetype)initWithFile:(NSString *)file error:(XADError *)error;
-(instancetype)initWithFile:(NSString *)file delegate:(id<XADArchiveDelegate>)del error:(XADError *)error;
-(instancetype)initWithData:(NSData *)data NS_SWIFT_UNAVAILABLE("Call throws on failure");
-(instancetype)initWithData:(NSData *)data error:(XADError *)error;
-(instancetype)initWithData:(NSData *)data delegate:(id<XADArchiveDelegate>)del error:(XADError *)error;
-(instancetype)initWithArchive:(XADArchive *)archive entry:(NSInteger)n NS_SWIFT_UNAVAILABLE("Call throws on failure");
-(instancetype)initWithArchive:(XADArchive *)archive entry:(NSInteger)n error:(XADError *)error;
-(instancetype)initWithArchive:(XADArchive *)otherarchive entry:(NSInteger)n delegate:(id<XADArchiveDelegate>)del error:(XADError *)error;
-(instancetype)initWithArchive:(XADArchive *)otherarchive entry:(NSInteger)n
     immediateExtractionTo:(NSString *)destination error:(XADError *)error;
-(instancetype)initWithArchive:(XADArchive *)otherarchive entry:(NSInteger)n
     immediateExtractionTo:(NSString *)destination subArchives:(BOOL)sub error:(XADError *)error;

-(BOOL)_parseWithErrorPointer:(XADError *)error;

@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSString *filename;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSArray<NSString*> *allFilenames;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSString *formatName;
@property (NS_NONATOMIC_IOSONLY, getter=isEncrypted, readonly) BOOL encrypted;
@property (NS_NONATOMIC_IOSONLY, getter=isSolid, readonly) BOOL solid;
@property (NS_NONATOMIC_IOSONLY, getter=isCorrupted, readonly) BOOL corrupted;
@property (NS_NONATOMIC_IOSONLY, readonly) NSInteger numberOfEntries;
@property (NS_NONATOMIC_IOSONLY, readonly) BOOL immediateExtractionFailed;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSString *commonTopDirectory;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSString *comment;

@property (NS_NONATOMIC_IOSONLY, assign) id<XADArchiveDelegate> delegate;

@property (NS_NONATOMIC_IOSONLY, copy) NSString *password;

@property (NS_NONATOMIC_IOSONLY) NSStringEncoding nameEncoding;

@property (NS_NONATOMIC_IOSONLY, readonly) XADError lastError;
-(void)clearLastError;
-(NSString *)describeLastError;
-(NSString *)describeError:(XADError)error;

@property (readonly, copy) NSString *description;



-(NSDictionary *)dataForkParserDictionaryForEntry:(NSInteger)n;
-(NSDictionary *)resourceForkParserDictionaryForEntry:(NSInteger)n;
-(NSDictionary *)combinedParserDictionaryForEntry:(NSInteger)n;

-(NSString *)nameOfEntry:(NSInteger)n;
-(BOOL)entryHasSize:(NSInteger)n;
-(off_t)uncompressedSizeOfEntry:(NSInteger)n;
-(off_t)compressedSizeOfEntry:(NSInteger)n;
-(off_t)representativeSizeOfEntry:(NSInteger)n;
-(BOOL)entryIsDirectory:(NSInteger)n;
-(BOOL)entryIsLink:(NSInteger)n;
-(BOOL)entryIsEncrypted:(NSInteger)n;
-(BOOL)entryIsArchive:(NSInteger)n;
-(BOOL)entryHasResourceFork:(NSInteger)n;
-(NSString *)commentForEntry:(NSInteger)n;
-(NSDictionary *)attributesOfEntry:(NSInteger)n;
-(NSDictionary *)attributesOfEntry:(NSInteger)n withResourceFork:(BOOL)resfork;
-(CSHandle *)handleForEntry:(NSInteger)n;
-(CSHandle *)handleForEntry:(NSInteger)n error:(XADError *)error;
-(CSHandle *)resourceHandleForEntry:(NSInteger)n;
-(CSHandle *)resourceHandleForEntry:(NSInteger)n error:(XADError *)error;
-(NSData *)contentsOfEntry:(NSInteger)n;
//-(NSData *)resourceContentsOfEntry:(int)n;

-(BOOL)extractTo:(NSString *)destination;
-(BOOL)extractTo:(NSString *)destination subArchives:(BOOL)sub;
-(BOOL)extractEntries:(NSIndexSet *)entryset to:(NSString *)destination;
-(BOOL)extractEntries:(NSIndexSet *)entryset to:(NSString *)destination subArchives:(BOOL)sub;
-(BOOL)extractEntry:(NSInteger)n to:(NSString *)destination;
-(BOOL)extractEntry:(NSInteger)n to:(NSString *)destination deferDirectories:(BOOL)defer;
-(BOOL)extractEntry:(NSInteger)n to:(NSString *)destination deferDirectories:(BOOL)defer
resourceFork:(BOOL)resfork;
-(BOOL)extractEntry:(NSInteger)n to:(NSString *)destination deferDirectories:(BOOL)defer
dataFork:(BOOL)datafork resourceFork:(BOOL)resfork;
-(BOOL)extractArchiveEntry:(NSInteger)n to:(NSString *)destination;

-(BOOL)_extractEntry:(NSInteger)n as:(NSString *)destfile deferDirectories:(BOOL)defer
dataFork:(BOOL)datafork resourceFork:(BOOL)resfork;

-(void)updateAttributesForDeferredDirectories;

//Tim Oliver
- (BOOL)extractContentsOfEntry:(NSInteger)n toPath:(NSString *)destination;

// Deprecated

+(NSArray *)volumesForFile:(NSString *)filename DEPRECATED_ATTRIBUTE;

-(int)sizeOfEntry:(int)n DEPRECATED_ATTRIBUTE;
-(void *)xadFileInfoForEntry:(int)n NS_RETURNS_INNER_POINTER DEPRECATED_ATTRIBUTE;
-(BOOL)extractEntry:(int)n to:(NSString *)destination overrideWritePermissions:(BOOL)override DEPRECATED_ATTRIBUTE;
-(BOOL)extractEntry:(int)n to:(NSString *)destination overrideWritePermissions:(BOOL)override resourceFork:(BOOL)resfork DEPRECATED_ATTRIBUTE;
-(void)fixWritePermissions DEPRECATED_ATTRIBUTE;

@end


#ifndef XAD_NO_DEPRECATED

#define XADAbort XADAbortAction
#define XADRetry XADRetryAction
#define XADSkip XADSkipAction
#define XADOverwrite XADOverwriteAction
#define XADRename XADRenameAction

typedef XADError xadERROR;
typedef off_t xadSize;

#define XADERR_NO XADNoError
#if 0
#define XADUnknownError          0x0001 /* unknown error */
#define XADInputError            0x0002 /* input data buffers border exceeded */
#define XADOutputError           0x0003 /* output data buffers border exceeded */
#define XADBadParametersError    0x0004 /* function called with illegal parameters */
#define XADOutOfMemoryError      0x0005 /* not enough memory available */
#define XADIllegalDataError      0x0006 /* data is corrupted */
#define XADNotSupportedError     0x0007 /* command is not supported */
#define XADResourceError         0x0008 /* required resource missing */
#define XADDecrunchError         0x0009 /* error on decrunching */
#define XADFiletypeError         0x000A /* unknown file type */
#define XADOpenFileError         0x000B /* opening file failed */
#define XADSkipError             0x000C /* file, disk has been skipped */
#define XADBreakError            0x000D /* user break in progress hook */
#define XADFileExistsError       0x000E /* file already exists */
#define XADPasswordError         0x000F /* missing or wrong password */
#define XADMakeDirectoryError    0x0010 /* could not create directory */
#define XADChecksumError         0x0011 /* wrong checksum */
#define XADVerifyError           0x0012 /* verify failed (disk hook) */
#define XADGeometryError         0x0013 /* wrong drive geometry */
#define XADDataFormatError       0x0014 /* unknown data format */
#define XADEmptyError            0x0015 /* source contains no files */
#define XADFileSystemError       0x0016 /* unknown filesystem */
#define XADFileDirectoryError    0x0017 /* name of file exists as directory */
#define XADShortBufferError      0x0018 /* buffer was too short */
#define XADEncodingError         0x0019 /* text encoding was defective */
#endif

#define XADAbort XADAbortAction
#define XADRetry XADRetryAction
#define XADSkip XADSkipAction
#define XADOverwrite XADOverwriteAction
#define XADRename XADRenameAction

#endif
