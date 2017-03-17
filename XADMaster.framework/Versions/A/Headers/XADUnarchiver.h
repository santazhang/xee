#import <Foundation/Foundation.h>

#import "XADArchiveParser.h"

typedef NS_ENUM(int, XADForkStyle) {
	XADForkStyleIgnored = 0,
	XADForkStyleMacOSX = 1,
	XADForkStyleHiddenAppleDouble = 2,
	XADForkStyleVisibleAppleDouble = 3,
	XADForkStyleHFVExplorerAppleDouble = 4,
	
#ifdef __APPLE__
	XADForkStyleDefault = XADForkStyleMacOSX,
#else
	XADForkStyleDefault = XADVisibleAppleDoubleForkStyle,
#endif

	XADIgnoredForkStyle NS_SWIFT_UNAVAILABLE("Use '.ignored'") = XADForkStyleIgnored,
	XADMacOSXForkStyle NS_SWIFT_UNAVAILABLE("Use '.macOSX'") = XADForkStyleMacOSX,
	XADHiddenAppleDoubleForkStyle NS_SWIFT_UNAVAILABLE("Use '.hiddenAppleDouble'") = XADForkStyleHiddenAppleDouble,
	XADVisibleAppleDoubleForkStyle NS_SWIFT_UNAVAILABLE("Use '.visibleAppleDouble'") = XADForkStyleVisibleAppleDouble,
	XADHFVExplorerAppleDoubleForkStyle NS_SWIFT_UNAVAILABLE("Use '.HFVExplorerAppleDouble'") = XADForkStyleHFVExplorerAppleDouble,
	
	XADDefaultForkStyle NS_SWIFT_UNAVAILABLE("Use '.default'") = XADForkStyleDefault,
};

NS_ASSUME_NONNULL_BEGIN

@protocol XADUnarchiverDelegate;

@interface XADUnarchiver:NSObject <XADArchiveParserDelegate>
{
	XADArchiveParser *parser;
	BOOL preservepermissions;

	BOOL shouldstop;

	NSMutableArray *deferreddirectories,*deferredlinks;
#if !__OBJC2__
	__unsafe_unretained id<XADUnarchiverDelegate> delegate;
	NSString *destination;
	XADForkStyle forkstyle;
	double updateinterval;
#endif
}

+(nullable instancetype)unarchiverForArchiveParser:(XADArchiveParser *)archiveparser;
+(nullable instancetype)unarchiverForPath:(NSString *)path NS_SWIFT_UNAVAILABLE("Call may throw exceptions, use init(for:error:) instead");
+(nullable instancetype)unarchiverForPath:(NSString *)path error:(nullable XADError *)errorptr;

-(instancetype)init UNAVAILABLE_ATTRIBUTE;
-(instancetype)initWithArchiveParser:(XADArchiveParser *)archiveparser NS_DESIGNATED_INITIALIZER;

@property (NS_NONATOMIC_IOSONLY, readonly, strong) XADArchiveParser *archiveParser;

@property (NS_NONATOMIC_IOSONLY, assign, nullable) id<XADUnarchiverDelegate> delegate;

@property (NS_NONATOMIC_IOSONLY, copy) NSString *destination;

@property (NS_NONATOMIC_IOSONLY) XADForkStyle macResourceForkStyle;

@property (NS_NONATOMIC_IOSONLY, setter=setPreserevesPermissions:) BOOL preservesPermissions;

@property (NS_NONATOMIC_IOSONLY) double updateInterval;

-(XADError)parseAndUnarchive;

-(XADError)extractEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict;
-(XADError)extractEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict forceDirectories:(BOOL)force;
-(XADError)extractEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict as:(nullable NSString *)path;
-(XADError)extractEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict as:(nullable NSString *)path forceDirectories:(BOOL)force;

-(XADError)finishExtractions;
-(XADError)_fixDeferredLinks;
-(XADError)_fixDeferredDirectories;

