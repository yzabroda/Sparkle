//
//  SPUUserDriver.h
//  Sparkle
//
//  Created by Mayur Pawashe on 2/14/16.
//  Copyright © 2016 Sparkle Project. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SPUStatusCompletionResults.h"
#import "SUExport.h"

NS_ASSUME_NONNULL_BEGIN

@class SPUUpdatePermissionRequest, SPUUpdatePermissionResponse, SUAppcastItem, SPUDownloadData;

/*!
 The API in Sparkle for controlling the user interaction.
 
 This protocol is used for implementing a user interface for the Sparkle updater. Sparkle's internal drivers tell
 an object that implements this protocol what actions to take and show to the user.
 
 Every method in this protocol is required (i.e, not optional) and has a void return type and can optionally take a single parameter block, which waits for
 a response back from the user driver. Note that every parameter block, or reply, *must* be responded to eventually -
 that is, none can be ignored. Furthermore, they can only be replied to *once* - a reply or completion block should be considered
 invalidated after it's once used. The faster a reply can be made, the more Sparkle may be able to idle, and so the better.
 Lastly, every method in this protocol can be called from any thread. Thus, an implementor may choose to always
 dispatch asynchronously to the main thread. However, an implementor should also avoid unnecessary nested asynchronous dispatches.
 
 An implementor of this protocol should act defensively. For example, it may be possible for an action that says to
 invalidate or dismiss something to be called multiple times in succession, and the implementor may choose to ignore further requests.
 */
SU_EXPORT @protocol SPUUserDriver <NSObject>

/*!
 * Show that an update can be checked by the user or not
 *
 * A client may choose to update the interface letting the user know if they can check for updates.
 * For example, this can be used for menu item validation on the "Check for Updates" action.
 *
 * This can be called from any thread.
 */
- (void)showCanCheckForUpdates:(BOOL)canCheckForUpdates;

/*!
 * Show an updater permission request to the user
 *
 * Ask the user for their permission regarding update checks.
 * This is typically only called once per app installation.
 *
 * @param request The update permission request.
 * @param reply A reply with a update permission response.
 *
 * This can be called from any thread
 */
- (void)showUpdatePermissionRequest:(SPUUpdatePermissionRequest *)request reply:(void (^)(SPUUpdatePermissionResponse *))reply;

/*!
 * Show the user initating an update check
 *
 * Respond to the user initiating an update check. Sparkle uses this to show the user a window with an indeterminate progress bar.
 *
 * @param updateCheckStatusCompletion A reply indicating whether the initiated update check is done or canceled.
 * Attempts to canceling can be made before -dismissUserInitiatedUpdateCheck is invoked. Replying with SUUserInitiatedCheckDone
 * on the other hand should not be done until -dismissUserInitiatedUpdateCheck is invoked.
 *
 * This can be called from any thread
 */
- (void)showUserInitiatedUpdateCheckWithCompletion:(void (^)(SPUUserInitiatedCheckStatus))updateCheckStatusCompletion;

/*!
 * Dismiss the user initiated update check from the user
 *
 * Dismiss whatever was started in -showUserInitiatedUpdateCheckWithCompletion:
 * This is an appropriate time to reply with SUUserInitiatedCheckDone if not having done so already
 *
 * This can be called from any thread
 */
- (void)dismissUserInitiatedUpdateCheck;

/*!
 * Show the user a new update is found and can be downloaded & installed
 *
 * Let the user know a new downloadable update is found and ask them what they want to do.
 *
 * @param appcastItem The Appcast Item containing information that reflects the new update
 *
 * @param reply
 * A reply of SUInstallUpdateChoice begins downloading and installing the new update.
 *
 * A reply of SUInstallLaterChoice reminds the user later of the update, which can act as a "do nothing" option.
 *
 * A reply of SUSkipThisVersionChoice skips this particular version and won't bother the user again,
 * unless they initiate an update check themselves.
 *
 * This can be called from any thread
 */
