#import <Cocoa/Cocoa.h>

@interface XeeFSRef:NSObject <NSCopying>
{
	FSRef ref;
	FSIterator iterator;
	int hash;
}

+(instancetype)refForPath:(NSString *)path;

-(instancetype)initWithPath:(NSString *)path;
-(instancetype)initWithFSRef:(FSRef *)fsref;

-(FSRef *)FSRef NS_RETURNS_INNER_POINTER;

@property (readonly, getter=isValid) BOOL valid;
@property (readonly, getter=isDirectory) BOOL directory;
@property (readonly, getter=isRemote) BOOL remote;

-(NSString *)name;
-(NSString *)path;
-(NSURL *)URL;
-(XeeFSRef *)parent;

-(off_t)dataSize;
-(off_t)dataPhysicalSize;
-(off_t)resourceSize;
-(off_t)resourcePhysicalSize;

-(CFAbsoluteTime)creationTime;
-(CFAbsoluteTime)modificationTime;
-(CFAbsoluteTime)attributeModificationTime;
-(CFAbsoluteTime)accessTime;
-(CFAbsoluteTime)backupTime;

-(NSString *)HFSTypeCode;
-(NSString *)HFSCreatorCode;

-(BOOL)startReadingDirectoryWithRecursion:(BOOL)recursive;
-(void)stopReadingDirectory;
-(XeeFSRef *)nextDirectoryEntry;
-(NSArray *)directoryContents;

-(BOOL)isEqual:(XeeFSRef *)other;
-(NSComparisonResult)compare:(XeeFSRef *)other;
-(NSComparisonResult)compare:(XeeFSRef *)other options:(int)options;
-(unsigned)hash;

-(NSString *)description;

-(id)copyWithZone:(NSZone *)zone;

@end