-(nullable XADUnarchiver *)unarchiverForEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict
wantChecksum:(BOOL)checksum error:(nullable XADError *)errorptr;
-(nullable XADUnarchiver *)unarchiverForEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict
resourceForkDictionary:(nullable NSDictionary<XADArchiveKeys,id> *)forkdict wantChecksum:(BOOL)checksum error:(nullable XADError *)errorptr;

-(XADError)_extractFileEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict as:(NSString *)destpath;
-(XADError)_extractDirectoryEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict as:(NSString *)destpath;
-(XADError)_extractLinkEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict as:(NSString *)destpath;
-(XADError)_extractArchiveEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict to:(NSString *)destpath name:(NSString *)filename;
-(XADError)_extractResourceForkEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict asAppleDoubleFile:(NSString *)destpath;

-(XADError)_updateFileAttributesAtPath:(NSString *)path forEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict
deferDirectories:(BOOL)defer;
-(XADError)_ensureDirectoryExists:(NSString *)path;

-(XADError)runExtractorWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict outputHandle:(CSHandle *)handle;
-(XADError)runExtractorWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict
outputTarget:(id)target selector:(SEL)sel argument:(id)arg;

-(NSString *)adjustPathString:(NSString *)path forEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict;

@property (NS_NONATOMIC_IOSONLY, readonly) BOOL _shouldStop;

@end


@protocol XADUnarchiverDelegate <NSObject>

@optional
-(void)unarchiverNeedsPassword:(XADUnarchiver *)unarchiver;

-(BOOL)unarchiver:(XADUnarchiver *)unarchiver shouldExtractEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict suggestedPath:(NSString *__nullable*__nullable)pathptr;
-(void)unarchiver:(XADUnarchiver *)unarchiver willExtractEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict to:(NSString *)path;
-(void)unarchiver:(XADUnarchiver *)unarchiver didExtractEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict to:(NSString *)path error:(XADError)error;

@required
-(BOOL)unarchiver:(XADUnarchiver *)unarchiver shouldCreateDirectory:(NSString *)directory;
@optional
-(BOOL)unarchiver:(XADUnarchiver *)unarchiver shouldDeleteFileAndCreateDirectory:(NSString *)directory;

@optional
-(BOOL)unarchiver:(XADUnarchiver *)unarchiver shouldExtractArchiveEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict to:(NSString *)path;
-(void)unarchiver:(XADUnarchiver *)unarchiver willExtractArchiveEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict withUnarchiver:(XADUnarchiver *)subunarchiver to:(NSString *)path;
-(void)unarchiver:(XADUnarchiver *)unarchiver didExtractArchiveEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict withUnarchiver:(XADUnarchiver *)subunarchiver to:(NSString *)path error:(XADError)error;

@required
-(nullable NSString *)unarchiver:(XADUnarchiver *)unarchiver destinationForLink:(XADString *)link from:(NSString *)path;

-(BOOL)extractionShouldStopForUnarchiver:(XADUnarchiver *)unarchiver;
-(void)unarchiver:(XADUnarchiver *)unarchiver extractionProgressForEntryWithDictionary:(NSDictionary<XADArchiveKeys,id> *)dict
fileFraction:(double)fileprogress estimatedTotalFraction:(double)totalprogress;

@optional
-(void)unarchiver:(XADUnarchiver *)unarchiver findsFileInterestingForReason:(NSString *)reason;

@optional
// Deprecated.
-(null_unspecified NSString *)unarchiver:(XADUnarchiver *)unarchiver pathForExtractingEntryWithDictionary:(null_unspecified NSDictionary *)dict DEPRECATED_ATTRIBUTE;
-(BOOL)unarchiver:(XADUnarchiver *)unarchiver shouldExtractEntryWithDictionary:(null_unspecified NSDictionary *)dict to:(null_unspecified NSString *)path DEPRECATED_ATTRIBUTE;
-(null_unspecified NSString *)unarchiver:(XADUnarchiver *)unarchiver linkDestinationForEntryWithDictionary:(null_unspecified NSDictionary *)dict from:(null_unspecified NSString *)path DEPRECATED_ATTRIBUTE;
@end

NS_ASSUME_NONNULL_END

