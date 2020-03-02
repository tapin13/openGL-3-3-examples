# OpenGL 3.3 Tutorial - Prevent z-fighting

Tutorial from https://learnopengl.com/Advanced-OpenGL/Depth-testing

A common visual artifact might occur when two planes or triangles are so closely aligned to each other that the depth buffer does not have enough precision to figure out which one of the two shapes is in front of the other.

The first and most important trick is never place objects too close to each other in a way that some of their triangles closely overlap.

![alt text](https://github.com/tapin13/openGL-3-3-examples/blob/master/tutorialXIV4_z-fighting/Screenshot.png)

![alt text](https://github.com/tapin13/openGL-3-3-examples/blob/master/tutorialXIV4_z-fighting/Screenshot1.png)

![alt text](https://github.com/tapin13/openGL-3-3-examples/blob/master/tutorialXIV4_z-fighting/Screenshot2.png)


