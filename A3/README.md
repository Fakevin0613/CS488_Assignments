## To get started:
```
premake4 gmake
make
./A3
./A3 Assets/puppet.lua
```

## Manual
The program is tested and run on gl42.student.cs

#### Assumption:
1. For middle mouse button dragging, since the instruction is unclear, moving the mouse in the y direction will change the angle along the x axis. For example knocking heads, raising legs in front.
2. Right mouse button will make the head turn to left or right, by moving the mouse in the x direction. Therefore, if combining wiht middle mouse button draging, the head can turn left/right, or up/down at same time.

#### Note:
1. The undo/redo is not implemented becauese of no time
2. The JointNode.cpp has two extra variable and two rotate function added


----------------------------head----------------------
----------------------------arm-----------------------
-----------------leftArm-----|------rightarm----------
-------------------|--------torso------|--------------
------------------Spear------|--------Spear-----------
-----------------------------butt---------------------
----------------------------/-----\-------------------
-----------------------leftLeg----rightLeg------------
-------------------------/----------\-----------------
---------------------leftShoe-------RightShoe---------

Each component except for arm, torso and butt consist of a node, joint and mesh


