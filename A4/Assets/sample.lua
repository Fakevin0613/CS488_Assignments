gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
red = gr.material({1.0, 0.0, 0.0}, {0.8, 0.1, 0.1}, 25)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.8, 0.1}, 25)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.8}, 25)
yellow = gr.material({1.0, 1.0, 0.0}, {0.8, 0.8, 0.1}, 25)
purple = gr.material({0.5, 0.0, 0.5}, {0.7, 0.1, 0.7}, 25)
orange = gr.material({1.0, 0.5, 0.0}, {0.9, 0.4, 0.1}, 25)
gray = gr.material({0.5, 0.5, 0.5}, {0.2, 0.2, 0.2}, 10)
charcoal = gr.material({0.2, 0.2, 0.2}, {0.1, 0.1, 0.1}, 10)
brown = gr.material({0.6, 0.4, 0.2}, {0.3, 0.2, 0.1}, 10)
dark_green = gr.material({0.1, 0.4, 0.1}, {0.0, 0.0, 0.0}, 0)
sand = gr.material({0.9, 0.8, 0.6}, {0.7, 0.7, 0.5}, 10)
stone = gr.material({0.5, 0.5, 0.5}, {0.2, 0.2, 0.2}, 5)
clay = gr.material({0.7, 0.4, 0.3}, {0.6, 0.3, 0.2}, 15) 
white = gr.material({1.0, 1.0, 1.0}, {0.9, 0.9, 0.9}, 10)

scene = gr.node('scene')
scene:rotate('X', 20)

-- UFO
ufo = gr.node('ufo')
scene:add_child(ufo)
ufo:translate(5, 14, 25)

ufo_body = gr.sphere('ufo_body')
ufo:add_child(ufo_body)
ufo_body:set_material(orange)
ufo_body:scale(6, 1, 6)
ufo_body:translate(0, 0, 0)

ufo_head = gr.sphere('ufo_head')
ufo:add_child(ufo_head)
ufo_head:set_material(orange)
ufo_head:scale(2.5, 2, 2.5)
ufo_head:translate(0, 1, 0)

-- cow
cow = gr.mesh('cow', 'Assets/cow.obj')
scene:add_child(cow)
cow:set_material(brown)
cow:scale(0.5, 0.5, 0.5)
cow:rotate('Z', 120)
cow:translate(5, 8, 25)

-- sphere
poly1 = gr.mesh( 'poly1', 'Assets/dodeca.obj' )
scene:add_child(poly1)
poly1:translate(-5, 0, 25)
poly1:set_material(clay)

-- sphere
poly2 = gr.mesh( 'poly2', 'Assets/dodeca.obj' )
scene:add_child(poly2)
poly2:translate(12, 0, 10)
poly2:set_material(clay)

-- sphere
poly3 = gr.mesh( 'poly3', 'Assets/dodeca.obj' )
scene:add_child(poly3)
poly3:translate(0, 0, 0)
poly3:set_material(clay)

-- sphere
poly4 = gr.mesh( 'poly4', 'Assets/dodeca.obj' )
scene:add_child(poly4)
poly4:translate(8, 0, 18)
poly4:set_material(clay)

-- mountainA
mountainA = gr.nh_sphere('mountainA', {-23, -15, -35}, 25)
scene:add_child(mountainA)
mountainA:set_material(stone)

-- mountainB
mountainB = gr.nh_sphere('mountainB', {7, -15, -48}, 35)
scene:add_child(mountainB)
mountainB:set_material(stone)

-- mountainC
mountainC = gr.nh_sphere('mountainC', {25, -15, -30}, 30)
scene:add_child(mountainC)
mountainC:set_material(stone)

-- cloudA
cloudA = gr.node("cloudA")
scene:add_child(cloudA)
cloudA:translate(0, 18, -15)

