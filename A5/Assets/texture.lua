-- Material
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

-- Root Node
scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, 0}, 3)
scene_root:add_child(s1)
s1:set_material(mat3)
s1:set_texture("Assets/texture/Ball1.png")
s1:rotate('Y', 90)

-- s2 = gr.plane('s2', {0, 0, 5}, {0, 0, 1}, 1, 1)
-- scene_root:add_child(s2)
-- s2:set_material(mat1)
-- s2:set_texture("Assets/texture/Ballll1.png")

-- Adjusted Lights
white_light = gr.light({-10.0, 10.0, 10.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

-- Render with adjusted camera
gr.render(scene_root, 'texture.png', 1024, 1024,
          {0, 0, 10}, {0, 0, -10}, {0, 1, 0}, 50,
          {0.3, 0.3, 0.3}, {white_light})