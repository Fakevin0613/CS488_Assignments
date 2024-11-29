-- A simple scene with five spheres

mat1 = gr.material({0.6, 0.8, 0.6}, {0.5, 0.6, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

scene_root = gr.node('root')

s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
scene_root:add_child(s3)
s3:set_material(mat2)

s6 = gr.cylinder('s6', {150, -200, -200}, {0, 1, 0}, 30, 300)
scene_root:add_child(s6)
s6:set_material(mat3)
s6:rotate('X', -30)

s7 = gr.cone('s7', {-150, -200, -250}, {0, 1, 0}, 2, 200)
scene_root:add_child(s7)
s7:set_material(mat3)
s7:rotate('X', -30)

s8 = gr.cone('s8', {0, 100, 200}, {0, 1, 0}, 1, 250)
scene_root:add_child(s8)
s8:set_material(mat1)
s8:rotate('X', 180)

s9 = gr.cylinder('s9', {100, 200, 100}, {0, 1, 0}, 20, 200)
scene_root:add_child(s9)
s9:set_material(mat1)
s9:rotate('Z', -120)


white_light = gr.light({-200.0, 250.0, 600.0}, {0.8, 0.8, 0.8}, {1, 0, 0})
magenta_light = gr.light({600.0, 200.0, 250.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'primitive-cylinder-and-cone.png', 2048, 2048,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
