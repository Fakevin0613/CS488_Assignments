-- Material
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 1.0, 1.0}, {0.8, 0.8, 0.8}, 50)
mat4 = gr.material({0.9, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50)
mat5 = gr.material({0.9, 0.4, 0.6}, {0.8, 0.8, 0.8}, 50)
mat6 = gr.material({0.1, 0.1, 0.1}, {0.8, 0.8, 0.8}, 50)
mat7 = gr.material({0.1, 0.1, 0.9}, {0.8, 0.8, 0.8}, 50)

-- Root Node
scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, 0}, 1)
scene_root:add_child(s1)
s1:set_material(mat1)
s1:set_texture("Assets/texture/Ball1.png")
s1:rotate('Y', 90)

s2 = gr.nh_sphere('s2', {0, 3, 0}, 1)
scene_root:add_child(s2)
s2:set_material(mat2)
s2:set_texture("Assets/texture/Ball2.png")
s2:rotate('Y', 90)

s3 = gr.nh_sphere('s3', {0, -3, 0}, 1)
scene_root:add_child(s3)
s3:set_material(mat3)
s3:set_texture("Assets/texture/Ball3.png")
s3:rotate('Y', 90)

s4 = gr.nh_sphere('s4', {0, 0, 3}, 1)
scene_root:add_child(s4)
s4:set_material(mat4)
s4:set_texture("Assets/texture/Ball4.png")
s4:rotate('Y', 90)

s5 = gr.nh_sphere('s5', {0, 0, -3}, 1)
scene_root:add_child(s5)
s5:set_material(mat5)
s5:set_texture("Assets/texture/Ball5.png")
s5:rotate('Y', 90)


-- Adjusted Lights
white_light = gr.light({-10.0, 10.0, 10.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

-- Render with adjusted camera
gr.render(scene_root, 'texture.png', 1024, 1024,
          {0, 0, 10}, {0, 0, -10}, {0, 1, 0}, 50,
          {0.3, 0.3, 0.3}, {white_light})