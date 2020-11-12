//
//  NoteViewController.swift
//  Notes
//
//  Created by Ping Lu on 01.11.20.
//

import UIKit

class NoteViewController: UIViewController {
    var note: Note?
    
    @IBOutlet var contentTextView: UITextView!
    
    // Handle note deletion
    @IBAction func deleteButton(_ sender: UIBarItem) {
        // Show delete confirmation dialogue and pass function deleteNote as action button's closure
        showSheetAlertWithOneAction(
            messageText: "This note will be deleted. This action cannot be undone.",
            actionButtonText: "Delete Note",
            dismissButtonText: "Cancel",
            caller: self
        ) {
            self.deleteNote()
        }
    }
    
    func deleteNote() {
        // Delete note from database
        NoteManager.shared.deleteNote(note: note!)
        // ...and switch back to note list view
        navigationController?.popViewController(animated: true)
    }
    
    // Prepare contentTextView;s text when view is about to appear
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(true)
        
        contentTextView.text = note!.content
    }
    
    // Save note when user hits Back button
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(true)
        
        // Update note's content with current contentTextView's text
        note!.content = contentTextView.text
        NoteManager.shared.saveNote(note: note!)
    }
}
