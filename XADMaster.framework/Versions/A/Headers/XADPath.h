#import "XADString.h"

#define XADUnixPathSeparator "/"
#define XADWindowsPathSeparator "\\"
#define XADEitherPathSeparator "/\\"
#define XADNoPathSeparator ""

@interface XADPath:NSObject <XADString,NSCopying>
{
	XADPath *parent;

	NSArray *cachedcanonicalcomponents;
	NSString *cachedencoding;
}

+(XADPath *)emptyPath;
#if __has_feature(objc_class_property)
@property (class, readonly, copy) XADPath *emptyPath;
#endif
+(instancetype)pathWithString:(NSString *)string;
+(instancetype)pathWithStringComponents:(NSArray<NSString*> *)components;
+(instancetype)separatedPathWithString:(NSString *)string;
+(instancetype)decodedPathWithData:(NSData *)bytedata encodingName:(XADStringEncodingName)encoding separators:(const char *)separators;
+(instancetype)analyzedPathWithData:(NSData *)bytedata source:(XADStringSource *)stringsource
separators:(const char *)pathseparators;

-(instancetype)init NS_DESIGNATED_INITIALIZER;
-(instancetype)initWithParent:(XADPath *)parentpath NS_DESIGNATED_INITIALIZER;
-(instancetype)initWithPath:(XADPath *)path parent:(XADPath *)parentpath;

@property (NS_NONATOMIC_IOSONLY, getter=isAbsolute, readonly) BOOL absolute;
@property (NS_NONATOMIC_IOSONLY, getter=isEmpty, readonly) BOOL empty;
-(BOOL)isCanonicallyEqual:(id)other;
-(BOOL)isCanonicallyEqual:(id)other encodingName:(XADStringEncodingName)encoding;
-(BOOL)hasPrefix:(XADPath *)other;
-(BOOL)hasCanonicalPrefix:(XADPath *)other;
-(BOOL)hasCanonicalPrefix:(XADPath *)other encodingName:(XADStringEncodingName)encoding;

@property (NS_NONATOMIC_IOSONLY, readonly) NSInteger depth; // Note: Does not take . or .. paths into account.
-(NSInteger)depthWithEncodingName:(NSString *)encoding;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSArray<NSString*> *pathComponents;
-(NSArray<NSString*> *)pathComponentsWithEncodingName:(XADStringEncodingName)encoding;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSArray<NSString*> *canonicalPathComponents;
-(NSArray<NSString*> *)canonicalPathComponentsWithEncodingName:(XADStringEncodingName)encoding;
-(void)_addPathComponentsToArray:(NSMutableArray *)components encodingName:(XADStringEncodingName)encoding;

@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSString *lastPathComponent;
-(NSString *)lastPathComponentWithEncodingName:(XADStringEncodingName)encoding;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSString *firstPathComponent;
-(NSString *)firstPathComponentWithEncodingName:(XADStringEncodingName)encoding;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSString *firstCanonicalPathComponent;
-(NSString *)firstCanonicalPathComponentWithEncodingName:(XADStringEncodingName)encoding;

@property (NS_NONATOMIC_IOSONLY, readonly, copy) XADPath *pathByDeletingLastPathComponent;
-(XADPath *)pathByDeletingLastPathComponentWithEncodingName:(XADStringEncodingName)encoding;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) XADPath *pathByDeletingFirstPathComponent;
-(XADPath *)pathByDeletingFirstPathComponentWithEncodingName:(XADStringEncodingName)encoding;

-(XADPath *)pathByAppendingXADStringComponent:(XADString *)component;
-(XADPath *)pathByAppendingPath:(XADPath *)path;
-(XADPath *)_copyWithParent:(XADPath *)newparent;

// These are safe for filesystem use, and adapted to the current platform.
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSString *sanitizedPathString;
-(NSString *)sanitizedPathStringWithEncodingName:(XADStringEncodingName)encoding;

// XADString interface.
// NOTE: These are not guaranteed to be safe for usage as filesystem paths,
// only for display!
-(BOOL)canDecodeWithEncodingName:(XADStringEncodingName)encoding;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSString *string;
-(NSString *)stringWithEncodingName:(XADStringEncodingName)encoding;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) NSData *data;
-(void)_appendPathToData:(NSMutableData *)data;

@property (NS_NONATOMIC_IOSONLY, readonly) BOOL encodingIsKnown;
@property (NS_NONATOMIC_IOSONLY, readonly, copy) XADStringEncodingName encodingName;
@property (NS_NONATOMIC_IOSONLY, readonly) float confidence;

