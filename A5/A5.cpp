// Termm--Fall 2024

#include <glm/ext.hpp>

#include "A5.hpp"
#include "Ray.hpp"
#include "Photon.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include <cstdlib>
#include <ctime>

using namespace std;
bool stochasticSampling;
bool softShadow;
bool reflection;
bool refraction;
bool glossyReflection;
bool glossyRefraction;
bool motionBlur;

// Generate a random float between 0.0 and 1.0
float rand_float() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

glm::vec3 traceRay(Ray &ray, SceneNode *root, const glm::vec3 &eye, const glm::vec3 &ambient, const std::list<Light *> &lights, int depth = 3) {
    Photon photon;
    glm::vec3 color(0.0f);
    glm::vec2 interval(0.001f, 10000.0f);
    // Check for intersection
    if (root->intersect(ray, interval, photon)) {
        photon.normal = normalize(photon.normal);
        photon.hitPoint += photon.normal * 0.001f; // Offset to prevent self-intersection

        PhongMaterial *material = static_cast<PhongMaterial *>(photon.material);

        // Ambient component
        glm::vec3 sampled_color = material->diffuse(photon.uv);
        // std::cout << "Sampled Color: " << sampled_color.x << " " << sampled_color.y << " " << sampled_color.z << std::endl;
        color += ambient * material->diffuse(photon.uv);

        if (softShadow){
            for (Light *light : lights) {
            // Cast multiple shadow rays for soft shadowing
            int shadowSamples = 32;  // Number of shadow rays (adjustable for quality vs. performance)
            float lightRadius = 20.0f; // Radius of area light (define in Light class)
            glm::vec3 lightColor(0.0f);

            for (int i = 0; i < shadowSamples; i++) {
                glm::vec3 lightPos = light->position;
                glm::vec3 u = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), ray.direction));
                if (glm::length(u) < 1e-6f){
                    u = glm::normalize(glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), ray.direction));
                }
                glm::vec3 v = glm::normalize(glm::cross(ray.direction, u));
                float randU = lightRadius * sqrt(rand_float()) * glm::cos(rand_float() * 2.0f * M_PI);
                float randV = lightRadius * sqrt(rand_float()) * glm::sin(rand_float() * 2.0f * M_PI);

                lightPos += randU * u + randV * v; // Perturbed light position for soft shadow sampling

                // Shadow ray
                Ray shadowRay(photon.hitPoint, lightPos - photon.hitPoint);
                Photon shadowRay_photon;
                glm::vec2 tempInterval(0.001f, length(lightPos - photon.hitPoint));

                // Check if shadow ray is blocked
                if (!root->intersect(shadowRay, tempInterval, shadowRay_photon)) {
                    glm::vec3 N = normalize(photon.normal);
                    glm::vec3 S = normalize(shadowRay.direction);
                    glm::vec3 R = normalize(2.0f * N * dot(N, S) - S);
                    glm::vec3 V = normalize(eye - photon.hitPoint);
                    double r = length(shadowRay.direction);
                    double attenuation = 1.0f / (light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r);

                    // Diffuse component
                    lightColor += glm::max(0.0f, dot(S, N)) * attenuation * material->diffuse(photon.uv) * light->colour;
                    // Specular component
                    lightColor += pow(glm::max(0.0f, dot(R, V)), material->shininess()) * attenuation * material->specular() * light->colour;
                }
            }
            lightColor /= float(shadowSamples);
            color += lightColor;
        }
        }
        else{
            // Illumination (diffuse + specular)
            for (Light *light : lights) {
                Ray shadowRay(photon.hitPoint, light->position - photon.hitPoint);
                Photon shadowRay_photon;
                glm::vec2 tempInterval(0.001f, length(light->position - photon.hitPoint));

                // Shadow test
                if (!root->intersect(shadowRay, tempInterval, shadowRay_photon)) {
                    glm::vec3 N = normalize(photon.normal);
                    glm::vec3 S = normalize(shadowRay.direction);
                    glm::vec3 R = normalize(2.0f * N * dot(N, S) - S);
                    glm::vec3 V = normalize(eye - photon.hitPoint);
                    double r = length(shadowRay.direction);
                    double attenuation = 1.0f / (light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r);

                    // Diffuse component
                    color += glm::max(0.0f, dot(S, N)) * attenuation * material->diffuse(photon.uv) * light->colour;
                    // Specular component
                    color += pow(glm::max(0.0f, dot(R, V)), material->shininess()) * attenuation * material->specular() * light->colour;
                }
            }
        }

        // Reflection and Refraction
        if (depth > 0 && (material->reflection() > 0.0f || material->transparency() > 0.0f)) {
            glm::vec3 reflectionColor(0.0f);
            glm::vec3 refractionColor(0.0f);

            // Reflection
            if (reflection) {
                // Basic reflection
                glm::vec3 reflectDir = normalize(ray.direction - 2.0f * photon.normal * dot(photon.normal, ray.direction));
                Ray reflectRay(photon.hitPoint + reflectDir * 0.001f, reflectDir);
                reflectionColor += traceRay(reflectRay, root, eye, ambient, lights, depth - 1);
                // Glossy Reflection
                if (glossyReflection) {
                    int glossySamples = 8;
                    float glossySpread = 0.2f;
                    glm::vec3 glossyColor(0.0f);

                    // Generate an orthonormal basis for perturbation
                    glm::vec3 u = normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), reflectDir));
                    if (glm::length(u) < 1e-6f) {
                        u = normalize(glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), reflectDir));
                    }
                    glm::vec3 v = normalize(glm::cross(reflectDir, u));

                    // Sample multiple rays for glossy effect
                    for (int i = 0; i < glossySamples; ++i) {
                        // Generate random offsets in a disk
                        float r = glossySpread * sqrt(rand_float()); // Radial distance
                        float theta = rand_float() * 2.0f * M_PI;    // Angle in polar coordinates
                        float offsetU = r * cos(theta);
                        float offsetV = r * sin(theta);

                        // Perturb the reflection direction
                        glm::vec3 perturbedDir = normalize(reflectDir + offsetU * u + offsetV * v);

                        // Trace the perturbed ray
                        Ray glossyRay(photon.hitPoint + perturbedDir * 0.001f, perturbedDir);
                        glossyColor += traceRay(glossyRay, root, eye, ambient, lights, depth - 1);
                    }

                    // Average the glossy rays
                    reflectionColor /= float(glossySamples);
                    reflectionColor += glossyColor / float(glossySamples);
                }
            }

            

            // Refraction
            if (refraction) {
                double eta = material->refraction();
                glm::vec3 I = normalize(ray.direction);
                glm::vec3 N = normalize(photon.normal);
                float cos_theta = glm::dot(-I, N);

                // Handle inside-to-outside transition
                if (cos_theta < 0.0f) {
                    N = -N;
                    eta = 1.0 / eta;
                    cos_theta = -cos_theta;
                }

                float discriminant = 1.0 - eta * eta * (1.0 - cos_theta * cos_theta);
                if (discriminant > 0.0f) {
                    glm::vec3 refractionDir = normalize(eta * I + (eta * cos_theta - sqrt(discriminant)) * N);
                    Ray refractionRay(photon.hitPoint - N * 0.001f, refractionDir);
                    refractionColor += traceRay(refractionRay, root, eye, ambient, lights, depth - 1);

                    if (glossyRefraction) {
                        int glossySamples = 4; // Smooth glossy refraction
                        float glossySpread = 0.05f;
                        glm::vec3 glossyColor(0.0f);

                        // Create orthonormal basis for glossy perturbations
                        glm::vec3 u = normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), refractionDir));
                        if (glm::length(u) < 1e-6f) {
                            u = normalize(glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), refractionDir));
                        }
                        glm::vec3 v = normalize(glm::cross(refractionDir, u));

                        // Sample glossy rays
                        for (int i = 0; i < glossySamples; ++i) {
                            float r = glossySpread * sqrt(rand_float());
                            float theta = rand_float() * 2.0f * M_PI;
                            float offsetU = r * cos(theta);
                            float offsetV = r * sin(theta);

                            glm::vec3 perturbedDir = normalize(refractionDir + offsetU * u + offsetV * v);
                            Ray glossyRay(photon.hitPoint - N * 0.001f, perturbedDir);
                            glossyColor += traceRay(glossyRay, root, eye, ambient, lights, depth - 1);
                        }
                        refractionColor /= float(glossySamples);
                        refractionColor += glossyColor / float(glossySamples);
                    }
                }
            }
            

            // Combine reflection, refraction, and diffuse components
            float reflectivity = reflection? material->reflection() : 0.0f;
            float transparency = refraction? material->transparency() : 0.0f;
            color = 1.0f / (1.0f + reflectivity + transparency) * color + reflectivity / (1.0f + reflectivity + transparency) * reflectionColor + transparency / (1.0f + reflectivity + transparency) * refractionColor;
        }

        return color;
    } else {
        // Background color gradient
        glm::vec3 topColor = glm::vec3(0.6, 0.0, 0.2); // Pink for the top
        glm::vec3 botColor = glm::vec3(0.2, 0.5, 1.0); // Purple for the bottom
        float t = 0.5f * (1.0f + glm::normalize(ray.direction).y);
        return (1.0f - t) * botColor + t * topColor;
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
		bool enableStochasticSampling,
        bool enableSoftShadow,
        bool enableReflection,
        bool enableRefraction,
        bool enableGlossyReflection,
        bool enableGlossyRefraction,
        bool enableMotionBlur
) {

  // Fill in raytracing code here...  
  stochasticSampling = enableStochasticSampling;
  softShadow = enableSoftShadow;
  reflection = enableReflection;
  refraction = enableRefraction;
  glossyReflection = enableGlossyReflection;
  glossyRefraction = enableGlossyRefraction;
  motionBlur = enableMotionBlur;

  std::cout << "Stochastic Sampling: " << stochasticSampling << std::endl;
  std::cout << "Soft Shadow: " << softShadow << std::endl;
  std::cout << "Reflection: " << reflection << std::endl;
  std::cout << "Refraction: " << refraction << std::endl;
  std::cout << "Glossy Reflection: " << glossyReflection << std::endl;
  std::cout << "Glossy Refraction: " << glossyRefraction << std::endl;
  std::cout << "Motion Blur: " << motionBlur << std::endl;


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
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			glm::vec3 colour(0.0f);
			if (stochasticSampling) {
                if (!motionBlur){
                    int sample = 16;
                    // Stockastic sampling
                    for (int i = 0; i < sample; i++) {
                        // Generate random offsets within the pixel
                        double randX = (rand_float() - 0.5) * 0.5;
                        double randY = (rand_float() - 0.5) * 0.5;

                        double px = (2.0 * (x + 0.5 + randX) / static_cast<double>(w) - 1.0) * imagePlaneWidth / 2.0;
                        double py = (1.0 - 2.0 * (y + 0.5 + randY) / static_cast<double>(h)) * imagePlaneHeight / 2.0;

                        const glm::vec3 primaryRay = glm::normalize(px * u_cam + py * v_cam - w_cam);
                        Ray ray = Ray(eye, primaryRay);

                        // Accumulate color from random sample
                        colour += traceRay(ray, root, eye, ambient, lights);
                    }
                    // Average colour over samples
                    colour /= static_cast<double>(sample);
                }
                else{
                    int sample = 2;
                    int motionSamples = 8;
                    // Stockastic sampling
                    for (int i = 0; i < sample; i++) {
                        for (int j = 0; j < motionSamples; j++) {
                            double time = j * (1.0f / (double) motionSamples);
                            // Generate random offsets within the pixel
                            double randX = (rand_float() - 0.5) * 0.5;
                            double randY = (rand_float() - 0.5) * 0.5;

                            double px = (2.0 * (x + 0.5 + randX) / static_cast<double>(w) - 1.0) * imagePlaneWidth / 2.0;
                            double py = (1.0 - 2.0 * (y + 0.5 + randY) / static_cast<double>(h)) * imagePlaneHeight / 2.0;

                            const glm::vec3 primaryRay = glm::normalize(px * u_cam + py * v_cam - w_cam);
                            Ray ray = Ray(eye, primaryRay, time);

                            // Accumulate color from random sample
                            colour += traceRay(ray, root, eye, ambient, lights);
                        }
                        colour /= static_cast<double>(motionSamples);
                        colour *= 0.35;

                        double randX = (rand_float() - 0.5) * 0.5;
                        double randY = (rand_float() - 0.5) * 0.5;

                        double px = (2.0 * (x + 0.5 + randX) / static_cast<double>(w) - 1.0) * imagePlaneWidth / 2.0;
                        double py = (1.0 - 2.0 * (y + 0.5 + randY) / static_cast<double>(h)) * imagePlaneHeight / 2.0;

                        const glm::vec3 primaryRay = glm::normalize(px * u_cam + py * v_cam - w_cam);
                        Ray finalRay = Ray(eye, primaryRay, 1);

                        // Accumulate color from random sample
                        colour += (traceRay(finalRay, root, eye, ambient, lights) * 0.65);


                    }
                    // Average colour over samples
                    colour /= static_cast<double>(sample);
                }
			}
			else{
				// without anti-aliasing
                if (!motionBlur){
                    double px = (2.0 * (x + 0.5) / static_cast<double>(w) - 1.0) * imagePlaneWidth / 2.0;
                    double py = (1.0 - 2.0 * (y + 0.5) / static_cast<double>(h)) * imagePlaneHeight / 2.0;

                    const glm::vec3 primaryRay = glm::normalize(px * u_cam + py * v_cam - w_cam);
                    Ray ray = Ray(eye, primaryRay);
                    colour += traceRay(ray, root, eye, ambient, lights);
                }
                else{
                    int motionSamples = 24;
                    for (int j = 0; j < motionSamples; j++) {
                        double time = j * (1.0f / (double) motionSamples);
                        double px = (2.0 * (x + 0.5) / static_cast<double>(w) - 1.0) * imagePlaneWidth / 2.0;
                        double py = (1.0 - 2.0 * (y + 0.5) / static_cast<double>(h)) * imagePlaneHeight / 2.0;

                        const glm::vec3 primaryRay = glm::normalize(px * u_cam + py * v_cam - w_cam);
                        Ray ray = Ray(eye, primaryRay, time);
                        // std::cout << "time: " << ray.time << std::endl;
                        colour += traceRay(ray, root, eye, ambient, lights);
                    }
                    colour /= static_cast<double>(motionSamples);
                    colour *= 0.5f;

                    double px = (2.0 * (x + 0.5) / static_cast<double>(w) - 1.0) * imagePlaneWidth / 2.0;
                    double py = (1.0 - 2.0 * (y + 0.5) / static_cast<double>(h)) * imagePlaneHeight / 2.0;

                    const glm::vec3 primaryRay = glm::normalize(px * u_cam + py * v_cam - w_cam);
                    Ray finalRay = Ray(eye, primaryRay, 1);
                    colour += (traceRay(finalRay, root, eye, ambient, lights) * 0.5f);
                }
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
