-- A simple scene with some miscellaneous geometry.
-- This file is very similar to nonhier.lua, but interposes
-- an additional transformation on the root node.  
-- The translation moves the scene, and the position of the camera
-- and lights have been modified accordingly.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0, 1, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0, 1, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
-- mat5 = gr.material({0.0, 0.3, 0.6}, {0.0, 0.2, 0.55}, 25, 0.7, 0, 1.5)
-- mat6 = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 10, 2, 0, 1.8)
-- mat7 = gr.material({0, 0, 0}, {0, 0, 0}, 10, 2, 0, 1.8)
mat5 = gr.material({0.0, 0.3, 0.6}, {0.0, 0.2, 0.55}, 25)
mat6 = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 10)
mat7 = gr.material({0, 0, 0}, {0, 0, 0}, 10)

scene = gr.node( 'scene' )
scene:translate(0, 0, -800)

s1 = gr.nh_sphere('s1', {-200, -125, -800}, 100)
scene:add_child(s1)
s1:set_material(mat3)

s2 = gr.nh_sphere('s2', {200, 50, -200}, 150)
scene:add_child(s2)
s2:set_material(mat1)

s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
scene:add_child(s3)
s3:set_material(mat2)

b1 = gr.nh_box('b1', {-200, -125, 0}, 100)
scene:add_child(b1)
b1:set_material(mat5)

s4 = gr.nh_sphere('s4', {0, 0, -400}, 50)
scene:add_child(s4)
s4:set_material(mat3)

s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
scene:add_child(s5)
s5:set_material(mat1)

b2 = gr.nh_box('b2', {100, 50, 200}, 100)
scene:add_child(b2)
b2:set_material(mat5)

-- A small stellated dodecahedron.

steldodec = gr.mesh( 'dodec', 'Assets/smstdodeca.obj' )
steldodec:set_material(mat3)
scene:add_child(steldodec)

white_light = gr.light({-100.0, 150.0, -400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, -650.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'nonhier2.png', 256, 256,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
