//
//  PPErrors.h
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#ifndef __PLAYERPROKIT_PPERRORS_H__
#define __PLAYERPROKIT_PPERRORS_H__

#import <Foundation/Foundation.h>
#include <PlayerPROCore/MADDefs.h>

__BEGIN_DECLS

//! The error domain of \c MADErr errors in <code>NSError</code>s.
extern NSString * __nonnull const PPMADErrorDomain;

/*!
 *	@function	PPCreateErrorFromMADErrorType
 *	@abstract	Create an \c NSError object encapulating a PlayerPROCore's \c MADErr
 *	@param		theErr
 *				The error to encapsulate
 *	@return		A retained \c NSError object, or \c nil if passed \c MADNoErr .
 *	@discussion	Creates an \c NSError object in the \c PPMADErrorDomain with \c NSLocalizedDescriptionKey,
 *				\c NSLocalizedFailureReasonErrorKey, and \c NSLocalizedRecoverySuggestionErrorKey set.<br>
 *				If the value in \c theErr isn't a value in \c MADErr, it returns an \c NSError in the
 *				\c NSOSStatusErrorDomain with nothing in the \c userInfo dictionary.
 */
extern NSError* __nullable PPCreateErrorFromMADErrorType(MADErr theErr) NS_RETURNS_RETAINED;

/*!
 *	@function	PPCreateErrorFromMADErrorTypeConvertingToCocoa
 *	@abstract	Create an \c NSError object from a PlayerPROCore's \c MADErr
 *	@param		theErr
 *				The error to encapsulate or convert
 *	@param		convertToCocoa
 *				If <code>YES</code>, attempts to convert the error value into an error
 *				value native to Cocoa.
 *	@return		A retained \c NSError object, or \c nil if passed \c MADNoErr .
 *	@discussion	Attempts to create an \c NSError object in an error domain that is native to the
 *				Foundation framework, such as \c NSPOSIXErrorDomain. If there isn't an equivalent,
 *				creates an \c NSError object in the \c PPMADErrorDomain with \c NSLocalizedDescriptionKey,
 *				\c NSLocalizedFailureReasonErrorKey, and \c NSLocalizedRecoverySuggestionErrorKey set.<br>
 *				If the value in \c theErr isn't a value in \c MADErr, it returns an \c NSError in the
 *				\c NSOSStatusErrorDomain with nothing in the \c userInfo dictionary.
 */
extern NSError* __nullable PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADErr theErr, BOOL convertToCocoa) NS_RETURNS_RETAINED;

//! Checks if the error sent to it is a user cancelled error.
//! This checks for \c MADUserCanceledErr in the \c PPMADErrorDomain and
//! \c NSUserCancelledError in the <code>NSCocoaErrorDomain</code>.
extern BOOL PPErrorIsUserCancelled(NSError * __nonnull theErr);

__END_DECLS

#endif
