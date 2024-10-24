rootNode = gr.node('root')
rootNode:translate(0.0, 0.0, -3.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0,0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0,1.0, 1.0}, {0.5, 0.5, 0.5}, 10)

torso = gr.mesh('sphere', 'torso')
torso:set_material(green)
torso:scale(0.5, 1.0, 0.5)
rootNode:add_child(torso)

arm = gr.mesh('sphere', 'arm')
rootNode:add_child(arm)
arm:set_material(blue)
arm:scale(0.6, 0.3, 0.5)
arm:translate(0.0, 0.9, 0.0)

butt = gr.mesh('sphere', 'butt')
rootNode:add_child(butt)
butt:set_material(blue)
butt:scale(0.7, 0.4, 0.5)
butt:translate(0.0, -0.9, 0.0)


-----------------Head-----------------
neckNode = gr.node('neck')
torso:add_child(neckNode)
neckNode:scale(1.0, 0.5, 1.0)
neckNode:translate(0.0, 1.0, 0.0)

neckJoint = gr.joint('neckJoint', {-20, 0, 60}, {-120, 0, 120});
neckNode:add_child(neckJoint)

head = gr.mesh('sphere', 'head')
neckJoint:add_child(head)
head:set_material(red)
head:scale(1.0, 1.0, 0.5)
head:translate(0.0, 1.3, 0.0)

-----------------Right Arm-----------------
rightUpperArmNode = gr.node('rightUpperArmNode')
torso:add_child(rightUpperArmNode)
rightUpperArmNode:scale(1.0, 0.5, 1.0)
rightUpperArmNode:translate(0.4, 0.75, 0.0)

rightUpperArmJoint = gr.joint('rightUpperArmJoint', {-45, 0, 45}, {-30, 0, 30});
rightUpperArmNode:add_child(rightUpperArmJoint)

rightUpperArm = gr.mesh('sphere', 'rightUpperArm')
rightUpperArmJoint:add_child(rightUpperArm)
rightUpperArm:set_material(green)
rightUpperArm:translate(0.6, 0.75, 0.0)
rightUpperArm:scale(1.5, 0.4, 0.4)
rightUpperArm:rotate('z', -30)

spearNode = gr.node('spearNode')
rightUpperArmNode:add_child(spearNode)
spearNode:scale(1.0 / 1.5, 1.0 / 0.4, 1.0 / 0.4)
spearNode:translate(2.0, -1.0, 0.0)


spearJoint = gr.joint('spearJoint', {-40, 0, 40}, {0, 0, 0});
spearNode:add_child(spearJoint)

spear = gr.mesh('cube', 'spear')
spearJoint:add_child(spear)
spear:set_material(white)
spear:translate(0.5, 0.0, 0.0)
spear:scale(0.15, 3.5, 0.15)


-----------------Left Arm-----------------
leftUpperArmNode = gr.node('leftUpperArmNode')
torso:add_child(leftUpperArmNode)
leftUpperArmNode:scale(1.0, 0.5, 1.0)
leftUpperArmNode:translate(-0.4, 0.75, 0.0)

leftUpperArmJoint = gr.joint('leftUpperArmJoint', {-45, 0, 45}, {-30, 0, 30});
leftUpperArmNode:add_child(leftUpperArmJoint)

leftUpperArm = gr.mesh('sphere', 'leftUpperArm')
leftUpperArmJoint:add_child(leftUpperArm)
leftUpperArm:set_material(green)
leftUpperArm:translate(-0.6, 0.75, 0.0)
leftUpperArm:scale(1.5, 0.4, 0.4)
leftUpperArm:rotate('z', 30)

shieldNode = gr.node('shieldNode')
leftUpperArmNode:add_child(shieldNode)
shieldNode:scale(1.0 / 1.5, 1.0 / 0.4, 1.0 / 0.4)
shieldNode:translate(-2.0, -1.0, 0.0)

shieldJoint = gr.joint('shieldJoint', {-40, 0, 40}, {0, 0, 0});
shieldNode:add_child(shieldJoint)

shield = gr.mesh('cube', 'shield')
shieldJoint:add_child(shield)
shield:set_material(white)
shield:translate(-0.5, 0.0, 0.0)
shield:scale(0.15, 3.5, 0.15)


-----------------Right Leg-----------------
rightUpperLegNode = gr.node('rightUpperLegNode')
torso:add_child(rightUpperLegNode)
rightUpperLegNode:scale(1.0, 0.5, 1.0)
rightUpperLegNode:translate(0.3, -0.75, 0.0)

rightUpperLegJoint = gr.joint('rightUpperLegJoint', {-60, 0, 60}, {-30, 0, 30});
rightUpperLegNode:add_child(rightUpperLegJoint)

rightUpperLeg = gr.mesh('sphere', 'rightUpperLeg')
rightUpperLegJoint:add_child(rightUpperLeg)
rightUpperLeg:set_material(green)
rightUpperLeg:translate(0.4, -0.75, 0.0)
rightUpperLeg:scale(0.5, 1.8, 0.5)
rightUpperLeg:rotate('z', 10)

rightShoeNode = gr.node('rightShoeNode')
rightUpperLegNode:add_child(rightShoeNode)
rightShoeNode:translate(0.8, -2.7, 0.0)

rightShoeJoint = gr.joint('rightShoeJoint', {-30, 0, 30}, {0, 0, 0});
rightShoeNode:add_child(rightShoeJoint)

rightShoe = gr.mesh('cube', 'rightShoe')
rightShoeJoint:add_child(rightShoe)
rightShoe:set_material(green)
rightShoe:translate(0.0, -0.5, 0.0)
rightShoe:scale(0.8, 0.4, 5.0)

-----------------Left Leg-----------------
leftUpperLegNode = gr.node('leftUpperLegNode')
torso:add_child(leftUpperLegNode)
leftUpperLegNode:scale(1.0, 0.5, 1.0)
leftUpperLegNode:translate(-0.3, -0.75, 0.0)

leftUpperLegJoint = gr.joint('leftUpperLegJoint', {-60, 0, 60}, {-30, 0, 30});
leftUpperLegNode:add_child(leftUpperLegJoint)

leftUpperLeg = gr.mesh('sphere', 'leftUpperLeg')
leftUpperLegJoint:add_child(leftUpperLeg)
leftUpperLeg:set_material(green)
leftUpperLeg:translate(-0.4, -0.75, 0.0)
leftUpperLeg:scale(0.5, 1.8, 0.5)
leftUpperLeg:rotate('z', -10)

leftShoeNode = gr.node('leftShoeNode')
leftUpperLegNode:add_child(leftShoeNode)
leftShoeNode:translate(-0.8, -2.7, 0.0)

leftShoeJoint = gr.joint('leftShoeJoint', {-30, 0, 30}, {0, 0, 0});
leftShoeNode:add_child(leftShoeJoint)

leftShoe = gr.mesh('cube', 'leftShoe')
leftShoeJoint:add_child(leftShoe)
leftShoe:set_material(green)
leftShoe:translate(0.0, -0.5, 0.0)
leftShoe:scale(0.8, 0.4, 5.0)


return rootNode;
