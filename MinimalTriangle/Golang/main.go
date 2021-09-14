package main

// https://kylewbanks.com/blog/tutorial-opengl-with-golang-part-1-hello-opengl

// https://github.com/go-gl/glfw/issues/140
// sudo apt-get install libgl1-mesa-dev xorg-dev
// go get -u github.com/go-gl/glfw/v3.3/glfw

// https://github.com/raedatoui/learn-opengl-golang

import (
	"fmt"
	"runtime"

	"github.com/go-gl/gl/v4.1-compatibility/gl"
	"github.com/go-gl/glfw/v3.3/glfw"
)

const (
	width  = 500
	height = 500
)

func main() {
	runtime.LockOSThread()

	window := initGlfw()
	defer glfw.Terminate()

	initOpenGL()

	for !window.ShouldClose() {
		draw(window)
	}
}

func initGlfw() *glfw.Window {
	if err := glfw.Init(); err != nil {
		panic(err)
	}

	glfw.WindowHint(glfw.Resizable, glfw.False)
	glfw.WindowHint(glfw.ContextVersionMajor, 4)
	glfw.WindowHint(glfw.ContextVersionMinor, 1)
	glfw.WindowHint(glfw.OpenGLProfile, glfw.OpenGLCompatProfile)
	glfw.WindowHint(glfw.OpenGLForwardCompatible, glfw.True)

	window, err := glfw.CreateWindow(width, height, "OpenGL Window", nil, nil)
	if err != nil {
		panic(err)
	}
	window.MakeContextCurrent()

	return window
}

func initOpenGL() {
	if err := gl.Init(); err != nil {
		panic(err)
	}

	version := gl.GoStr(gl.GetString(gl.VERSION))
	fmt.Println("OpenGL version", version)
}

func draw(window *glfw.Window) {

	gl.ClearColor(0.5, 0.5, 0.5, 1.0)

	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)

	gl.Begin(gl.TRIANGLES)
	gl.Color3f(0.0, 0.0, 1.0)
	gl.Vertex2f(0.0, 0.5)
	gl.Color3f(0.0, 1.0, 0.0)
	gl.Vertex2f(-0.667, -0.5)
	gl.Color3f(1.0, 0.0, 0.0)
	gl.Vertex2f(0.667, -0.5)
	gl.End()

	gl.Flush()

	glfw.PollEvents()
	window.SwapBuffers()
}
