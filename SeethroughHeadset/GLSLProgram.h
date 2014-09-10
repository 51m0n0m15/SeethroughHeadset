#pragma once

#include "includes.h"

namespace GLSLShader {
	enum GLSLShaderType {
		VERTEX, FRAGMENT, GEOMETRY,TESS_CONTROL, TESS_EVALUATION
	};
};

/**
A wrapper class for GLSL shaders, taken from the "GLSL Cookbook"
*/
class GLSLProgram
{
	private:
		int handle;
		bool linked;
		string logString;

		int getUniformLocation(const char * name );
		bool fileExists( const string & fileName );
		
	public:
		GLSLProgram();
		bool compileShaderFromFile( const char * fileName, GLSLShader::GLSLShaderType type );
		bool compileShaderFromString( const string & source, GLSLShader::GLSLShaderType type );
		bool link();
		bool validate();
		void use();

		string log();
		
		int getHandle();
		bool isLinked();
		
		void bindAttribLocation( GLuint location, const char * name);
		void bindFragDataLocation( GLuint location, const char * name );

		void setUniform(const char *name,float x,float y, float z);
		void setUniform(const char *name, const glm::vec2 & v);
		void setUniform(const char *name, const glm::vec3 & v);
		void setUniform(const char *name, const glm::vec4 & v);
		void setUniform(const char *name, const glm::mat4 & m);
		void setUniform(const char *name, const glm::mat3 & m);
		void setUniform(const char *name, float val );
		void setUniform(const char *name, int val );
		void setUniform(const char *name, bool val );
		
		void printActiveUniforms();
		void printActiveAttribs();
};
