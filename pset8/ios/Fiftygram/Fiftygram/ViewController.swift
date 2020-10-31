//
//  ViewController.swift
//  Fiftygram
//
//  Created by Ping Lu on 29.10.20.
//

import UIKit

class ViewController: UIViewController, UIImagePickerControllerDelegate, UINavigationControllerDelegate {
    @IBOutlet var imageView: UIImageView!
    @IBOutlet var instructionText: UITextView!
   
    // Create a CIContext object
    let context = CIContext()
    // Track the original photo
    var original: UIImage!
    // Track if user is selecting backgorund image for chroma key filter
    var isSelectingBackgorund: Bool = false
    // Track the background photo
    var background: UIImage!
    
    // Apply Sepia filter to the original photo
    @IBAction func applySepia() {
        // Ensure original photo is not nil
        if original == nil {
            return
        }
        
        // Instantiate a CIFilter object representing the filter to apply, and provide values for its parameters.
        let filter = CIFilter(name: "CISepiaTone")
        filter?.setValue(1.0, forKey: kCIInputIntensityKey)
        
        // Cast the original photo into a CIImage and provide it as the input image parameter to the filter.
        filter?.setValue(CIImage(image: original), forKey: kCIInputImageKey)
        
        // Display filtered photo.
        display(filter: filter!)
    }

    // Apply Noir filter to the original photo
    @IBAction func applyNoir() {
        // Ensure original photo is not nil
        if original == nil {
            return
        }
        
        // Instantiate a CIFilter object representing the filter to apply.
        let filter = CIFilter(name: "CIPhotoEffectNoir")
        
        // Cast the original photo into a CIImage and provide it as the input image parameter to the filter.
        filter?.setValue(CIImage(image: original), forKey: kCIInputImageKey)
        
        // Display filtered image.
        display(filter: filter!)
    }

    // Apply Vintage filter to the original photo
    @IBAction func applyVintage() {
        // Ensure original photo is not nil
        if original == nil {
            return
        }
        
        // Instantiate a CIFilter object representing the filter to apply, and provide values for its parameters.
        let filter = CIFilter(name: "CIPhotoEffectProcess")
        
        // Cast the original photo into a CIImage and provide it as the input image parameter to the filter.
        filter?.setValue(CIImage(image: original), forKey: kCIInputImageKey)
        
        // Display filtered photo.
        display(filter: filter!)
    }
    
    // Apply Chroma Key
    @IBAction func applyChromaKey() {
        // Ensure original photo is not nil
        if original == nil {
            return
        }
        
        isSelectingBackgorund = true
        // Let user select background image.
        openImagePicker()
    }
    
    // Apply old film filter which simulates scratchy analog film
    @IBAction func applyOldFilm() {
        guard
            // Ensure original photo is not nil
            let original = original,
            // Cast it into CIImage
            let ciOriginal = CIImage(image: original),
            // Apply old film filter
            let ciOldFilmImage = oldFilmFilter(inputImage: ciOriginal)
        else {
            return
        }
    
        // Display filtered image
        imageView.image = UIImage(cgImage: self.context.createCGImage(ciOldFilmImage, from: ciOldFilmImage.extent)!, scale: original.scale, orientation: original.imageOrientation)
    }

    // Handle photo selection
    @IBAction func choosePhoto(_ sender: UIBarButtonItem) {
        // Open image picker
        openImagePicker()
    }
    
    // Save photo
    @IBAction func savePhoto() {
        if let image = imageView.image {
            // Instantiate a ImageSaver object
            let imageSaver = ImageSaver()
            
            // Handle save success
            imageSaver.successHandler = {
                // Display notification banner informing user save succeeded
                NotificationBanner.show("Saved")
            }
            
            // Handle save error
            imageSaver.errorHandler = {
                print("Save error: \($0.localizedDescription)")
            }
            
            // Save photo to photo library
            imageSaver.writeToPhotoAlbum(image: image)
        }
    }
    
