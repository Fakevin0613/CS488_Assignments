rootNode = gr.node('root')
rootNode:translate(0.0, 0.0, -3.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0,0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0,1.0, 1.0}, {0.5, 0.5, 0.5}, 10)

torso = gr.mesh('sphere', 'torso')
torso:set_material(white)
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

rightUpperArmJoint = gr.joint('rightUpperArmJoint', {-60, 0, 60}, {-30, 0, 30});
rightUpperArmNode:add_child(rightUpperArmJoint)

rightUpperArm = gr.mesh('sphere', 'rightUpperArm')
rightUpperArmJoint:add_child(rightUpperArm)
rightUpperArm:set_material(white)
rightUpperArm:translate(0.6, 0.75, 0.0)
rightUpperArm:scale(1.5, 0.4, 0.4)
rightUpperArm:rotate('z', -30)

rightLowerArmNode = gr.node('rightLowerArmNode')
rightUpperArmNode:add_child(rightLowerArmNode)
rightLowerArmNode:scale(1.0 / 1.5, 1.0 / 0.4, 1.0 / 0.4)
rightLowerArmNode:translate(2.0, -1.0, 0.0)


rightLowerArmJoint = gr.joint('rightLowerArmJoint', {-40, 0, 10}, {0, 0, 0});
rightLowerArmNode:add_child(rightLowerArmJoint)

rightLowerArm = gr.mesh('sphere', 'rightLowerArm')
rightLowerArmJoint:add_child(rightLowerArm)
rightLowerArm:set_material(white)
rightLowerArm:translate(0.5, 0.0, 0.0)
rightLowerArm:scale(1.4, 0.15, 0.15)
rightLowerArm:rotate('z', -130)


-----------------Left Arm-----------------
leftUpperArmNode = gr.node('leftUpperArmNode')
torso:add_child(leftUpperArmNode)
leftUpperArmNode:scale(1.0, 0.5, 1.0)
leftUpperArmNode:translate(-0.4, 0.75, 0.0)

leftUpperArmJoint = gr.joint('leftUpperArmJoint', {-60, 0, 60}, {-30, 0, 30});
leftUpperArmNode:add_child(leftUpperArmJoint)

leftUpperArm = gr.mesh('sphere', 'leftUpperArm')
leftUpperArmJoint:add_child(leftUpperArm)
leftUpperArm:set_material(white)
leftUpperArm:translate(-0.6, 0.75, 0.0)
leftUpperArm:scale(1.5, 0.4, 0.4)
leftUpperArm:rotate('z', 30)

-----------------Right Leg-----------------
rightUpperLegNode = gr.node('rightUpperLegNode')
torso:add_child(rightUpperLegNode)
rightUpperLegNode:scale(1.0, 0.5, 1.0)
rightUpperLegNode:translate(0.3, -0.75, 0.0)

rightUpperLegJoint = gr.joint('rightUpperLegJoint', {-60, 0, 60}, {-30, 0, 30});
rightUpperLegNode:add_child(rightUpperLegJoint)

rightUpperLeg = gr.mesh('sphere', 'rightUpperLeg')
rightUpperLegJoint:add_child(rightUpperLeg)
rightUpperLeg:set_material(white)
rightUpperLeg:translate(0.4, -0.75, 0.0)
rightUpperLeg:scale(0.5, 1.8, 0.5)
rightUpperLeg:rotate('z', 10)

-----------------Left Leg-----------------
leftUpperLegNode = gr.node('leftUpperLegNode')
torso:add_child(leftUpperLegNode)
leftUpperLegNode:scale(1.0, 0.5, 1.0)
leftUpperLegNode:translate(-0.3, -0.75, 0.0)

leftUpperLegJoint = gr.joint('leftUpperLegJoint', {-60, 0, 60}, {-30, 0, 30});
leftUpperLegNode:add_child(leftUpperLegJoint)

leftUpperLeg = gr.mesh('sphere', 'leftUpperLeg')
leftUpperLegJoint:add_child(leftUpperLeg)
leftUpperLeg:set_material(white)
leftUpperLeg:translate(-0.4, -0.75, 0.0)
leftUpperLeg:scale(0.5, 1.8, 0.5)
leftUpperLeg:rotate('z', -10)

return rootNode;
