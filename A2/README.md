## To get started:
```
premake4 gmake
make
./A2
```

## Manual
The program is tested and run on gl42.student.cs

#### Assumption:
1. The near is always in front of the far plane
2. The view transformation and model transformation use the same factor to scale mouse moment
3. The gnomons will disappear if outside the viewport or near/far plane
4. The initial position is set as what the screenshot looks like
5. The clipping part is inspired from https://chaosinmotion.com/2016/05/22/3d-clipping-in-homogeneous-coordinates/comment-page-1/