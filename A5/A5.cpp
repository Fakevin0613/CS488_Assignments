// Termm--Fall 2024

#include <glm/ext.hpp>

#include "A5.hpp"
#include "Ray.hpp"
#include "Photon.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
using namespace std;

glm::vec3 traceRay(Ray &ray, SceneNode *root, const glm::vec3 & eye, const glm::vec3 & ambient, const std::list<Light *> & lights) {
	Photon photon;
    glm::vec3 color;
	glm::vec2 interval(0.001f, 10000.0f);
	if ( root->intersect( ray, interval, photon ) ) {
		photon.normal = normalize(photon.normal);
		photon.hitPoint += photon.normal * 0.001f;

		PhongMaterial *material = static_cast<PhongMaterial *>(photon.material);
		// ambient
		color += ambient * material->diffuse();
		for (Light * light : lights) {
			Ray shadowRay(photon.hitPoint, light->position - photon.hitPoint);
			Photon shadowRay_photon;
			glm::vec2 tempInterval(0.001f, length(light->position - photon.hitPoint));
			
			if (root->intersect( shadowRay, tempInterval, shadowRay_photon)) {
				continue;  // skip this light when blocked
			}
			glm::vec3 N = normalize(photon.normal);
			glm::vec3 S = normalize(shadowRay.direction);
			glm::vec3 R = normalize(2 * N * dot(N, S) - S);
			glm::vec3 V = normalize(eye - photon.hitPoint);
			double r = length(shadowRay.direction);
			double attenuation = 1.0f / ( light->falloff[0] + light->falloff[1]*r + light->falloff[2]*r*r );

			// diffuse
			color += glm::max(0.0f, dot(S, N)) * attenuation * material->diffuse() * light->colour;
			// specular
			color += pow(glm::max(0.0f, dot(R, V)), material->shininess()) * attenuation * material->specular() * light->colour;
		}
		return color; 
	} else {
		// non-trivial background
        glm::vec3 topColor = glm::vec3(0.6, 0.0, 0.2);  // pink for the top
		glm::vec3 botColor = glm::vec3(0.2, 0.5, 1.0);   // purple for the bot
		float t = 0.5f * (1.0f + glm::normalize(ray.direction).y);
		glm::vec3 color = (1.0f - t) * botColor + t * topColor;
		return color; 
	}
}

void A5_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights,
		bool enableSuperSampling
) {

  // Fill in raytracing code here...  

  std::cout << "F24: Calling A5_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

    double aspect_ratio = static_cast<double>(w) / h;
    double imagePlaneHeight = 2.0 * glm::tan(glm::radians(fovy / 2.0));
    double imagePlaneWidth = imagePlaneHeight * aspect_ratio;

    glm::vec3 w_cam = glm::normalize(-view);           // Forward direction
    glm::vec3 u_cam = glm::normalize(glm::cross(up, w_cam)); // Right direction
    glm::vec3 v_cam = glm::cross(w_cam, u_cam);        // Up direction
	std::cout << "Super Sampling: " << enableSuperSampling << std::endl;
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			glm::vec3 colour(0.0f);
			if (enableSuperSampling) {
				// with super sampling
				int sample = 3;
				for (int i = 0; i < sample; i++) {
					for (int j = 0; j < sample; j++) {
						double px = (2.0 * (x + (i + 0.5) / sample) / static_cast<double>(w) - 1.0) * imagePlaneWidth / 2.0;
						double py = (1.0 - 2.0 * (y + (j + 0.5) / sample) / static_cast<double>(h)) * imagePlaneHeight / 2.0;
						const glm::vec3 primaryRay = glm::normalize(px * u_cam + py * v_cam - w_cam);
						Ray ray = Ray(eye, primaryRay);
						colour += traceRay(ray, root, eye, ambient, lights);
					}
				}
				// Average colour over samples
    			colour /= static_cast<double>(sample * sample);
			}
			else{
				// without super sampling
				double px = (2.0 * (x + 0.5) / static_cast<double>(w) - 1.0) * imagePlaneWidth / 2.0;
				double py = (1.0 - 2.0 * (y + 0.5) / static_cast<double>(h)) * imagePlaneHeight / 2.0;

				const glm::vec3 primaryRay = glm::normalize(px * u_cam + py * v_cam - w_cam);
				Ray ray = Ray(eye, primaryRay);
				colour += traceRay(ray, root, eye, ambient, lights);
			}
			colour = glm::clamp(colour, glm::vec3(0.0f), glm::vec3(1.0f));

			image(x, y, 0) = (double) colour.r;
			image(x, y, 1) = (double) colour.g;
			image(x, y, 2) = (double) colour.b;
		}
		if (y % (h / 100) == 0 || y == h - 1) {
			double progress = (static_cast<double>(y) / h) * 100.0f;
			std::cout << "\rRendering: " << std::fixed << progress << "% completed" << std::flush;
		}
	}
	std::cout << std:: endl << "Finished!" << std::endl;
}
