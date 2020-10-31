//
//  chromaKeyEffect.swift
//  Fiftygram
//
//  Created by Ping Lu on 31.10.20.
//
//  Source:
//  https://developer.apple.com/documentation/coreimage/applying_a_chroma_key_effect


import UIKit

func chromaKeyFilter(fromHue: CGFloat, toHue: CGFloat) -> CIFilter?
{
    // Allocate memory. The color cube has three dimensions, each with four elements of data (RGBA).
    let size = 64
    var cubeRGB = [Float]()
        
    // Use a for-loop to iterate through each color combination of red, green, and blue, simulating a color gradient.
    for z in 0 ..< size {
        let blue = CGFloat(z) / CGFloat(size-1)
        for y in 0 ..< size {
            let green = CGFloat(y) / CGFloat(size-1)
            for x in 0 ..< size {
                let red = CGFloat(x) / CGFloat(size-1)
                    
                // Convert RGB to HSV. Even though the color cube exists in RGB color space, it is easier to isolate and remove color based on hue. Input 0 for green hues to indicate complete removal; use 1 for other hues to leave those colors intact. To specify green as a hue value, convert its angle in the hue pie chart to a range of 0 to 1. The green in the sample image has hue between 0.3 (108 out of 360 degrees) and 0.4 (144 out of 360 degrees). Your shade of green may differ, so adjust the range accordingly.
                let hue = getHue(red: red, green: green, blue: blue)
                let alpha: CGFloat = (hue >= fromHue && hue <= toHue) ? 0: 1
                    
                // The CIColorCube filter requires premultiplied alpha values, meaning that the values in the lookup table have their transparency baked into their stored entries rather than applied when accessed.
                cubeRGB.append(Float(red * alpha))
                cubeRGB.append(Float(green * alpha))
                cubeRGB.append(Float(blue * alpha))
                cubeRGB.append(Float(alpha))
            }
        }
    }

    let data = Data(buffer: UnsafeBufferPointer(start: &cubeRGB, count: cubeRGB.count))

    // Create a Core Image filter from the cube data by invoking the name CIColorCube.
    let colorCubeFilter = CIFilter(name: "CIColorCube", parameters: ["inputCubeDimension": size, "inputCubeData": data])
    return colorCubeFilter
}

// Converting RGB to HSV
func getHue(red: CGFloat, green: CGFloat, blue: CGFloat) -> CGFloat
{
    let color = UIColor(red: red, green: green, blue: blue, alpha: 1)
    var hue: CGFloat = 0
    color.getHue(&hue, saturation: nil, brightness: nil, alpha: nil)
    return hue
}
