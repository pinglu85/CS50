//
//  scratchyAnalogFilter.swift
//  Fiftygram
//
//  Created by Ping Lu on 31.10.20.
//
//  Source:
//  https://developer.apple.com/documentation/coreimage/simulating_scratchy_analog_film

import UIKit

func oldFilmFilter(inputImage: CIImage) -> CIImage? {
    // Apply Sepia Tone filter
    guard
        let sepiaFilter = CIFilter(name: "CISepiaTone",
                                   parameters:
                                    [
                                        kCIInputImageKey: inputImage,
                                        kCIInputIntensityKey: 1.0
                                    ]),
        let sepiaCIImage = sepiaFilter.outputImage
    else {
        return nil
    }
    
    // Simulate grain by creating randomly varying speckle
    guard
        let coloredNoise = CIFilter(name: "CIRandomGenerator"),
        let noiseImage = coloredNoise.outputImage
    else {
        return nil
    }
    
    // Apply a whitening effect by chaining the noise output to a CIColorMatrix filter. This built-in filter multiplies the noise color values individually and applies a bias to each component. For white grain, apply whitening to the y-component of RGB and no bias.
    let whitenVector = CIVector(x: 0, y: 1, z: 0, w: 0)
    let fineGrain = CIVector(x:0, y:0.005, z:0, w:0)
    let zeroVector = CIVector(x: 0, y: 0, z: 0, w: 0)
    
    guard
        let whiteningFilter = CIFilter(name: "CIColorMatrix",
                                       parameters:
                                        [
                                            kCIInputImageKey: noiseImage,
                                            "inputRVector": whitenVector,
                                            "inputGVector": whitenVector,
                                            "inputBVector": whitenVector,
                                            "inputAVector": fineGrain,
                                            "inputBiasVector": zeroVector
                                        ]),
        let whiteSpecks = whiteningFilter.outputImage
    else {
        return nil
    }
    
    // Create the grainy image by compositing the whitened noise as input over the sepia-toned source image using the CISourceOverCompositing filter.
    guard
        let speckCompositor = CIFilter(name: "CISourceOverCompositing",
                                       parameters:
                                        [
                                            kCIInputImageKey: whiteSpecks,
                                            kCIInputBackgroundImageKey: sepiaCIImage
                                        ]),
        let speckedImage = speckCompositor.outputImage
    else {
        return nil
    }
    
    // Simulate Scratch by scaling randomly varying noise.
    // 1. Scale the random noise output vertically by applying a scaling CGAffineTransform.
    let verticalScale = CGAffineTransform(translationX: 1.5, y: 25)
    let transformedNoise = noiseImage.transformed(by: verticalScale)
    
    // 2. Color the output of the CIRandomGenerator filter. For the dark scratches, instead focus on only the red component, setting the other vector inputs to zero. This time, instead of multiplying the green, blue, and alpha channels, add bias (0, 1, 1, 1).
    let darkenVector = CIVector(x: 4, y: 0, z: 0, w: 0)
    let darkenBias = CIVector(x: 0, y: 1, z: 1, w: 1)
    
    guard
        let darkeningFilter = CIFilter(name: "CIColorMatrix",
                                       parameters:
                                        [
                                            kCIInputImageKey: transformedNoise,
                                            "inputRVector": darkenVector,
                                            "inputGVector": zeroVector,
                                            "inputBVector": zeroVector,
                                            "inputAVector": zeroVector,
                                            "inputBiasVector": darkenBias
                                        ]),
        let randomScratches = darkeningFilter.outputImage
    else {
        return nil
    }
    
    // The resulting scratches are cyan-colored, so grayscale them using the CIMinimumComponentFilter, which takes the minimum of the RGB values to produce a grayscale image.
   /* guard
        let grayScaleFilter = CIFilter(name: "CIMinimumComponentFilter",
                                       parameters:
                                        [
                                            kCIInputImageKey: randomScratches
                                        ]),
        let darkScratches = grayScaleFilter.outputImage
    else {
        return nil
    }
   */
    guard
         let grayScaleFilter = CIFilter(name: "CIPhotoEffectNoir",
                                        parameters:
                                         [
                                             kCIInputImageKey: randomScratches
                                         ]),
         let darkScratches = grayScaleFilter.outputImage
     else {
         return nil
     }
    
    // Composite the specks and scratches to the sepia image
    guard
        let oldFilm = CIFilter(name: "CIMultiplyCompositing",
                               parameters:
                                [
                                    kCIInputImageKey: darkScratches,
                                    kCIInputBackgroundImageKey: speckedImage
                                ]),
        let oldFilmImage = oldFilm.outputImage
    else {
        return nil
    }
    
    // Since the noise images had different dimensions than the source image, crop the composited result to the original image size to remove excess beyond the original extent.
    let finalImage = oldFilmImage.cropped(to: inputImage.extent)
    
    return finalImage
}
