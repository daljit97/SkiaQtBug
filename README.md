This example integrates OpenGL rendering using Skia with the Qt Quick Scene Graph on Windows. Unfortunately, there is a bug (that I was unable to fix) that introduces rendering artifacts in the Qt scenegraph.
For example, in the picture below we have a bezier curve drawn using Skia and on top of that a text string rendered using native Qt Quick. The text appears blurry and broken.

![alt text](https://imgur.com/a/jFYtFKq)
