#include <cmath>
#include <iostream>
#include <vector>
#include "sift.hpp"
#include "image.hpp"

namespace sift {

ScaleSpacePyramid generate_scale_space_pyramid(const Image& img, float sigma) //TODO: rename sigma -> base_sigma
{
    // assume initial sigma is 1.0 (after resizing) and smooth
    // the image with sigma_diff to reach requried smoothing
    Image base_img = rgb_to_grayscale(img);
    base_img = base_img.resize(img.width*2, img.height*2);
    std::cout << "resized base image\n";
    float sigma_diff = std::sqrt(sigma*sigma - 1.0f);
    base_img = convolve(base_img, make_gaussian_filter(sigma_diff), true);
    std::cout << "convolved base image\n";

    int num_octaves = 4, scales_per_octave = 3;
    int imgs_per_octave = scales_per_octave + 3;

    // determine all sigma values for all
    // and generate gaussian kernels for all sigmas
    float k = std::pow(2, 1.0/scales_per_octave);
    std::vector<float> sigma_vals {sigma};
    for (int i = 1; i < imgs_per_octave; i++) {
        float sigma_prev = sigma * std::pow(k, sigma_vals[i-1]);
        float sigma_total = k * sigma_prev;
        sigma_vals.push_back(std::sqrt(sigma_total*sigma_total - sigma_prev*sigma_prev));
    }
    std::vector<Image> gaussian_kernels;
    for (int i = 1; i < sigma_vals.size(); i++)
        gaussian_kernels.push_back(make_gaussian_filter(sigma_vals[i]));
    std::cout << "generated kernels\n";
    // create a scale space pyramid of gaussian images
    // images in each octave are 2x smaller than in the previous
    ScaleSpacePyramid pyramid = {
        .num_octaves = num_octaves,
        .imgs_per_octave = imgs_per_octave,
        .octaves = std::vector<std::vector<Image>>(num_octaves)
    };
    for (int i = 0; i < num_octaves; i++) {
        pyramid.octaves[i].push_back(base_img);
        for (Image& kernel : gaussian_kernels) {
            Image& prev_img = pyramid.octaves[i].back();
            pyramid.octaves[i].push_back(convolve(prev_img, kernel, true));
        }
        // prepare base image for next octave
        base_img = pyramid.octaves[i][imgs_per_octave-3];
        base_img = base_img.resize(base_img.width/2, base_img.height/2); //TODO: nearest neighbour
    }
    return pyramid;
}

// generate pyramid of difference of gaussian (DoG) images
DoGPyramid generate_dog_pyramid(const ScaleSpacePyramid& img_pyramid)
{
    DoGPyramid dog_pyramid = {
        .num_octaves = img_pyramid.num_octaves,
        .imgs_per_octave = img_pyramid.imgs_per_octave - 1,
        .octaves = std::vector<std::vector<Image>>(img_pyramid.num_octaves)
    };
    for (int i = 0; i < dog_pyramid.num_octaves; i++) {
        for (int j = 1; j < img_pyramid.imgs_per_octave; j++) {
            Image diff = img_pyramid.octaves[i][j];
    std::cout << "here\n";
            for (int pix_idx = 0; pix_idx < diff.size; pix_idx++) {
                diff.data[pix_idx] -= img_pyramid.octaves[i][j-1].data[pix_idx];
            }
            dog_pyramid.octaves[i].push_back(diff);
        }
    }
    return dog_pyramid;
}

} //namespace sift
