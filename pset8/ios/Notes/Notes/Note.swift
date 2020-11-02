//
//  Note.swift
//  Notes
//
//  Created by Ping Lu on 01.11.20.
//

import Foundation
import SQLite3

struct Note {
    let id: Int32
    var content: String
}

// Work with databse
class NoteManager {
    // Pointer to our database file
    var database: OpaquePointer?
    
    // Since connecting to a database can be kind of expensive, we want to save the connection to the database and don't want to have multiple instance of NoteManager. Therefore we create an instance of NoteManager inside of itself. This is something called singelton.
    static let shared = NoteManager()
    // Prevent anyone from instantiating the class
    private init() {
    }
    
    func connectDatabase() {
        // Ensure database is only connected once
        if database != nil {
            return
        }
        
        do {
            //  Get a path to a file on user's phone, open or create a file called notes.sqlitea
            let databaseURL = try FileManager.default.url(
                for: .documentDirectory,
                in: .userDomainMask,
                appropriateFor: nil,
                create: false
            ).appendingPathComponent("notes.sqlite")
            
            // Open the database and store the connection to database to the pointer, ensure the open is successful
            if sqlite3_open(databaseURL.path, &database) != SQLITE_OK {
                let errMsg: String = String(cString: sqlite3_errmsg(database))
                print("Error opening databse: \(errMsg)")
                return
            }
            
            // Create table notes if not exists and ensure it is successful
            if sqlite3_exec(database,
                            "CREATE TABLE IF NOT EXISTS notes (content TEXT)",
                            nil,
                            nil,
                            nil
            ) != SQLITE_OK {
                let errMsg: String = String(cString: sqlite3_errmsg(database))
                print("Error creating table: \(errMsg)")
                return
            }
            
        } catch let error {
            print("Error creating database: \(error)")
        }
    }
    
    func createNote() -> Int {
        connectDatabase()
        
        var statement: OpaquePointer? = nil
        
        // Prepare insert statement and ensure it is successful
        if sqlite3_prepare_v2(database,
                              "INSERT INTO notes (content) VALUES ('New note')",
                              -1,
                              &statement,
                              nil
        ) != SQLITE_OK {
            let errMsg: String = String(cString: sqlite3_errmsg(database))
            print("Error creating note insert statement: \(errMsg)")
            return -1
        }
        
        // Execute insert statement and ensure it is successful
        if sqlite3_step(statement) != SQLITE_DONE {
            let errMsg: String = String(cString: sqlite3_errmsg(database))
            print("Error inserting note: \(errMsg)")
            return -1
        }
        
        // Delete the prepared statement
        sqlite3_finalize(statement)
        
        // Return the inserted row's id
        return Int(sqlite3_last_insert_rowid(database))
    }
    
    func getNotes() -> [Note] {
        connectDatabase()
        
        var result: [Note] = []
        var statement: OpaquePointer? = nil
        
        // Prepare select statement and ensure it is successful
        if sqlite3_prepare_v2(database,
                              "SELECT rowid, content FROM notes",
                              -1,
                              &statement,
                              nil
        ) != SQLITE_OK {
            let errMsg: String = String(cString: sqlite3_errmsg(database))
            print("Error creating note select statement: \(errMsg)")
            return []
        }
        
        // Execute select statement until there are no more rows
        while sqlite3_step(statement) == SQLITE_ROW {
            // Get note's id and note's content from the last row from the statement
            let noteId: Int32 = sqlite3_column_int(statement, 0)
            let noteContent: String = String(cString: sqlite3_column_text(statement, 1))
            // ...and append them to the result array
            result.append(Note(id: noteId, content: noteContent))
        }
        
        // Delete the prepared statement
        sqlite3_finalize(statement)
        
        return result
    }
    
    func saveNote(note: Note) {
        connectDatabase()
        
        var statement: OpaquePointer? = nil
        
        // Prepare update statement and ensure it is successful
        if sqlite3_prepare_v2(database,
                              "UPDATE notes SET content = ? WHERE rowid = ?",
                              -1,
                              &statement,
                              nil
        ) != SQLITE_OK {
            let errMsg: String = String(cString: sqlite3_errmsg(database))
            print("Error creating note update statement: \(errMsg)")
            return
        }
        
        // Bind data to the placeholders in the update statement
        let noteContent: NSString = NSString(string: note.content)
        let noteId: Int32 = note.id
        sqlite3_bind_text(statement, 1, noteContent.utf8String, -1, nil)
        sqlite3_bind_int(statement, 2, noteId)
        
        // Execute update statment and ensure it is successful
        if sqlite3_step(statement) != SQLITE_DONE {
            let errMsg: String = String(cString: sqlite3_errmsg(database))
            print("Error saving note: \(errMsg)")
            return
        }
        
        // Delete the prepared statement
        sqlite3_finalize(statement)
    }
    
    func deleteNote(note: Note) {
        connectDatabase()
        
        var statement: OpaquePointer? = nil
        
        // Prepare delete statement and ensure it is successful
        if sqlite3_prepare_v2(database,
                              "DELETE FROM notes WHERE rowid = ?",
                              -1,
                              &statement,
                              nil
        ) != SQLITE_OK {
            let errMsg: String = String(cString: sqlite3_errmsg(database))
            print("Error creating delete note statement: \(errMsg)")
            return
        }
        
        // Bind data to placeholder in the statement
        let noteId: Int32 = note.id
        sqlite3_bind_int(statement, 1, noteId)
        
        // Execute delete statement and ensure it is successful
        if sqlite3_step(statement) != SQLITE_DONE {
            let errMsg: String = String(cString: sqlite3_errmsg(database))
            print("Error deleting note: \(errMsg)")
            return
        }
        
        // Delete the prepared statement
        sqlite3_finalize(statement)
    }
}