@property (NS_NONATOMIC_IOSONLY, readonly, strong) XADStringSource *source;

#ifdef __APPLE__
-(BOOL)canDecodeWithEncoding:(NSStringEncoding)encoding;
-(NSString *)stringWithEncoding:(NSStringEncoding)encoding;
-(NSString *)sanitizedPathStringWithEncoding:(NSStringEncoding)encoding;
@property (NS_NONATOMIC_IOSONLY, readonly) NSStringEncoding encoding;
#endif

// Other interfaces.
@property (NS_NONATOMIC_IOSONLY, readonly) NSUInteger hash;

// Deprecated.
@property (NS_NONATOMIC_IOSONLY, readonly, copy) XADPath *safePath DEPRECATED_ATTRIBUTE; // Deprecated. Use sanitizedPathString: instead.

// Subclass methods.
@property (NS_NONATOMIC_IOSONLY, readonly) BOOL _isPartAbsolute;
@property (NS_NONATOMIC_IOSONLY, readonly) BOOL _isPartEmpty;
-(NSInteger)_depthOfPartWithEncodingName:(NSString *)encoding;
-(void)_addPathComponentsOfPartToArray:(NSMutableArray *)array encodingName:(XADStringEncodingName)encoding;
-(NSString *)_lastPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(NSString *)_firstPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(XADPath *)_pathByDeletingLastPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(XADPath *)_pathByDeletingFirstPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(BOOL)_canDecodePartWithEncodingName:(XADStringEncodingName)encoding;
-(void)_appendPathForPartToData:(NSMutableData *)data;
@property (NS_NONATOMIC_IOSONLY, readonly, strong) XADStringSource *_sourceForPart;

@end


@interface XADStringPath:XADPath
{
	NSString *string;
}

-(instancetype)init UNAVAILABLE_ATTRIBUTE;
-(instancetype)initWithComponentString:(NSString *)pathstring NS_DESIGNATED_INITIALIZER;
-(instancetype)initWithComponentString:(NSString *)pathstring parent:(XADPath *)parentpath NS_DESIGNATED_INITIALIZER;
-(instancetype)initWithPath:(XADStringPath *)path parent:(XADPath *)parentpath;

@property (NS_NONATOMIC_IOSONLY, readonly) BOOL _isPartAbsolute;
@property (NS_NONATOMIC_IOSONLY, readonly) BOOL _isPartEmpty;
-(NSInteger)_depthOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(void)_addPathComponentsOfPartToArray:(NSMutableArray *)array encodingName:(XADStringEncodingName)encoding;
-(NSString *)_lastPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(NSString *)_firstPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(XADPath *)_pathByDeletingLastPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(XADPath *)_pathByDeletingFirstPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(BOOL)_canDecodePartWithEncodingName:(XADStringEncodingName)encoding;
-(void)_appendPathForPartToData:(NSMutableData *)data;
@property (NS_NONATOMIC_IOSONLY, readonly, strong) XADStringSource *_sourceForPart;

@end

@interface XADRawPath:XADPath
{
	NSData *data;
	XADStringSource *source;
	const char *separators;
}

-(instancetype)init UNAVAILABLE_ATTRIBUTE;
-(instancetype)initWithData:(NSData *)bytedata source:(XADStringSource *)stringsource
separators:(const char *)pathseparators NS_DESIGNATED_INITIALIZER;
-(instancetype)initWithData:(NSData *)bytedata source:(XADStringSource *)stringsource
separators:(const char *)pathseparators parent:(XADPath *)parentpath NS_DESIGNATED_INITIALIZER;
-(instancetype)initWithPath:(XADRawPath *)path parent:(XADPath *)parentpath;

@property (NS_NONATOMIC_IOSONLY, readonly) BOOL _isPartAbsolute;
@property (NS_NONATOMIC_IOSONLY, readonly) BOOL _isPartEmpty;
-(NSInteger)_depthOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(void)_addPathComponentsOfPartToArray:(NSMutableArray<NSString*> *)array encodingName:(XADStringEncodingName)encoding;
-(NSString *)_lastPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(NSString *)_firstPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(XADPath *)_pathByDeletingLastPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(XADPath *)_pathByDeletingFirstPathComponentOfPartWithEncodingName:(XADStringEncodingName)encoding;
-(BOOL)_canDecodePartWithEncodingName:(XADStringEncodingName)encoding;
-(void)_appendPathForPartToData:(NSMutableData *)data;
@property (NS_NONATOMIC_IOSONLY, readonly, strong) XADStringSource *_sourceForPart;

@end