    // View loaded
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Display instruction message
        instructionText.isHidden = false
    }
    
    // Load the filtered image into UIImageView
    func display(filter: CIFilter) {
        // Get a CIImage object representing the filter’s output. The filter has not yet executed at this point—the image object is a “recipe” specifying how to create an image with the specified filter, parameters, and input. Core Image performs this recipe only when you request rendering.
        let output = filter.outputImage!
        
        // Render the output image to a Core Graphics image, then convert it back to a UIImage and load it into UIImageView.
        imageView.image = UIImage(cgImage: self.context.createCGImage(output, from: output.extent)!, scale: original.scale, orientation: original.imageOrientation)
    }
    
    // Open imager picker
    func openImagePicker() {
        // Ensure user's photo library is accessible
        if UIImagePickerController.isSourceTypeAvailable(.photoLibrary) {
            // Instantiate a UIImagePickerController object
            let picker = UIImagePickerController()
            // Make this the delegate of image picker
            picker.delegate = self
            // Specify the photo source
            picker.sourceType = .photoLibrary
            // Use the Navigation Controller to present the image picker
            self.navigationController?.present(picker, animated: true, completion: nil)
        }
    }

    // User selected a photo from the image picker
    func imagePickerController(
        _ picker: UIImagePickerController,
        didFinishPickingMediaWithInfo info: [UIImagePickerController.InfoKey : Any]
    ) {
        // Close the image picker
        self.navigationController?.dismiss(animated: true, completion: nil)
        
        // Get the photo user selected and cast it into a UIImage
        if let image = info[UIImagePickerController.InfoKey.originalImage] as? UIImage {
            // Hide the instruction message
            instructionText.isHidden = true
            
            // If current photo is not selected as background image
            if !isSelectingBackgorund {
                // Display the photo
                imageView.image = image
                // Remember the original photo
                original = image
            // Otherwise
            } else {
                // ...execute chroma key effect
                background = image
                chromaKeyEffect()
                isSelectingBackgorund = false
                background = nil
            }
        }
    }
    
    // Chroma Key Effect
    // Source:
    // https://developer.apple.com/documentation/coreimage/applying_a_chroma_key_effect
    func chromaKeyEffect() {
        // Remove green from original photo
        let chromaCIFilter = chromaKeyFilter(fromHue: 0.3, toHue: 0.4)
        chromaCIFilter?.setValue(CIImage(image: original), forKey: kCIInputImageKey)
        let sourceCIImageWithoutBackground = chromaCIFilter?.outputImage
        
        // Adjust the foreground image position
        let movedInputImage = sourceCIImageWithoutBackground!.transformed(by: CGAffineTransform(translationX: 200, y: 300)).transformed(by: CGAffineTransform(scaleX: 2, y: 2))
        
        // Ensure background photo is not nil
        if let background = background {
            // Cast the background image into a CIImage
            let backgroundCIImage = CIImage(image: background)
            
            // Chain a CISourceOverCompositing filter to the color cube filter to composite a background image to the greenscreened output.
            let compositor = CIFilter(name:"CISourceOverCompositing")
            compositor?.setValue(movedInputImage, forKey: kCIInputImageKey)
            compositor?.setValue(backgroundCIImage, forKey: kCIInputBackgroundImageKey)
            
            // Display composited photo
            display(filter: compositor!)
        }
    }
}

// Save image
class ImageSaver: NSObject {
    var successHandler: (() -> Void)?
    var errorHandler: ((Error) -> Void)?
    
    // Write image to photo library
    func writeToPhotoAlbum(image: UIImage) {
        UIImageWriteToSavedPhotosAlbum(image, self, #selector(saveError), nil)
    }
    
    // Handle save success and error
    @objc func saveError(_ image: UIImage, didFinishSavingWithError error: Error?, contextInfo: UnsafeRawPointer) {
        if let error = error {
            errorHandler?(error)
        } else {
            successHandler?()
        }
    }
}
