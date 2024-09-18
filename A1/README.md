## To get started:
```
premake4 gmake
make
./A0
```

## Manual
The program is tested and run on gl42.student.cs

#### Assumption:
1. Dig (Create new Maze) will create an entirely new maze, and move the avatar to starting point. but anything else such as color, rotation, scaling,etc should not change
2. Reset will reset the color, rotation, scalling, grid height, etc. and the maze and avatar will be reset to the state when it is just created (Dig).
3. The destruction made by "shift" movement of avatar will be restored by Reset
4. The speed of persistence is constant throughout the rotation.
5. The floor is not created outside the maze (the ring of cells)
6. The avatar is travelling one cell per key press.
7. The avatar will not travel out of maze.
8. The User can play the maze once the open the program.
9. The avatar is created after inspired by https://piazza.com/class/lza5y6kauyf4os/post/30
#### Bonus Feature:
1. Smooth animate the motion of avatar when traveling through the maze.