/*
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#import <Foundation/Foundation.h>

/*!
 Error domain used if there was multiple errors on <AWSTask taskForCompletionOfAllTasks:>.
 */
extern NSString *const AWSTaskErrorDomain;

/*!
 An exception that is thrown if there was multiple exceptions on <AWSTask taskForCompletionOfAllTasks:>.
 */
extern NSString *const AWSTaskMultipleExceptionsException;

@class AWSExecutor;
@class AWSTask;

/*!
 A block that can act as a continuation for a task.
 */
typedef id(^AWSContinuationBlock)(AWSTask *task);

/*!
 The consumer view of a Task. A AWSTask has methods to
 inspect the state of the task, and to add continuations to
 be run once the task is complete.
 */
@interface AWSTask : NSObject
{
@private
	BOOL _cancelled;
	BOOL _faulted;
	BOOL _completed;
}

/*!
 Creates a task that is already completed with the given result.
 @param result The result for the task.
 */
+ (instancetype)taskWithResult:(id)result;

/*!
 Creates a task that is already completed with the given error.
 @param error The error for the task.
 */
+ (instancetype)taskWithError:(NSError *)error;

/*!
 Creates a task that is already completed with the given exception.
 @param exception The exception for the task.
 */
+ (instancetype)taskWithException:(NSException *)exception;

/*!
 Creates a task that is already cancelled.
 */
+ (instancetype)cancelledTask;

/*!
 Returns a task that will be completed (with result == nil) once
 all of the input tasks have completed.
 @param tasks An `NSArray` of the tasks to use as an input.
 */
+ (instancetype)taskForCompletionOfAllTasks:(NSArray *)tasks;

/*!
 Returns a task that will be completed once all of the input tasks have completed.
 If all tasks complete successfully without being faulted or cancelled the result will be
 an `NSArray` of all task results in the order they were provided.
 @param tasks An `NSArray` of the tasks to use as an input.
 */
+ (instancetype)taskForCompletionOfAllTasksWithResults:(NSArray *)tasks;

/*!
 Returns a task that will be completed a certain amount of time in the future.
 @param millis The approximate number of milliseconds to wait before the
 task will be finished (with result == nil).
 */
+ (instancetype)taskWithDelay:(int)millis;

/*!
 Returns a task that will be completed after the given block completes with
 the specified executor.
 @param executor A BFExecutor responsible for determining how the
 continuation block will be run.
 @param block The block to immediately schedule to run with the given executor.
 @returns A task that will be completed after block has run.
 If block returns a AWSTask, then the task returned from
 this method will not be completed until that task is completed.
 */
+ (instancetype)taskFromExecutor:(AWSExecutor *)executor
                       withBlock:(id (^)(void))block;

// Properties that will be set on the task once it is completed.

/*!
 The result of a successful task.
 */
@property (nonatomic, strong, readonly) id result;


/*!
 The error of a failed task.
 */
@property (nonatomic, strong, readonly) NSError *error;

/*!
 The exception of a failed task.
 */
@property (nonatomic, strong, readonly) NSException *exception;

/*!
 Whether this task has been cancelled.
 */
@property (assign, readonly, getter = isCancelled) BOOL cancelled;

/*!
 Whether this task has completed due to an error or exception.
 */
@property (assign, readonly, getter = isFaulted) BOOL faulted;

/*!
 Whether this task has completed.
 */
@property (assign, readonly, getter = isCompleted) BOOL completed;

/*!
 Enqueues the given block to be run once this task is complete.
 This method uses a default execution strategy. The block will be
 run on the thread where the previous task completes, unless the
 the stack depth is too deep, in which case it will be run on a
 dispatch queue with default priority.
 @param block The block to be run once this task is complete.
 @returns A task that will be completed after block has run.
 If block returns a AWSTask, then the task returned from
 this method will not be completed until that task is completed.
 */
- (instancetype)continueWithBlock:(AWSContinuationBlock)block;

/*!
 Enqueues the given block to be run once this task is complete.
 @param executor A BFExecutor responsible for determining how the
 continuation block will be run.
 @param block The block to be run once this task is complete.
 @returns A task that will be completed after block has run.
 If block returns a AWSTask, then the task returned from
 this method will not be completed until that task is completed.
 */
- (instancetype)continueWithExecutor:(AWSExecutor *)executor
                           withBlock:(AWSContinuationBlock)block;

/*!
 Identical to continueWithBlock:, except that the block is only run
 if this task did not produce a cancellation, error, or exception.
 If it did, then the failure will be propagated to the returned
 task.
 @param block The block to be run once this task is complete.
 @returns A task that will be completed after block has run.
 If block returns a AWSTask, then the task returned from
 this method will not be completed until that task is completed.
 */
- (instancetype)continueWithSuccessBlock:(AWSContinuationBlock)block;

/*!
 Identical to continueWithExecutor:withBlock:, except that the block
 is only run if this task did not produce a cancellation, error, or
 exception. If it did, then the failure will be propagated to the
 returned task.
 @param executor A BFExecutor responsible for determining how the
 continuation block will be run.
 @param block The block to be run once this task is complete.
 @returns A task that will be completed after block has run.
 If block returns a AWSTask, then the task returned from
 this method will not be completed until that task is completed.
 */
- (instancetype)continueWithExecutor:(AWSExecutor *)executor
                    withSuccessBlock:(AWSContinuationBlock)block;

/*!
 Waits until this operation is completed.
 This method is inefficient and consumes a thread resource while
 it's running. It should be avoided. This method logs a warning
 message if it is used on the main thread.
 */
- (void)waitUntilFinished;

@end
