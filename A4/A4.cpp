// Termm--Fall 2024

#include <glm/ext.hpp>

#include "A4.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"
#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
using namespace std;

// glm::vec3 traceRay(Ray& ray, SceneNode* root, const glm::vec3& ambient, const std::list<Light*>& lights) {
//     HitRecord hitRecord;
//     glm::vec2 interval(0.001f, std::numeric_limits<float>::max());

//     if (root->intersect(ray, interval, hitRecord)) {
//         // Base colour is ambient light
//         glm::vec3 colour = ambient * hitRecord.material->shininess();

//         // Loop over all lights to compute diffuse and specular components
//         for (const auto& light : lights) {
//             glm::vec3 light_dir = glm::normalize(light->position - hitRecord.hitPoint);
//             float light_distance = glm::length(light->position - hitRecord.hitPoint);

//             // Shadow ray to check if this point is in shadow
//             Ray shadowRay(hitRecord.hitPoint + 0.001f * hitRecord.normal, light_dir);
//             HitRecord shadowHit;
//             glm::vec2 shadowInterval(0.001f, light_distance);

//             if (!root->intersect(shadowRay, shadowInterval, shadowHit)) {
//                 // Diffuse shading
//                 float diff = glm::max(glm::dot(hitRecord.normal, light_dir), 0.0f);
//                 colour += diff * hitRecord.material->diffuse() * light->colour;

//                 // Specular shading
//                 glm::vec3 reflectDir = glm::reflect(-light_dir, hitRecord.normal);
//                 float spec = glm::pow(glm::max(glm::dot(reflectDir, -ray.direction), 0.0f), hitRecord.material->shininess());
//                 colour += spec * hitRecord.material->shininess() * light->colour;
//             }
//         }
//         return colour;
//     }
//     return glm::vec3(0.0f);  // Background colour (black)
// }


glm::vec3 trace_color(Ray &ray,
            SceneNode *root,
            const glm::vec3 & eye,
            const glm::vec3 & ambient,
            const std::list<Light *> & lights,
            const int maxHits = 5) {
	HitRecord record;
    glm::vec3 color;
	glm::vec2 interval(0.001f, std::numeric_limits<float>::max());
	if ( root->intersect( ray, interval, record ) ) {
		// hit
		record.normal = normalize(record.normal);
		record.hitPoint += record.normal * 0.0001;

		PhongMaterial *material = static_cast<PhongMaterial *>(record.material);

		// ambient
		color += material->diffuse() * ambient;

		for (Light * light : lights) {
			Ray shadowRay(record.hitPoint, light->position - record.hitPoint);
			HitRecord shadowRay_record;
			glm::vec2 tempInterval(0.0f, std::numeric_limits<float>::max());
			
			if (root->intersect( shadowRay, tempInterval, shadowRay_record)) {
				continue;
			}

			glm::vec3 L = normalize(shadowRay.direction);
			glm::vec3 N = normalize(record.normal);
			glm::vec3 R = normalize(2 * N * dot(N, L) - L);
			glm::vec3 V = normalize(eye - record.hitPoint);
			double r = length(shadowRay.direction);
			double attenuation = 1.0 / ( light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r );

			// // diffuse
			// color += dot(L, N) * attenuation * material->diffuse() * light->colour;

			// specular
			color += pow(glm::max(0.0, (double)dot(R, V)), material->shininess()) * attenuation * material->specular() * light->colour;
		}

		if (maxHits > 0) {
			glm::vec3 reflection_direction = ray.direction - 2 * record.normal * dot(ray.direction, record.normal);
            Ray reflection_ray(record.hitPoint, reflection_direction);
			// cout << "relection ray " << to_string(reflection_ray.origin) << endl;
			float reflect_coef = 0.2;
			color = glm::mix(color, trace_color(reflection_ray, root, eye, ambient, lights, maxHits - 1), reflect_coef);
		}

	} else {
		// miss -> background
		glm::vec3 unit_direction = glm::normalize(ray.direction);
        float t = unit_direction.y;
        color += (1.0 - t) * glm::vec3(0.5, 0.4, 0.3) + t * glm::vec3(0.0, 0.7, 1.0);
	}
	return color; 
}

void A4_Render(
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
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...  

  std::cout << "F24: Calling A4_Render(\n" <<
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

	// // Compute the aspect ratio
    // double aspect_ratio = static_cast<double>(w) / h;

    // // Calculate the height and width of the image plane at a distance of 1
    // double imagePlaneHeight = 2.0 * glm::tan(glm::radians(fovy / 2.0));
    // double imagePlaneWidth = imagePlaneHeight * aspect_ratio;

    // // Camera basis vectors
    // glm::vec3 w_cam = glm::normalize(-view);           // Forward direction
    // glm::vec3 u_cam = glm::normalize(glm::cross(up, w_cam)); // Right direction
    // glm::vec3 v_cam = glm::cross(w_cam, u_cam);        // Up direction

	glm::vec3 _w = normalize(view); // z-axis
	glm::vec3 _u = normalize(cross(_w, up)); // x-axis
	glm::vec3 _v = cross(_u, _w); // y-axis
	float d = h / 2 / glm::tan(glm::radians(fovy / 2));
	glm::vec3 BL_corner_direction = _w * d - _u * (float)w / 2 - _v * (float)h / 2;

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			// // Convert pixel coordinates to normalized device coordinates
            // double px = (2.0 * (x + 0.5) / static_cast<double>(w) - 1.0) * imagePlaneWidth / 2.0;
            // double py = (1.0 - 2.0 * (y + 0.5) / static_cast<double>(h)) * imagePlaneHeight / 2.0;

            // // Calculate the direction of the ray for this pixel
            // glm::vec3 ray_dir = glm::normalize(px * u_cam + py * v_cam - w_cam);

            // // Create the ray starting at the eye position and in the calculated direction
            // Ray ray(eye, ray_dir);

            // // Trace the ray and determine colour based on intersections and lighting
            // glm::vec3 colour = traceRay(ray, root, ambient, lights);

			const glm::vec3 direction = BL_corner_direction + (float)(h - y) * _v + (float)x * _u;
			Ray ray = Ray(eye, direction);

			
			ray.direction = direction;
            glm::vec3 colour = trace_color(ray, root, eye, ambient, lights);

			// Red: 
			image(x, y, 0) = (double)colour.r;
			// Green: 
			image(x, y, 1) = (double)colour.g;
			// Blue: 
			image(x, y, 2) = (double)colour.b;
		}
	}

}
