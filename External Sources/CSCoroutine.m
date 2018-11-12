#include <Availability.h>
#import <Foundation/Foundation.h>
#ifndef MAC_OS_X_VERSION_MIN_REQUIRED
#define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_1
#endif
#ifndef MAC_OS_X_VERSION_MAX_ALLOWED
#define MAC_OS_X_VERSION_MAX_ALLOWED MAC_OS_X_VERSION_10_4
#endif
#import "CSCoroutine.h"
#include <pthread.h>
#include <objc/runtime.h>
#include <objc/message.h>

@interface NSProxy (Hidden)
- (void)doesNotRecognizeSelector:(SEL)sel;
@end

static pthread_key_t currkey, mainkey;

static void CSCoroutineFreeMain(void *raw_main)
{
    CSCoroutine* main = (CSCoroutine *)raw_main;
	[main release];
}

static CSCoroutine *CSMainCoroutine()
{
	CSCoroutine *main = (CSCoroutine *)pthread_getspecific(mainkey);
	if (!main) {
		main = [[CSCoroutine alloc] initWithTarget:nil stackSize:0];
		pthread_setspecific(mainkey, main);
	}
	return main;
}

static CSCoroutine *CSCurrentCoroutine()
{
	CSCoroutine *curr = (CSCoroutine *)pthread_getspecific(currkey);
	if (curr) {
		return curr;
	} else {
		return CSMainCoroutine();
	}
}

static CSCoroutine *CSSetCurrentCoroutine(CSCoroutine *new)
{
	CSCoroutine *curr = CSCurrentCoroutine();
	pthread_setspecific(currkey, new);
	return curr;
}

@implementation CSCoroutine

+ (void)initialize
{
	pthread_key_create(&currkey, NULL);
	pthread_key_create(&mainkey, CSCoroutineFreeMain);
}

+ (CSCoroutine *)mainCoroutine
{
	return CSMainCoroutine();
}

+ (CSCoroutine *)currentCoroutine
{
	return CSCurrentCoroutine();
}

+ (void)setCurrentCoroutine:(CSCoroutine *)curr
{
	CSSetCurrentCoroutine(curr);
}

+ (void)returnFromCurrent
{
	[CSCurrentCoroutine() returnFrom];
}

- (id)initWithTarget:(id)targetobj stackSize:(size_t)stackbytes
{
	target = targetobj;
	stacksize = stackbytes;
	if (stacksize > 0) {
		stack = malloc(stacksize);
    } else {
        stack = NULL;
    }
	fired = target ? NO : YES;

	caller = nil;
	inv = nil;

	return self;
}

- (void)dealloc
{
	free(stack);
	[inv release];
	[super dealloc];
}

- (void)switchTo
{
	CSCoroutine *curr = CSSetCurrentCoroutine(self);
	caller = curr;
    swapcontext(&caller->ctx, &ctx);
}

- (void)returnFrom
{
	CSSetCurrentCoroutine(caller);
    swapcontext(&ctx, &caller->ctx);
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)sel
{
	return [target methodSignatureForSelector:sel];
}

static void CSLeopardCoroutineStart()
{
	CSCoroutine *coro = CSCurrentCoroutine();
	[coro->inv invoke];
	[coro returnFrom];
	[NSException raise:@"CSCoroutineException" format:@"Attempted to switch to a coroutine that has ended"];
}

- (void)forwardInvocation:(NSInvocation *)invocation
{
	if (fired) {
		[NSException raise:@"CSCoroutineException" format:@"Attempted to start a coroutine that is already running"];
	}
	fired = YES;

	inv = [invocation retain];
	[inv setTarget:target];

    getcontext(&ctx);
    ctx.uc_stack.ss_sp = (void*)((uint64_t)(stack + stacksize) & ~15);
    ctx.uc_stack.ss_size = stacksize - 16;
    ctx.uc_stack.ss_flags = 0;
    makecontext(&ctx, CSLeopardCoroutineStart, 0);
    
	[self switchTo];
}

@end

@implementation NSObject (CSCoroutine)

- (CSCoroutine *)newCoroutine
{
    size_t kDefaultStackSize = 1024 * 1024;
    return [self newCoroutineWithStackSize: kDefaultStackSize];
}

- (CSCoroutine *)newCoroutineWithStackSize:(size_t)stacksize
{
	return [[CSCoroutine alloc] initWithTarget:self stackSize:stacksize];
}

@end
