-- Material
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)

-- Root Node
scene_root = gr.node('root')

-- Torus Primitive
s1 = gr.torus('s1', {0, 0, 0}, 1, 2)
scene_root:add_child(s1)
s1:set_material(mat3)
s1:rotate('X', -30)
s1:rotate('Y', 30)

-- Adjusted Lights
white_light = gr.light({-10.0, 10.0, 10.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

-- Render with adjusted camera
gr.render(scene_root, 'primitive-torus.png', 512, 512,
          {0, 0, 10}, {0, 0, -10}, {0, 1, 0}, 50,
          {0.3, 0.3, 0.3}, {white_light})