- (void)showUpdateFoundWithAppcastItem:(SUAppcastItem *)appcastItem reply:(void (^)(SPUUpdateAlertChoice))reply;

/*!
 * Show the user a new update has been downloaded and can be installed
 *
 * This method behaves just like -showUpdateFoundWithAppcastItem:reply: except the update has already been downloaded.
 */
- (void)showDownloadedUpdateFoundWithAppcastItem:(SUAppcastItem *)appcastItem reply:(void (^)(SPUUpdateAlertChoice))reply;

/*!
 * Show the user an update that has started installing can be resumed and installed immediately
 *
 * Let the user know an update that has already been downloaded and started installing can be resumed.
 * Note at this point the update cannot be canceled.
 *
 * @param appcastItem The Appcast Item containing information that reflects the new update
 *
 * @param reply A reply of SUInstallAndRelaunchUpdateNow installs the update immediately and relaunches the new update.
 * A reply of SUInstallUpdateNow installs the update immediately but does not relaunch the new update.
 * A reply of SUDismissUpdateInstallation dismisses the update installation. Note the update will attempt to finish installation
 * after the application terminates.
 *
 * This can be called from any thread
 */
- (void)showResumableUpdateFoundWithAppcastItem:(SUAppcastItem *)appcastItem reply:(void (^)(SPUInstallUpdateStatus))reply;

/*!
 * Show the user the release notes for the new update
 *
 * Display the release notes to the user. This will be called after showing the new update.
 * This is only applicable if the release notes are linked from the appcast, and are not directly embedded inside of the appcast file.
 * That is, this may be invoked if the releaseNotesURL from the appcast item is non-nil.
 *
 * @param downloadData The data for the release notes that was downloaded from the new update's appcast.
 *
 * This can be called from any thread
 */
- (void)showUpdateReleaseNotesWithDownloadData:(SPUDownloadData *)downloadData;

/*!
 * Show the user that the new update's release notes could not be downloaded
 *
 * This will be called after showing the new update.
 * This is only applicable if the release notes are linked from the appcast, and are not directly embedded inside of the appcast file.
 * That is, this may be invoked if the releaseNotesURL from the appcast item is non-nil.
 *
 * @param error The error associated with why the new update's release notes could not be downloaded.
 *
 * This can be called from any thread
 */
- (void)showUpdateReleaseNotesFailedToDownloadWithError:(NSError *)error;

/*!
 * Show the user a new update was not found
 *
 * Let the user know a new update was not found after they tried initiating an update check.
 *
 * @param acknowledgement Acknowledge to the updater that no update found was shown.
 *
 * This can be called from any thread
 */
- (void)showUpdateNotFoundWithAcknowledgement:(void (^)(void))acknowledgement;

/*!
 * Show the user an update error occurred
 *
 * Let the user know that the updater failed with an error. This will not be invoked without the user having been
 * aware that an update was in progress.
 *
 * @param acknowledgement Acknowledge to the updater that the error was shown.
 *
 * This can be called from any thread
 */
- (void)showUpdaterError:(NSError *)error acknowledgement:(void (^)(void))acknowledgement;

/*!
 * Show the user that downloading the new update initiated
 *
 * Let the user know that downloading the new update started.
 *
 * @param downloadUpdateStatusCompletion A reply of SUDownloadUpdateCanceled can be used to cancel
 * the download at any point before -showDownloadDidStartExtractingUpdate is invoked.
 * A reply of SUDownloadUpdateDone signifies that the download is done, which should not be invoked until
 * -showDownloadDidStartExtractingUpdate
 *
 * This can be called from any thread
 */
- (void)showDownloadInitiatedWithCompletion:(void (^)(SPUDownloadUpdateStatus))downloadUpdateStatusCompletion;

/*!
 * Show the user the content length of the new update that will be downloaded
 *
 * @param expectedContentLength The expected content length of the new update being downloaded. This will be greater than 0.
 *
 * This can be called from any thread
 */
