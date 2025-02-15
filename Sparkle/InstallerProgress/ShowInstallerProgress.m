//
//  ShowInstallerProgress.m
//  Installer Progress
//
//  Created by Mayur Pawashe on 4/7/16.
//  Copyright © 2016 Sparkle Project. All rights reserved.
//

#import <AppKit/AppKit.h>
#import "ShowInstallerProgress.h"
#import "SUStatusController.h"
#import "SUHost.h"
#import "SULocalizations.h"

@interface ShowInstallerProgress ()

@property (nonatomic) SUStatusController *statusController;

@end

@implementation ShowInstallerProgress
{
    NSString *_updatingString;
    NSString *_cancelUpdateTitle;
    NSString *_installingUpdateTitle;
}

@synthesize statusController = _statusController;

- (void)loadLocalizationStringsFromHost:(SUHost *)host
{
    // Try to retrieve localization strings from the old bundle if possible
    // We won't display these strings until installerProgressShouldDisplayWithHost:
    // (which will be after the update is trusted)
    // If we fail to load localizations in any way, we default to English
    
    NSBundle *hostSparkleBundle;
    {
        NSURL *hostSparkleURL = [host.bundle.privateFrameworksURL URLByAppendingPathComponent:@"Sparkle.framework" isDirectory:YES];
        if (hostSparkleURL == nil) {
            hostSparkleBundle = nil;
        } else {
            hostSparkleBundle = [NSBundle bundleWithURL:hostSparkleURL];
        }
    }
    
    NSString *updatingString;
    {
        NSString *updatingFormatStringFromBundle = (hostSparkleBundle != nil) ? SULocalizedStringFromTableInBundle(@"Updating %@", @"Sparkle", hostSparkleBundle, nil) : nil;

        NSString *hostNameFromBundle = host.name;
        NSString *hostName = (hostNameFromBundle != nil) ? hostNameFromBundle : @"";
        
        if (updatingFormatStringFromBundle != nil) {
            // Replacing the %@ will be a bit safer than using +[NSString stringWithFormat:]
            updatingString = [updatingFormatStringFromBundle stringByReplacingOccurrencesOfString:@"%@" withString:hostName];
        } else {
            updatingString = [@"Updating " stringByAppendingString:hostName];
        }
    }
    
    _updatingString = updatingString;
    
    NSString *cancelUpdateTitle;
    {
        NSString *cancelUpdateTitleFromBundle = (hostSparkleBundle != nil) ?  SULocalizedStringFromTableInBundle(@"Cancel Update", @"Sparkle", hostSparkleBundle, @"") : nil;
        cancelUpdateTitle = (cancelUpdateTitleFromBundle != nil) ? cancelUpdateTitleFromBundle : @"Cancel Update";
    }
    
    _cancelUpdateTitle = cancelUpdateTitle;
    
    NSString *installingUpdateTitle;
    {
        NSString *installingUpdateTitleFromBundle = (hostSparkleBundle != nil) ?  SULocalizedStringFromTableInBundle(@"Installing update…", @"Sparkle", hostSparkleBundle, @"") : nil;
        installingUpdateTitle = (installingUpdateTitleFromBundle != nil) ? installingUpdateTitleFromBundle : @"Installing update…";
    }
    
    _installingUpdateTitle = installingUpdateTitle;
}

- (void)installerProgressShouldDisplayWithHost:(SUHost *)host
{
    self.statusController = [[SUStatusController alloc] initWithHost:host windowTitle:_updatingString centerPointValue:nil minimizable:NO closable:NO];
    
    [self.statusController setButtonTitle:_cancelUpdateTitle target:nil action:nil isDefault:NO];
    [self.statusController beginActionWithTitle:_installingUpdateTitle maxProgressValue:0 statusText:@""];
    
    [self.statusController showWindow:self];
}

- (void)installerProgressShouldStop
{
    [self.statusController close];
    self.statusController = nil;
}

@end
