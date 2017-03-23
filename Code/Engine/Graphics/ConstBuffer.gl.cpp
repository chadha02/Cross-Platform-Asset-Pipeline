#include "ConstBuffer.h"
#include "Graphics.h"
#include "../Time/Time.h"
#include "../Math/Functions.h"

bool eae6320::ConstBuffer::Init(ConstBufferType bufferType, size_t size_buff, void *i_ptr)
{
	if (bufferType == ConstBufferType::FRAME_CALL_BUFFER)
	{
		buffernumber = 0;
	}

	if (bufferType == ConstBufferType::DRAW_CALL_BUFFER)
	{
		buffernumber = 1;
	}

	if (bufferType == ConstBufferType::MATERIAL_CALL_BUFFER)
	{
		buffernumber = 2;
	}

	buffSize = size_buff;
	bool wereThereErrors = false;

	// Create a uniform buffer object and make it active
	{
		const GLsizei buffers = 1;
		glGenBuffers(buffers, &s_constantBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, s_constantBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to bind the new uniform buffer %u: %s",
					s_constantBufferId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to get an unused uniform buffer ID: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}
	
	if (buffernumber == 0)
	{
		s_constantFrameBufferData.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	}

	// Fill in the constant buffer
	// Allocate space and copy the constant data into the uniform buffer
	{
		const GLenum usage = GL_DYNAMIC_DRAW;	// The buffer will be modified frequently and used to draw
		glBufferData(GL_UNIFORM_BUFFER, buffSize, reinterpret_cast<const GLvoid*>(i_ptr), usage);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to allocate the new uniform buffer %u: %s",
				s_constantBufferId, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			goto OnExit;
		}
	}

OnExit:

	return !wereThereErrors;
}

void eae6320::ConstBuffer::Bind()
{
	//const GLuint bindingPointAssignedInShader = 0;
	glBindBufferBase(GL_UNIFORM_BUFFER, buffernumber, s_constantBufferId);
	EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
}

void eae6320::ConstBuffer::Update(void * i_ptr)
{
	//s_constantFrameBufferData.g_elapsedSecondCount_total = eae6320::Time::GetElapsedSecondCount_total();
	// Make the uniform buffer active
	{
		glBindBuffer(GL_UNIFORM_BUFFER, s_constantBufferId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	// Copy the updated memory to the GPU
	{
		GLintptr updateAtTheBeginning = 0;
		GLsizeiptr updateTheEntireBuffer = static_cast<GLsizeiptr>(buffSize);
		glBufferSubData(GL_UNIFORM_BUFFER, updateAtTheBeginning, updateTheEntireBuffer,i_ptr);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
}

void  eae6320::ConstBuffer::Cleanup()
{
	bool wereThereErrors = false; 
	if (s_constantBufferId != 0)
	{
		const GLsizei bufferCount = 1;
		glDeleteBuffers(bufferCount, &s_constantBufferId);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			wereThereErrors = true;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the constant buffer: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		s_constantBufferId = 0;
	}
}