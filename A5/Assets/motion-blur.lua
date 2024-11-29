-- Material
mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

-- Root Node
scene_root = gr.node('root')

s1 = gr.moving_sphere('s1', {2, 2, 0}, 0.75, {-4, -4, 0})
scene_root:add_child(s1)
s1:set_material(mat3)

s3 = gr.moving_sphere('s1', {-2, 1, 0}, 0.6, {3, 2, 0})
scene_root:add_child(s3)
s3:set_material(mat3)

s4 = gr.moving_sphere('s1', {3, -1, 0}, 0.6, {-5, 2, 0})
scene_root:add_child(s4)
s4:set_material(mat3)

-- Adjusted Lights
white_light = gr.light({-10.0, 10.0, 10.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

-- Render with adjusted camera
gr.render(scene_root, 'motion-blur.png', 1024, 1024,
          {0, 0, 10}, {0, 0, -10}, {0, 1, 0}, 50,
          {0.3, 0.3, 0.3}, {white_light})