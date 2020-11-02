//
//  NoteListViewController.swift
//  Notes
//
//  Created by Ping Lu on 01.11.20.
//

import UIKit

class NoteListViewController: UITableViewController {
    var notes: [Note] = []
    
    // Handle note creation
    @IBAction func createNote() {
        _ = NoteManager.shared.createNote()
        reload()
    }
    
    // Reload table view with new data
    func reload() {
        // Get all notes from database
        notes = NoteManager.shared.getNotes()
        // ...and reload table view
        tableView.reloadData()
    }
    
    // Load notes from database into table view when view is about to appear
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(true)
        reload()
    }
    
    // Define number of section
    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }
    
    // Define number of rows
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return notes.count
    }
    
    // Define cell
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        // Get a cell from the cell pool
        let cell = tableView.dequeueReusableCell(withIdentifier: "NoteCell", for: indexPath)
        // Set cell's text to corresponding note's content
        cell.textLabel?.text = notes[indexPath.row].content
        
        return cell
    }
    
    // Swipe to delete action with "trash" icon
    override func tableView(_ tableView: UITableView, trailingSwipeActionsConfigurationForRowAt indexPath: IndexPath) -> UISwipeActionsConfiguration? {
        let deleteAction = UIContextualAction(style: .destructive, title: nil, handler: {(_, _, completionHandler) in
            // Show delete confirmation dialogue; pass function deleteNote as action button's closure
            showSheetAlertWithOneAction(
                messageText: "This note will be deleted. This action cannot be undone.",
                actionButtonText: "Delete Note",
                dismissButtonText: "Cancel",
                caller: self
            ) {
                self.deleteNote(indexPath: indexPath)
            }
            
            completionHandler(true)
        })
        
        deleteAction.image = UIImage(systemName: "trash")
        deleteAction.backgroundColor = .systemRed
        let configuration = UISwipeActionsConfiguration(actions: [deleteAction])
        return configuration
    }
    
    func deleteNote(indexPath: IndexPath) {
        // Delete note from database
        let note: Note = notes[indexPath.row]
        NoteManager.shared.deleteNote(note: note)
        // Update notes array by getting all notes from database
        notes = NoteManager.shared.getNotes()
        // Delete note in table view
        tableView.deleteRows(at: [indexPath], with: .fade)
    }
    
    // Prepare for segue
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Ensure segue is NoteSegue
        if segue.identifier == "NoteSegue",
           // Ensure segue detination is NoteViewController
           let destination = segue.destination as? NoteViewController,
           // Get selected row's index
           let index = tableView.indexPathForSelectedRow?.row
        {
            // Pass corresponding note to destination
            destination.note = notes[index]
        }
    }
}
