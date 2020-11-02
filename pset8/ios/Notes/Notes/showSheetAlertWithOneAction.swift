//
//  confirmationDialogueForDelete.swift
//  Notes
//
//  Created by Ping Lu on 02.11.20.
//

import UIKit

func showSheetAlertWithOneAction(messageText: String, actionButtonText: String, dismissButtonText: String, caller: UIViewController, actionButtonClosure: @escaping () -> Void) {
    // Declare alert message
    let dialogueMessage = UIAlertController(title: nil, message: messageText, preferredStyle: .actionSheet)
    
    // Create action button with action handler
    let actionButton = UIAlertAction(title: actionButtonText, style: .destructive, handler: { _ in
        actionButtonClosure()
    })
    
    // Create dismiss button
    let dismissButton = UIAlertAction(title: dismissButtonText, style: .cancel, handler: nil)
    
    // Add action and dismiss button to dialogue message
    dialogueMessage.addAction(actionButton)
    dialogueMessage.addAction(dismissButton)
    
    // Present dialogue message to user
    caller.present(dialogueMessage, animated: true, completion: nil)
}
