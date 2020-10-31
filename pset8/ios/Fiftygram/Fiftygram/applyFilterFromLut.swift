//
//  applyFilterFromLut.swift
//  Fiftygram
//
//  Created by Ping Lu on 30.10.20.
//

import UIKit

func applyFilter(with lookupImage: UIImage, to image: UIImage) -> UIImage? {
    guard let cgInputImage = image.cgImage else {
        return nil
    }

    guard let glContext = EAGLContext(api: .openGLES2) else {
        return nil
    }

    let ciContext = CIContext(eaglContext: glContext)

    guard let lookupFilter = CIFilter(lookupImage: lookupImage, dimension: 64) else {
        return nil
    }

    lookupFilter.setValue(CIImage(cgImage: cgInputImage),
                          forKey: "inputImage")

    guard let output = lookupFilter.outputImage else {
        return nil
    }

    guard let cgOutputImage = ciContext.createCGImage(output, from: output.extent) else {
        return nil
    }

    return UIImage(cgImage: cgOutputImage)
}
