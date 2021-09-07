// Hello World with OpenGL compute shader!
// I have wanted to do this for a long time and now it is done!
// This is slightly less elegant than its siblings since compute shaders
// can not deal with bytes. I chose to repackage to int on the CPU.
// This is an example, use freely.
// By Ingemar Ragnemalm 2017

#version 430
#extension GL_ARB_compute_shader : enable
//#extension GL_ARB_shader_storage_buffer : enable
#define width 16
#define height 1

// Compute shader invocations in each work group

layout(std430, binding = 6) buffer offsbuf {int offs[];};
layout(std430, binding = 5) buffer strbuf {int str[];};
layout(local_size_x=width, local_size_y=height) in;

//Kernel Program
void main()
{
        int i = int(gl_GlobalInvocationID.x);
        str[i] = str[i] + offs[i];
}
