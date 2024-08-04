#pragma once

#include "Core/BaseMacro.h"

#if HAS_OPENGL_LIB
#include "ImportOpenGL.h"
#else
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;
typedef long long GLsizeiptr;
typedef long long GLintptr;
typedef char GLchar;

#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_3D                     0x806F
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_2D_ARRAY               0x8C1A
#define GL_RENDERBUFFER                   0x8D41
#define GL_TEXTURE_CUBE_MAP_ARRAY         0x9009

#define GL_ARRAY_BUFFER                   0x8892
#define GL_UNIFORM_BUFFER                 0x8A11
#define GL_DRAW_INDIRECT_BUFFER           0x8F3F

#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPUTE_SHADER                 0x91B9
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82

#define GL_SRGB_EXT                       0x8C40
#define GL_SRGB8_EXT                      0x8C41
#define GL_SRGB_ALPHA_EXT                 0x8C42
#define GL_SRGB8_ALPHA8_EXT               0x8C43
#define GL_SLUMINANCE_ALPHA_EXT           0x8C44
#define GL_SLUMINANCE8_ALPHA8_EXT         0x8C45
#define GL_SLUMINANCE_EXT                 0x8C46
#define GL_SLUMINANCE8_EXT                0x8C47
#define GL_COMPRESSED_SRGB_EXT            0x8C48
#define GL_COMPRESSED_SRGB_ALPHA_EXT      0x8C49
#define GL_COMPRESSED_SLUMINANCE_EXT      0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA_EXT 0x8C4B
#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT  0x8C4C
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT 0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT 0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT 0x8C4F
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#define GL_COMPRESSED_LUMINANCE_LATC1_EXT 0x8C70
#define GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT 0x8C71
#define GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT 0x8C72
#define GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT 0x8C73
#define GL_COMPRESSED_RGBA_BPTC_UNORM_ARB 0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB 0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB 0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB 0x8E8F
#define GL_RGB565                         0x8D62

#define GL_HALF_FLOAT                     0x140B
#define GL_UNSIGNED_SHORT_5_6_5           0x8363
#define GL_UNSIGNED_INT_2_10_10_10_REV    0x8368
#define GL_UNSIGNED_INT_24_8              0x84FA

#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_STENCIL_INDEX1                 0x8D46
#define GL_STENCIL_INDEX4                 0x8D47
#define GL_STENCIL_INDEX8                 0x8D48
#define GL_STENCIL_INDEX16                0x8D49

#define GL_MAX_COLOR_ATTACHMENTS          0x8CDF
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_COLOR_ATTACHMENT16             0x8CF0
#define GL_COLOR_ATTACHMENT17             0x8CF1
#define GL_COLOR_ATTACHMENT18             0x8CF2
#define GL_COLOR_ATTACHMENT19             0x8CF3
#define GL_COLOR_ATTACHMENT20             0x8CF4
#define GL_COLOR_ATTACHMENT21             0x8CF5
#define GL_COLOR_ATTACHMENT22             0x8CF6
#define GL_COLOR_ATTACHMENT23             0x8CF7
#define GL_COLOR_ATTACHMENT24             0x8CF8
#define GL_COLOR_ATTACHMENT25             0x8CF9
#define GL_COLOR_ATTACHMENT26             0x8CFA
#define GL_COLOR_ATTACHMENT27             0x8CFB
#define GL_COLOR_ATTACHMENT28             0x8CFC
#define GL_COLOR_ATTACHMENT29             0x8CFD
#define GL_COLOR_ATTACHMENT30             0x8CFE
#define GL_COLOR_ATTACHMENT31             0x8CFF
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41
#endif

#include "OpenGLDefines.h"

//  π”√OpenGL 4.0+ || OpenGL ES 3.0+ 
#define RED_INTEGER  GL_RED_INTEGER
#define RG_INTEGER   GL_RG_INTEGER
#define RGB_INTEGER  GL_RGB_INTEGER
#define RGBA_INTEGER  GL_RGBA_INTEGER

namespace GL
{

FORCEINLINE void GenRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
#if HAS_OPENGL_LIB
	glGenRenderbuffers(n, renderbuffers);
#endif
}

