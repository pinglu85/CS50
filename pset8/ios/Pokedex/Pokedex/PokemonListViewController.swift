//
//  PokemonListViewController.swift
//  Pokedex
//
//  Created by Ping Lu on 27.10.20.
//

import UIKit

class PokemonListViewController: UITableViewController, UISearchBarDelegate {
    var pokemon: [PokemonListResult] = []
    var searchResult: [PokemonListResult] = []
    // Track search acitivity
    var isSearching: Bool = false
    
    @IBOutlet var searchBar: UISearchBar!
    
    
    // Uppercase the first letter in text
    func capitalize(text: String) -> String {
        return text.prefix(1).uppercased() + text.dropFirst()
    }
    
    // Handle pokemon search
    func searchBar(_ searchBar: UISearchBar, textDidChange searchText: String) {
        // Search activity begins
        isSearching = true
        
        // An empty search should match everthing
        if searchText == "" {
            searchResult = pokemon
        }
        else {
            // Empty search result array
            searchResult = []
            // Iterate over pokemon array
            for p in pokemon {
                // If search text matchs any substring in pokemon's name
                if p.name.contains(searchText.lowercased()) {
                    // ...append the pokemon to search result array
                    searchResult.append(p)
                }
            }
        }
        // ...reload table view with new data
        tableView.reloadData()
    }
    
    // Handle search cancel
    func searchBarCancelButtonClicked(_ searchBar: UISearchBar) {
        // Search acitivity ends
        isSearching = false
        tableView.reloadData()
    }
    
    // When view loads
    override func viewDidLoad() {
        super.viewDidLoad()
        searchBar.delegate = self
        
        guard let url = URL(string: "https://pokeapi.co/api/v2/pokemon?limit=151") else {
            return
        }
        
        // Fetch pokemon list from API
        URLSession.shared.dataTask(with: url) { (data, response, error) in
            // Ensure data is not nil
            guard let data = data else {
                return
            }
            
            do {
                // Parse response data
                let entries = try JSONDecoder().decode(PokemonListResults.self, from: data)
                // Set pokemon array to response result
                self.pokemon = entries.results
                
                // Back to the main thread
                DispatchQueue.main.async {
                    // ...reload table view with new data
                    self.tableView.reloadData()
                }
            }
            catch let error {
                print(error)
            }
        }.resume()
    }
    
    // Set number of section
    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }
    
    // Set number of rows
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return isSearching ? searchResult.count : pokemon.count
    }
    
    // Set cell's text
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        // Grab a cell from cell pool.
        let cell = tableView.dequeueReusableCell(withIdentifier: "PokemonCell", for: indexPath)
        cell.textLabel?.text = isSearching ? capitalize(text: searchResult[indexPath.row].name) : capitalize(text: pokemon[indexPath.row].name)
        return cell
    }
    
    // Prepare for segue
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Ensure the segue is ShowPokemonSegue
        if segue.identifier == "ShowPokemonSegue",
                // Ensure the view we are going to is a PokemonViewController
                let destination = segue.destination as? PokemonViewController,
                // Ensure the selected row
                let index = tableView.indexPathForSelectedRow?.row {
            destination.url = isSearching ? searchResult[index].url : pokemon[index].url
        }
    }
}