- (void)showDownloadDidReceiveExpectedContentLength:(NSUInteger)expectedContentLength;

/*!
 * Show the user that the update download received more data
 *
 * This may be an appropriate time to advance a visible progress indicator of the download
 * @param length The length of the data that was just downloaded
 *
 * This can be called from any thread
 */
- (void)showDownloadDidReceiveDataOfLength:(NSUInteger)length;

/*!
 * Show the user that the update finished downloading and started extracting
 *
 * This is an appropriate time to reply with SUDownloadUpdateDone if not done so already
 * Sparkle uses this to show an indeterminate progress bar.
 *
 * Note that an update can resume at this point after having been downloaded before,
 * so this may be called without any of the download callbacks being invoked prior.
 *
 * This can be called from any thread
 */
- (void)showDownloadDidStartExtractingUpdate;

/*!
 * Show the user that the update is extracting with progress
 *
 * Let the user know how far along the update extraction is.
 *
 * @param progress The progress of the extraction from a 0.0 to 1.0 scale
 *
 * This can be called from any thread
 */
- (void)showExtractionReceivedProgress:(double)progress;

/*!
 * Show the user that the update is ready to install
 *
 * Let the user know that the update is ready and ask them whether they want to install or not.
 * Note if the target application is already terminated and an update can be performed silently, this method may not be invoked.
 *
 * @param installUpdateHandler A reply of SUInstallAndRelaunchUpdateNow installs the update immediately and relaunches the new update.
 * A reply of SUInstallUpdateNow installes the update immediately but does not relaunch the new update.
 * A reply of SUDismissUpdateInstallation dismisses the update installation. Note the update may still be installed after
 * the application terminates, however there is not a strong guarantee that this will happen.
 *
 * This can be called from any thread
 */
- (void)showReadyToInstallAndRelaunch:(void (^)(SPUInstallUpdateStatus))installUpdateHandler;

/*!
 * Show the user that the update is installing
 *
 * Let the user know that the update is currently installing. Sparkle uses this to show an indeterminate progress bar.
 *
 * This can be called from any thread
 */
- (void)showInstallingUpdate;

/*!
 * Terminate the application and show or dismiss installer progress.
 *
 * Sparkle is asking us to send a request to terminate the application.
 *
 * If this will terminate the current application, an implementor may decide to dismiss progress UI in case the termination request is delayed or canceled.
 * Or if this will terminate a remote application, an implementor may decide to show the user that the update is currently installing.
 *
 * This can be called from any thread
 */
- (void)terminateApplication;

/*!
 * Terminate the application silently.
 *
 * Sparkle is asking us to send a request to terminate the application without showing any UI interaction.
 * This may be invoked after Sparkle has downloaded an update in the background automatically,
 * and the updater's delegate has decided to install & relaunch the application without disrupting the user (think of a backgrounded application).
 *
 * After the application is terminated, the update will be installed and the application will be relaunched.
 *
 * This can be called from any thread
 */
- (void)terminateApplicationSilently;

/*!
 * Show the user that the update installation finished
 *
 * Let the user know that the update finished installing.
 * This will only be invoked if the updater process is still alive, which is typically not the case if
 * the updater's lifetime is tied to the application it is updating.
 *
 * @param acknowledgement Acknowledge to the updater that the installation finish was shown.
 *
 * This can be called from any thread
 */
- (void)showUpdateInstallationDidFinishWithAcknowledgement:(void (^)(void))acknowledgement;

/*!
 * Dismiss the current update installation
 *
 * Stop and tear down everything. Reply to all outstanding reply/completion blocks.
 * Dismiss all update windows, alerts, progress, etc from the user.
 * Unregister for application termination and system power off if not done so already.
 * Basically, stop everything that could have been started. Sparkle may invoke this when aborting or finishing an update.
 * Do not, however, invaldate the next update check timer.
 *
 * This can be called from any thread, and could be called multiple times in succession.
 */
- (void)dismissUpdateInstallation;

@end

NS_ASSUME_NONNULL_END