FORCEINLINE void BindRenderbuffer(GLenum target, GLuint renderbuffer)
{
#if HAS_OPENGL_LIB
	glBindRenderbuffer(target, renderbuffer);
#endif
}

FORCEINLINE void RenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
#if HAS_OPENGL_LIB
	glRenderbufferStorage(target, internalformat, width, height);
#endif
}

FORCEINLINE void DeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
{
#if HAS_OPENGL_LIB
	glDeleteRenderbuffers(n, renderbuffers);
#endif
}

FORCEINLINE void GenTextures(GLsizei n, GLuint* textures)
{
#if HAS_OPENGL_LIB
	glGenTextures(n, textures);
#endif
}

FORCEINLINE void BindTexture(GLenum target, GLuint texture)
{
#if HAS_OPENGL_LIB
	glBindTexture(target, texture);
#endif
}

FORCEINLINE void PixelStorei(GLenum pname, GLint param)
{
#if HAS_OPENGL_LIB
	glPixelStorei(pname, param);
#endif
}

FORCEINLINE void TexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
#if HAS_OPENGL_LIB
	glTexStorage2D(target, levels, internalformat, width, height);
#endif
}

FORCEINLINE void TexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
#if HAS_OPENGL_LIB
	glTexStorage3D(target, levels, internalformat, width, height, depth);
#endif
}

FORCEINLINE void DeleteTextures(GLsizei n, const GLuint* textures)
{
#if HAS_OPENGL_LIB
	glDeleteTextures(n, textures);
#endif
}

FORCEINLINE void CompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
#if HAS_OPENGL_LIB
	glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
#endif
}

FORCEINLINE void CompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
{
#if HAS_OPENGL_LIB
	glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
#endif
}

FORCEINLINE void CompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data)
{
#if HAS_OPENGL_LIB
	glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);
#endif
}

FORCEINLINE void CompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data)
{
#if HAS_OPENGL_LIB
	glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
#endif
}

FORCEINLINE void TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)
{
#if HAS_OPENGL_LIB
	glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
#endif
}

FORCEINLINE void TexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
#if HAS_OPENGL_LIB
	glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
#endif
}

FORCEINLINE void TexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels)
{
#if HAS_OPENGL_LIB
	glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
#endif
}

FORCEINLINE void TexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
#if HAS_OPENGL_LIB
	glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
#endif
}

FORCEINLINE void GenBuffers(GLsizei n, GLuint* buffers)
{
#if HAS_OPENGL_LIB
	glGenBuffers(n, buffers);
#endif
}

FORCEINLINE void BindBuffer(GLenum target, GLuint buffer)
{
#if HAS_OPENGL_LIB
	glBindBuffer(target, buffer);
#endif
}

FORCEINLINE void BufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
#if HAS_OPENGL_LIB
	glBufferData(target, size, data, usage);
#endif
}

FORCEINLINE void BufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
#if HAS_OPENGL_LIB
	glBufferSubData(target, offset, size, data);
#endif
}

FORCEINLINE void DeleteBuffers(GLsizei n, const GLuint* buffers)
{
#if HAS_OPENGL_LIB
	glDeleteBuffers(n, buffers);
#endif
}

FORCEINLINE GLuint CreateShader(GLenum type)
{
#if HAS_OPENGL_LIB
	return glCreateShader(type);
#else
	return 0u;
#endif
}

FORCEINLINE void DeleteShader(GLuint shader)
{
#if HAS_OPENGL_LIB
	glDeleteShader(shader);
#endif
}

FORCEINLINE void ShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length)
{
#if HAS_OPENGL_LIB
	glShaderSource(shader, count, string, length);
#endif
}

FORCEINLINE void CompileShader(GLuint shader)
{
#if HAS_OPENGL_LIB
	glCompileShader(shader);
#endif
}

FORCEINLINE void GetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
#if HAS_OPENGL_LIB
	glGetShaderiv(shader, pname, params);
#endif
}

FORCEINLINE GLuint CreateProgram()
{
#if HAS_OPENGL_LIB
	return glCreateProgram();
#else
	return 0u;
#endif
}

FORCEINLINE void DeleteProgram(GLuint program)
{
#if HAS_OPENGL_LIB
	glDeleteProgram(program);
#endif
}

