//
//  ViewController.swift
//  Pokedex
//
//  Created by Ping Lu on 27.10.20.
//

import UIKit

class PokemonViewController: UIViewController {
    var url: String!
    var pokemonName: String!
    // Track whether pokemon is caught
    var isCaught: Bool = false

    @IBOutlet var nameLabel: UILabel!
    @IBOutlet var numberLabel: UILabel!
    @IBOutlet var type1Label: UILabel!
    @IBOutlet var type2Label: UILabel!
    @IBOutlet var actionButton: UIButton!
    @IBOutlet var pokemonImageView: UIImageView!
    @IBOutlet var descriptionTextView: UITextView!

    // Uppercase the first letter in text
    func capitalize(text: String) -> String {
        return text.prefix(1).uppercased() + text.dropFirst()
    }
    
    func setActionButton(button: UIButton) {
        // If pokemon is caught
        if isCaught {
            // ...set button's text to "Release"
            button.setTitle("Release", for: .normal)
            // ...and set its background color to green
            button.backgroundColor = UIColor.systemGreen
        }
        // Otherwise
        else {
            // ...set button's text to "Release"
            button.setTitle("Catch", for: .normal)
            // ...and set its background color to blue
            button.backgroundColor = UIColor.link
        }
    }

    // Handle click on action button
    @IBAction func toggleCatch() {
        // Toggle isCaught
        isCaught = !isCaught
        
        // Update isCaught state in UserDefaults
        UserDefaults.standard.set(isCaught, forKey: pokemonName)
        // Update action button's text and its background color.
        setActionButton(button: actionButton)
    }
    
    // View is about to be added
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)

        // ...clear all placeholder text
        nameLabel.text = ""
        numberLabel.text = ""
        type1Label.text = ""
        type2Label.text = ""
        descriptionTextView.text = ""

        // Load pokemon's data
        loadPokemon()
    }
    
    // Load pokemon's data
    func loadPokemon() {
        // Fetch pokemon's name, id, types and sprite
        URLSession.shared.dataTask(with: URL(string: url)!) { (data, response, error) in
            // Ensure data is not nil
            guard let data = data else {
                return
            }

            do {
                // Parse response data
                let result = try JSONDecoder().decode(PokemonResult.self, from: data)
               
                // Get pokemon front_default sprite
                let pokemonImageUrl = URL(string: result.sprites.front_default)!
                
                // Load remote image URL into pokemonImageView
                DispatchQueue.global().async {
                    [weak self] in
                    // Download pokemon image data from remote URL
                    if let data = try? Data(contentsOf: pokemonImageUrl) {
                        // Convert the image data to a UIImage
                        if let image = UIImage(data: data) {
                            // Back to main thread
                            DispatchQueue.main.async {
                                // Load it into UIImageView
                                self?.pokemonImageView.image = image
                            }
                        }
                    }
                }
                
                // Back to main thread
                DispatchQueue.main.async {
                    self.pokemonName = result.name
                    // Set title in navigation bar to pokemon's name
                    self.navigationItem.title = self.capitalize(text: result.name)
                    // Set name label's text to pokemon's name
                    self.nameLabel.text = self.capitalize(text: result.name)
                    // Set number label's text to pokemon's id
                    self.numberLabel.text = String(format: "#%03d", result.id)
                    
                    // Iterate over pokemon's types and set type labels' text
                    for typeEntry in result.types {
                        if typeEntry.slot == 1 {
                            self.type1Label.text = typeEntry.type.name
                        }
                        else if typeEntry.slot == 2 {
                            self.type2Label.text = typeEntry.type.name
                        }
                    }
                    
                    // If the state isCaught of the pokemon is already stored in UserDefaults
                    if UserDefaults.standard.object(forKey: result.name) != nil {
                        // ...set isCaught to the value stored in UserDefaults
                        self.isCaught = UserDefaults.standard.bool(forKey: result.name)
                    }
                   
                    // Set action button's text and its background color.
                    self.setActionButton(button: self.actionButton)
                }
                
                let pokemonId: Int = result.id
                
                // Fetch pokemon's description
                let descriptionUrl: String = "https://pokeapi.co/api/v2/pokemon-species/" + String(pokemonId) + "/"
                URLSession.shared.dataTask(with: URL(string: descriptionUrl)!) {(data, response, error)
                    in
                    // Ensure data is not nil
                    guard let data = data else {
                        return
                    }
                    
                    do {
                        // Parse response data
                        let result = try JSONDecoder().decode(PokemonDescription.self, from: data)
                        
                        // Back to main thread
                        DispatchQueue.main.async {
                            // Iterate over flavor_text_entries array
                            for flavorText in result.flavor_text_entries {
                                // Find the first English description
                                if flavorText.language.name == "en" {
                                    // Format text
                                    let text: String = flavorText.flavor_text.replacingOccurrences(of: "\n", with: " ")
                                    // Display formatted text in descriptionTextView
                                    self.descriptionTextView.text = text
                                    // Jump out of the loop
                                    break
                                }
                            }
                           
                        }
                    }
                    catch let error {
                        print(error)
                    }
                }.resume()
            }
            catch let error {
                print(error)
            }
        }.resume()
    }
    
}
