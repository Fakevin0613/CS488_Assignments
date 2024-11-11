// Termm--Fall 2024

#include "GeometryNode.hpp"
using namespace std;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

bool GeometryNode::intersect(Ray& ray, glm::vec2 interval, HitRecord& hitRecord)
{
	Ray invRay(
        glm::vec3(invtrans * glm::vec4(ray.origin, 1.0)),
        glm::vec3(invtrans * glm::vec4(ray.direction, 0.0))
    );

	HitRecord invHitRecord;
	invHitRecord.material = nullptr;
	bool hit = false;
	float near = interval[1];

	if (m_primitive->intersect(invRay, interval, invHitRecord)) {
		hit = true;
		if ( invHitRecord.material == nullptr ) 
			invHitRecord.material = m_material;
		near = invHitRecord.t;
		hitRecord = invHitRecord;
	}

	if( SceneNode::intersect(ray, interval, hitRecord) ) {
		hit = true;
		near = invHitRecord.t;
		hitRecord = invHitRecord;
		cout << "SceneNode hit" << endl;
	}

	if (hit) {
		hitRecord.hitPoint = glm::vec3(trans * glm::vec4(hitRecord.hitPoint, 1.0));
        hitRecord.normal = glm::mat3(glm::transpose(invtrans)) * hitRecord.normal;
	}
	return hit;
}