FORCEINLINE void AttachShader(GLuint program, GLuint shader)
{
#if HAS_OPENGL_LIB
	glAttachShader(program, shader);
#endif
}

FORCEINLINE void LinkProgram(GLuint program)
{
#if HAS_OPENGL_LIB
	glLinkProgram(program);
#endif
}

FORCEINLINE void GetProgramiv(GLuint program, GLenum pname, GLint* params)
{
#if HAS_OPENGL_LIB
	glGetProgramiv(program, pname, params);
#endif
}

FORCEINLINE void GetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
#if HAS_OPENGL_LIB
	glGetProgramInfoLog(program, bufSize, length, infoLog);
#endif
}

FORCEINLINE void BindFramebuffer(GLenum target, GLuint framebuffer)
{
#if HAS_OPENGL_LIB
	glBindFramebuffer(target, framebuffer);
#endif
}

FORCEINLINE void GenFrameBuffers(GLsizei n, GLuint* framebuffers)
{
#if HAS_OPENGL_LIB
	glGenFramebuffers(n, framebuffers);
#endif
}

FORCEINLINE void DrawArrays(GLenum mode, GLint first, GLsizei count)
{
#if HAS_OPENGL_LIB
	glDrawArrays(mode, first, count);
#endif
}

FORCEINLINE void DrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
#if HAS_OPENGL_LIB
	glDrawArraysInstanced(mode, first, count, instancecount);
#endif
}

FORCEINLINE void DrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
{
#if HAS_OPENGL_LIB
	glDrawElements(mode, count, type, indices);
#endif
}

FORCEINLINE void DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount)
{
#if HAS_OPENGL_LIB
	glDrawElementsInstanced(mode, count, type, indices, instancecount);
#endif
}

FORCEINLINE void ActiveTexture(GLenum texture)
{
#if HAS_OPENGL_LIB
	glActiveTexture(texture);
#endif
}

FORCEINLINE void Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
#if HAS_OPENGL_LIB
	glViewport(x, y, width, height);
#endif
}

FORCEINLINE void Uniformli(GLint location, GLint v0)
{
#if HAS_OPENGL_LIB
	glUniform1i(location, v0);
#endif
}

FORCEINLINE void Uniform1f(GLint location, GLfloat v0)
{
#if HAS_OPENGL_LIB
	glUniform1f(location, v0);
#endif
}

FORCEINLINE void Uniform2f(GLint location, GLfloat v0, GLfloat v1)
{
#if HAS_OPENGL_LIB
	glUniform2f(location, v0, v1);
#endif
}


FORCEINLINE void Uniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
#if HAS_OPENGL_LIB
	glUniform3f(location, v0, v1, v2);
#endif
}

FORCEINLINE void Uniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
#if HAS_OPENGL_LIB
	glUniform4f(location, v0, v1, v2, v3);
#endif
}

FORCEINLINE void Uniform1iv(GLint location, GLsizei count, const GLint* value)
{
#if HAS_OPENGL_LIB
	glUniform1iv(location, count, value);
#endif
}

FORCEINLINE void Uniform1fv(GLint location, GLsizei count, const GLfloat* value)
{
#if HAS_OPENGL_LIB
	glUniform1fv(location, count, value);
#endif
}

FORCEINLINE void Uniform2fv(GLint location, GLsizei count, const GLfloat* value)
{
#if HAS_OPENGL_LIB
	glUniform2fv(location, count, value);
#endif
}

FORCEINLINE void Uniform3fv(GLint location, GLsizei count, const GLfloat* value)
{
#if HAS_OPENGL_LIB
	glUniform3fv(location, count, value);
#endif
}

FORCEINLINE void Uniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
#if HAS_OPENGL_LIB
	glUniform4fv(location, count, value);
#endif
}

FORCEINLINE void UniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
#if HAS_OPENGL_LIB
	glUniformMatrix3fv(location, count, transpose, value);
#endif
}

FORCEINLINE void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
#if HAS_OPENGL_LIB
	glUniformMatrix4fv(location, count, transpose, value);
#endif
}


FORCEINLINE void UseProgram(GLuint program)
{
#if HAS_OPENGL_LIB
	glUseProgram(program);
#endif
}

FORCEINLINE void Flush()
{
#if HAS_OPENGL_LIB
	glFlush();
#endif
}

}
