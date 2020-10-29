//
//  Pokemon.swift
//  Pokedex
//
//  Created by Ping Lu on 27.10.20.
//

import Foundation

struct PokemonListResults: Codable {
    let results: [PokemonListResult]
}

struct PokemonListResult: Codable {
    let name: String
    let url: String
}

struct PokemonResult: Codable {
    let id: Int
    let name: String
    let sprites: PokemonSprites
    let types: [PokemonTypeEntry]
}

struct PokemonTypeEntry: Codable {
    let slot: Int
    let type: PokemonType
}

struct PokemonType: Codable {
    let name: String
}

struct PokemonSprites: Codable {
    let back_default: String?
    let back_female: String?
    let back_shiny: String?
    let back_shiny_female: String?
    let front_default: String
    let front_female: String?
    let front_shiny: String?
    let front_shiny_female: String?
}

struct PokemonDescription: Codable {
    let flavor_text_entries: [PokemonFlavorTextEntry]
}

struct PokemonFlavorTextEntry: Codable {
    let flavor_text: String
    let language: PokemonFlavorTextLanguage
    let version: PokemonFlavorTextVersion
}

struct PokemonFlavorTextLanguage: Codable {
    let name: String
    let url: String
}

struct PokemonFlavorTextVersion: Codable {
    let name: String
    let url: String
}
