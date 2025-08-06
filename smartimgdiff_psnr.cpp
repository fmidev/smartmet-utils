#include <Magick++.h>
#include <iostream>
#include <cmath>
#include <optional>
#include <string>

double pngdiff(
    const std::string& image1_path,
    const std::string& image2_path,
    const std::optional<std::string>& output_path = std::nullopt)
{
    using namespace Magick;
    using namespace std;

    try {
        // Load images
        Magick::Image img1(image1_path);
        Magick::Image img2(image2_path);

        // Check if images have the same dimensions
        if ((img1.size() != img2.size())
            || (img1.depth() != img2.depth()))
        {
            std::cerr << image1_path
                << " : size " << img1.size().width() << "x" << img1.size().height()
                << " depth=" << img1.depth() << std::endl;
            std::cerr << image2_path
                << " : size " << img2.size().width() << "x" << img2.size().height()
                << " depth=" << img2.depth() << std::endl;
            throw runtime_error("Images must have the same dimensions and depths");
        }

        const double mse = img1.compare(img2, Magick::MetricType::MeanSquaredErrorMetric);
        const double psnr = (mse == 0)
            ? std::numeric_limits<double>::infinity()
            : 20 * log10(1.0 / sqrt(mse));

        if (output_path and mse > 0)
        {
            // Save the difference image if output path is provided
            // Additionally apply contrast stretch to enhance visibility of differences
            try
            {
                // Create a difference image
                Magick::Image diff_image = img1;
                diff_image.composite(img2, 0, 0, Magick::CompositeOperator::DifferenceCompositeOp);
                diff_image.contrastStretch(0.0, 1.0);
                diff_image.write(*output_path);
            }
            catch(const std::exception& e)
            {
                std::cerr << "Failed to write difference image: " << e.what() << '\n';
            }
        }

        return psnr;
    } catch (const Exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1.0; // Indicate an error
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3 || argc > 4) {
        std::cerr << "Usage: " << argv[0] << " <image1> <image2> [output]" << std::endl;
        return 1;
    }

    Magick::InitializeMagick(nullptr);

    std::string image1_path = argv[1];
    std::string image2_path = argv[2];
    std::optional<std::string> output_path = (argc == 4) ? std::make_optional(argv[3]) : std::nullopt;

    double psnr = pngdiff(image1_path, image2_path, output_path);
    if (psnr >= 0) {
        std::cout << "PSNR: " << psnr << " dB" << std::endl;
    } else {
        std::cout << "Failed to calculate PSNR." << std::endl;
    }

    return 0;
}