cloud1 = gr.sphere('cloud1')
cloudA:add_child(cloud1)
cloud1:set_material(white)
cloud1:scale(6, 1, 3)
cloud1:translate(0, 0, 0)

cloud2 = gr.sphere('cloud1')
cloudA:add_child(cloud2)
cloud2:set_material(white)
cloud2:scale(5, 1, 4)
cloud2:translate(2, 1, -1)

-- cloudB
cloudB = gr.node("cloudB")
scene:add_child(cloudB)
cloudB:translate(-17, 14, -3)

cloud3 = gr.sphere('cloud3')
cloudB:add_child(cloud3)
cloud3:set_material(white)
cloud3:scale(7, 1.5, 3)
cloud3:translate(0, 0, 0)

cloud4 = gr.sphere('cloud4')
cloudB:add_child(cloud4)
cloud4:set_material(white)
cloud4:scale(6, 1.2, 3)
cloud4:translate(2, 1, -1)

-- cloudC
cloudC = gr.node("cloudC")
scene:add_child(cloudC)
cloudC:translate(11, 15, -5)

cloud5 = gr.sphere('cloud5')
cloudC:add_child(cloud5)
cloud5:set_material(white)
cloud5:scale(6.5, 1.3, 4)
cloud5:translate(0, 0, 0)

cloud6 = gr.sphere('cloud6')
cloudC:add_child(cloud6)
cloud6:set_material(white)
cloud6:scale(6, 1.4, 3)
cloud6:translate(-1, 1, 1)

-- the tree
tree = gr.node('tree')
scene:add_child(tree)
tree:translate(10, 0, 10)
tree:rotate('Y', 180)

p1 = gr.cube('p1')
tree:add_child(p1)
p1:set_material(charcoal)
p1:scale(0.8, 10, 0.8)
p1:translate(0, 0, 0)

s = gr.sphere('s')
tree:add_child(s)
s:set_material(dark_green)
s:scale(3, 3, 3)
s:translate(0, 8, 0)

for i = 1, 6 do
	an_tree = gr.node('tree' .. tostring(i))
	an_tree:rotate('Y', (i-1) * 60)
	scene:add_child(an_tree)
	an_tree:add_child(tree)
end

-- the floor
plane = gr.mesh( 'plane', 'Assets/plane.obj' )
scene:add_child(plane)
plane:set_material(stone)
plane:scale(30, 30, 30)


-- Create a human figure
human = gr.node('human')
scene:add_child(human)
human:translate(5, 2, 25)

-- Torso
torso = gr.cube('torso')
human:add_child(torso)
torso:set_material(red)
torso:scale(1, 2, 0.5)

-- Head
head = gr.sphere('head')
human:add_child(head)
head:set_material(blue)
head:scale(0.5, 0.5, 0.5)
head:translate(0.5, 2.5, 0.25)

-- Left Arm
left_arm = gr.cube('left_arm')
human:add_child(left_arm)
left_arm:set_material(blue)
left_arm:scale(0.3, 1, 0.3)
left_arm:translate(-0.3, 1, 0.1)

-- Right Arm
right_arm = gr.cube('right_arm')
human:add_child(right_arm)
right_arm:set_material(blue)
right_arm:scale(0.3, 1, 0.3)
right_arm:translate(1, 1, 0.1)

-- Left Leg
left_leg = gr.cube('left_leg')
human:add_child(left_leg)
left_leg:set_material(blue)
left_leg:scale(0.4, 1, 0.4)
left_leg:translate(0.1, -1, 0.1)

-- Right Leg
right_leg = gr.cube('right_leg')
human:add_child(right_leg)
right_leg:set_material(blue)
right_leg:scale(0.4, 1, 0.4)
right_leg:translate(0.6, -1, 0.1)


-- The lights
l1 = gr.light({400, 400, 400}, {0.8, 0.8, 0.8}, {1, 0, 0})
gr.render(scene, 'sample.png', 512, 512,
	  {0, 0, 50}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})