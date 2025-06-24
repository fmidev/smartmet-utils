#include <Magick++.h>
#include <iostream>
#include <cmath>
#include <string>

double pngdiff(const std::string& image1_path, const std::string& image2_path) {
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

        return psnr;
    } catch (const Exception& e) {
        cerr << "Error: " << e.what() << endl;
        return -1.0; // Indicate an error
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <image1> <image2>" << std::endl;
        return 1;
    }

    Magick::InitializeMagick(nullptr);

    std::string image1_path = argv[1];
    std::string image2_path = argv[2];

    double psnr = pngdiff(image1_path, image2_path);
    if (psnr >= 0) {
        std::cout << "PSNR: " << psnr << " dB" << std::endl;
    } else {
        std::cout << "Failed to calculate PSNR." << std::endl;
    }

    return 0;
}
