#include "scene.h"
#include "screen.h"
#include "raytracer.h"
#include "CImg-2.5.5/CImg.h"

int main() {
	while (true) {
		scene scene_;
		screen screen_(*scene_.m_camera);
		cimg_library::CImg<float> image(screen_.m_width, screen_.m_height, 1, 3, 0);

		raytracer raytracer_(scene_, screen_, image);
		raytracer_.run();
	}
}