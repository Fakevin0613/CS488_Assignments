rootNode = gr.node('root')
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
yellow = gr.material({1.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

s0 = gr.mesh('sphere','s0')
rootNode:add_child(s0)
s0:scale(1.0, 1.0, 1.0)
s0:set_material(white)

rootNode:translate(-2.75, 0.0, -10.0)
return rootNode
