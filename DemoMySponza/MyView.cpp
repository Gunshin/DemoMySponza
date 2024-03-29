#include "MyView.hpp"
#include <SceneModel/SceneModel.hpp>
#include <tygra/FileHelper.hpp>
#include <tsl/primitives.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>
#include <tygra/Window.hpp>
#include "MyController.hpp"


glm::vec3 ConvVec3(tsl::Vector3 &vec_);

//$(SolutionDir)demo\

MyView::
MyView()
{
}

MyView::
~MyView()
{
}

void MyView::
setScene(std::shared_ptr<const SceneModel::Context> scene)
{
	scene_ = scene;
}

void MyView::
windowViewWillStart(std::shared_ptr<tygra::Window> window)
{
	assert(scene_ != nullptr);

	std::cout << "----------------------------------------------------------------------------------" << std::endl;
	std::cout << "File:        es3-kepler/FXAA/assets/shaders/FXAA.vert" << std::endl;
	std::cout << "SDK Version: v2.0 " << std::endl;
	std::cout << "Email:       gameworks@nvidia.com" << std::endl;
	std::cout << "Site:        http://developer.nvidia.com/" << std::endl;
	std::cout << "" << std::endl;
	std::cout << "Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "Redistribution and use in source and binary forms, with or without" << std::endl;
	std::cout << "modification, are permitted provided that the following conditions" << std::endl;
	std::cout << "are met:" << std::endl;
	std::cout << " * Redistributions of source code must retain the above copyright" << std::endl;
	std::cout << "   notice, this list of conditions and the following disclaimer." << std::endl;
	std::cout << " * Redistributions in binary form must reproduce the above copyright" << std::endl;
	std::cout << "   notice, this list of conditions and the following disclaimer in the" << std::endl;
	std::cout << "   documentation and/or other materials provided with the distribution." << std::endl;
	std::cout << " * Neither the name of NVIDIA CORPORATION nor the names of its" << std::endl;
	std::cout << "   contributors may be used to endorse or promote products derived" << std::endl;
	std::cout << "   from this software without specific prior written permission." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY" << std::endl;
	std::cout << "EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE" << std::endl;
	std::cout << "IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR" << std::endl;
	std::cout << "PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR" << std::endl;
	std::cout << "CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL," << std::endl;
	std::cout << "EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO," << std::endl;
	std::cout << "PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR" << std::endl;
	std::cout << "PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY" << std::endl;
	std::cout << "OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT" << std::endl;
	std::cout << "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE" << std::endl;
	std::cout << "OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "----------------------------------------------------------------------------------" << std::endl;
	std::cout << "\n\n\n" << std::endl;

	std::cout << "To turn DoF rendering off/on, press 'T'" << std::endl;
	std::cout << "To turn Shadows rendering off/on, press 'Y'" << std::endl;
	std::cout << "To turn FXAA rendering off/on, press 'U'" << std::endl;

	std::cout << "\n\n\n" << std::endl;

	GenerateShaderPrograms();

	SetupSSBOS();

	SceneModel::GeometryBuilder builder = SceneModel::GeometryBuilder();
	std::vector<SceneModel::Mesh> meshes = builder.getAllMeshes();
	GenerateMeshes(meshes);

	vboInstances.resize(meshes.size());
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{

		std::vector<SceneModel::InstanceId> ids = scene_->getInstancesByMeshId(meshes[i].getId());

		vboInstances[i] = VBO([this, i, ids](GLuint bufferID_) -> bool
		{
			std::vector<InstanceData> data;
			for (unsigned int j = 0; j < ids.size(); ++j)
			{
				InstanceData instance;
				instance.positionData = scene_->getInstanceById(ids[j]).getTransformationMatrix();
				instance.materialDataIndex = static_cast<GLint>(mapMaterialIndex[scene_->getInstanceById(ids[j]).getMaterialId()]);
				data.push_back(instance);
			}

			glBindBuffer(GL_ARRAY_BUFFER, bufferID_);
			glBufferData(GL_ARRAY_BUFFER,
				data.size() * sizeof(InstanceData),
				data.data(),
				GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			vboInstances[i].SetSize(data.size());

			return true;
		});
		vboInstances[i].GenerateBuffer();

		unsigned int offset = 0;

		glGenVertexArrays(1, &loadedMeshes[i].mesh.vao);
		glBindVertexArray(loadedMeshes[i].mesh.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer.GetElementVBOID());
		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.GetVertexVBOID());

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(offset));
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
			sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(offset));
		offset += sizeof(glm::vec3);

		unsigned int instanceOffset = 0;
		glBindBuffer(GL_ARRAY_BUFFER, vboInstances[i].GetVBOID());

		for (int a = 2; a < 6; ++a)
		{
			glEnableVertexAttribArray(a);
			glVertexAttribPointer(a, 3, GL_FLOAT, GL_FALSE,
				sizeof(InstanceData), TGL_BUFFER_OFFSET(instanceOffset));
			glVertexAttribDivisor(a, 1);
			instanceOffset += sizeof(glm::vec3);
		}

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE,
			sizeof(InstanceData), TGL_BUFFER_OFFSET(instanceOffset));
		glVertexAttribDivisor(6, 1);
		instanceOffset += sizeof(GLint);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

	}

	// set up light vao since it uses a different channel layout
	{
		vboPointLight = VBO([this](GLuint bufferID_) -> bool
		{
			std::vector<SceneModel::PointLight> sceneLights = scene_->getAllPointLights();
			std::vector<PointLightData> data;
			data.resize(sceneLights.size());
			for (unsigned int i = 0; i < sceneLights.size(); ++i)
			{
				PointLightData light;
				light.position = sceneLights[i].getPosition();
				light.range = sceneLights[i].getRange();
				light.intensity = sceneLights[i].getIntensity();
				data[i] = light;
			}

			glBindBuffer(GL_ARRAY_BUFFER, bufferID_);
			glBufferData(GL_ARRAY_BUFFER,
				data.size() * sizeof(PointLightData),
				data.data(),
				GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			return true;
		});
		vboPointLight.GenerateBuffer();

		unsigned int offset = 0;

		glGenVertexArrays(1, &pointLightMesh.vao);
		glBindVertexArray(pointLightMesh.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer.GetElementVBOID());
		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.GetVertexVBOID());

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(offset));
		offset += sizeof(glm::vec3);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
			sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(offset));
		offset += sizeof(glm::vec3);

		unsigned int instanceOffset = 0;
		glBindBuffer(GL_ARRAY_BUFFER, vboPointLight.GetVBOID());

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(PointLightData), TGL_BUFFER_OFFSET(instanceOffset));
		glVertexAttribDivisor(2, 1);
		instanceOffset += sizeof(glm::vec3);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
			sizeof(PointLightData), TGL_BUFFER_OFFSET(instanceOffset));
		glVertexAttribDivisor(3, 1);
		instanceOffset += sizeof(float);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
			sizeof(PointLightData), TGL_BUFFER_OFFSET(instanceOffset));
		glVertexAttribDivisor(4, 1);
		instanceOffset += sizeof(glm::vec3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

	}

	// set up spot light buffer
	//{
	//	vboSpotLight = VBO([this](GLuint bufferID_) -> bool
	//	{
	//		std::vector<SceneModel::SpotLight> sceneLights = scene_->getAllSpotLights();
	//		std::vector<SpotLightData> data;
	//		data.resize(sceneLights.size());
	//		for (unsigned int i = 0; i < sceneLights.size(); ++i)
	//		{
	//			SpotLightData light;
	//			light.position = sceneLights[i].getPosition();
	//			light.range = sceneLights[i].getRange();
	//			light.intensity = sceneLights[i].getIntensity();
	//			light.coneAngleDegrees = sceneLights[i].getConeAngleDegrees() / 2;
	//			light.direction = sceneLights[i].getDirection();
	//			data[i] = light;
	//		}

	//		glBindBuffer(GL_ARRAY_BUFFER, bufferID_);
	//		glBufferData(GL_ARRAY_BUFFER,
	//			data.size() * sizeof(SpotLightData),
	//			data.data(),
	//			GL_STATIC_DRAW);
	//		glBindBuffer(GL_ARRAY_BUFFER, 0);

	//		return true;
	//	});
	//	vboSpotLight.GenerateBuffer();

	//	unsigned int offset = 0;

	//	glGenVertexArrays(1, &spotLightMesh.vao);
	//	glBindVertexArray(spotLightMesh.vao);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer.GetElementVBOID());
	//	glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.GetVertexVBOID());

	//	glEnableVertexAttribArray(0); // position
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
	//		sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(offset));
	//	offset += sizeof(glm::vec3);

	//	glEnableVertexAttribArray(1); // normal
	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
	//		sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(offset));
	//	offset += sizeof(glm::vec3);

	//	unsigned int instanceOffset = 0;
	//	glBindBuffer(GL_ARRAY_BUFFER, vboSpotLight.GetVBOID());

	//	glEnableVertexAttribArray(2); // light position
	//	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
	//		sizeof(SpotLightData), TGL_BUFFER_OFFSET(instanceOffset));
	//	glVertexAttribDivisor(2, 1);
	//	instanceOffset += sizeof(glm::vec3);

	//	glEnableVertexAttribArray(3); // half cone angle
	//	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
	//		sizeof(SpotLightData), TGL_BUFFER_OFFSET(instanceOffset));
	//	glVertexAttribDivisor(3, 1);
	//	instanceOffset += sizeof(float);

	//	glEnableVertexAttribArray(4); // light direction
	//	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
	//		sizeof(SpotLightData), TGL_BUFFER_OFFSET(instanceOffset));
	//	glVertexAttribDivisor(4, 1);
	//	instanceOffset += sizeof(glm::vec3);

	//	glEnableVertexAttribArray(5); // range
	//	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE,
	//		sizeof(SpotLightData), TGL_BUFFER_OFFSET(instanceOffset));
	//	glVertexAttribDivisor(5, 1);
	//	instanceOffset += sizeof(float);

	//	glEnableVertexAttribArray(6); // intensity
	//	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE,
	//		sizeof(SpotLightData), TGL_BUFFER_OFFSET(instanceOffset));
	//	glVertexAttribDivisor(6, 1);
	//	instanceOffset += sizeof(glm::vec3);

	//	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//	glBindVertexArray(0);

	//}
	// finalize spotlights
	{

		glGenVertexArrays(1, &spotLightMesh.vao);
		glBindVertexArray(spotLightMesh.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer.GetElementVBOID());
		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.GetVertexVBOID());

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(0));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
			sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(sizeof(glm::vec3)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	// finalise the square
	{
		glGenVertexArrays(1, &globalLightMesh.vao);
		glBindVertexArray(globalLightMesh.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer.GetElementVBOID());
		glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.GetVertexVBOID());

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(0));

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
			sizeof(MeshBuffer::Vertex), TGL_BUFFER_OFFSET(sizeof(glm::vec3)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glGenFramebuffers(1, &gbufferFBO);
	glGenTextures(1, &depthStencilTO);
	glGenTextures(3, gbufferTO);

	glGenFramebuffers(1, &lbufferFBO);
	glGenTextures(1, &lbufferTO);

	glGenFramebuffers(1, &shadowDepthFBO);
	glGenTextures(1, &shadowDepthTO);

	glGenFramebuffers(1, &depthPrePassFBO);
	glGenTextures(1, &depthPrePassTO);

	glGenFramebuffers(1, &depthHorizontalPassFBO);
	glGenTextures(1, &depthHorizontalPassTO);

	glGenFramebuffers(1, &depthVerticalPassFBO);
	glGenTextures(1, &depthVerticalPassTO);

	glGenFramebuffers(1, &fxaaFBO);
	glGenRenderbuffers(1, &fxaaColourRBO);

	int rollingAverage = 20;
	std::vector<float> temp;

	gbufferTimes.resize(rollingAverage);
	gbufferTimer = std::make_shared<QueryTimer>([this, rollingAverage](GLuint64 startTime_, GLuint64 endTime_)->void
	{
		for (unsigned int i = 0; i < gbufferTimes.size() - 1; ++i)
		{
			gbufferTimes[i] = gbufferTimes[i + 1];
		}
		gbufferTimes[rollingAverage - 1] = endTime_ - startTime_;

		GLuint64 average = 0;
		for (unsigned int i = 0; i < gbufferTimes.size(); ++i)
		{
			average += gbufferTimes[i];
		}

		average /= gbufferTimes.size();

		logger.Log(std::to_string(average));
	});

	backgroundTimes.resize(rollingAverage);
	backgroundTimer = std::make_shared<QueryTimer>([this, rollingAverage](GLuint64 startTime_, GLuint64 endTime_)->void
	{
		for (unsigned int i = 0; i < backgroundTimes.size() - 1; ++i)
		{
			backgroundTimes[i] = backgroundTimes[i + 1];
		}
		backgroundTimes[rollingAverage - 1] = endTime_ - startTime_;

		GLuint64 average = 0;
		for (unsigned int i = 0; i < backgroundTimes.size(); ++i)
		{
			average += backgroundTimes[i];
		}

		average /= backgroundTimes.size();

		logger.Log(std::to_string(average));
	});

	globalLightsTimes.resize(rollingAverage);
	globalLightsTimer = std::make_shared<QueryTimer>([this, rollingAverage](GLuint64 startTime_, GLuint64 endTime_)->void
	{
		for (unsigned int i = 0; i < globalLightsTimes.size() - 1; ++i)
		{
			globalLightsTimes[i] = globalLightsTimes[i + 1];
		}
		globalLightsTimes[rollingAverage - 1] = endTime_ - startTime_;

		GLuint64 average = 0;
		for (unsigned int i = 0; i < globalLightsTimes.size(); ++i)
		{
			average += globalLightsTimes[i];
		}

		average /= globalLightsTimes.size();

		logger.Log(std::to_string(average));
	});

	lbufferTimes.resize(rollingAverage);
	lbufferTimer = std::make_shared<QueryTimer>([this, rollingAverage](GLuint64 startTime_, GLuint64 endTime_)->void
	{
		for (unsigned int i = 0; i < lbufferTimes.size() - 1; ++i)
		{
			lbufferTimes[i] = lbufferTimes[i + 1];
		}
		lbufferTimes[rollingAverage - 1] = endTime_ - startTime_;

		GLuint64 average = 0;
		for (unsigned int i = 0; i < lbufferTimes.size(); ++i)
		{
			average += lbufferTimes[i];
		}

		average /= lbufferTimes.size();

		logger.Log(std::to_string(average));
	});

	postTimes.resize(rollingAverage);
	postTimer = std::make_shared<QueryTimer>([this, rollingAverage](GLuint64 startTime_, GLuint64 endTime_)->void
	{
		for (unsigned int i = 0; i < postTimes.size() - 1; ++i)
		{
			postTimes[i] = postTimes[i + 1];
		}
		postTimes[rollingAverage - 1] = endTime_ - startTime_;

		GLuint64 average = 0;
		for (unsigned int i = 0; i < postTimes.size(); ++i)
		{
			average += postTimes[i];
		}

		average /= postTimes.size();

		logger.Log(std::to_string(average));
	});

	dofTimes.resize(rollingAverage);
	dofTimer = std::make_shared<QueryTimer>([this, rollingAverage](GLuint64 startTime_, GLuint64 endTime_)->void
	{
		for (unsigned int i = 0; i < dofTimes.size() - 1; ++i)
		{
			dofTimes[i] = dofTimes[i + 1];
		}
		dofTimes[rollingAverage - 1] = endTime_ - startTime_;

		GLuint64 average = 0;
		for (unsigned int i = 0; i < dofTimes.size(); ++i)
		{
			average += dofTimes[i];
		}

		average /= dofTimes.size();

		logger.Log(std::to_string(average));
	});

	spotLightTimes.resize(rollingAverage);
	spotLightTimer = std::make_shared<QueryTimer>([this, rollingAverage](GLuint64 startTime_, GLuint64 endTime_)->void
	{
		for (unsigned int i = 0; i < spotLightTimes.size() - 1; ++i)
		{
			spotLightTimes[i] = spotLightTimes[i + 1];
		}
		spotLightTimes[rollingAverage - 1] = endTime_ - startTime_;

		GLuint64 average = 0;
		for (unsigned int i = 0; i < spotLightTimes.size(); ++i)
		{
			average += spotLightTimes[i];
		}

		average /= spotLightTimes.size();

		logger.Log(std::to_string(average));
	});


}

void MyView::
windowViewDidReset(std::shared_ptr<tygra::Window> window,
int width,
int height)
{
	glViewport(0, 0, width, height);
	_width = width;
	_height = height;

	aspectRatio = static_cast<float>(width) / height;

	{
		// gbuffer position texture
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[0]);
		glTexImage2D(
			GL_TEXTURE_RECTANGLE,
			0,
			GL_RGB32F,
			width,
			height,
			0,
			GL_RGB,
			GL_FLOAT,
			NULL
			);
		glBindTexture(GL_TEXTURE_RECTANGLE, 0);

		// gbuffer normal texture
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[1]);
		glTexImage2D(
			GL_TEXTURE_RECTANGLE,
			0,
			GL_RGB32F,
			width,
			height,
			0,
			GL_RGB,
			GL_FLOAT,
			NULL
			);
		glBindTexture(GL_TEXTURE_RECTANGLE, 0);

		// gbuffer material texture
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[2]);
		glTexImage2D(
			GL_TEXTURE_RECTANGLE,
			0,
			GL_RGBA32F,
			width,
			height,
			0,
			GL_RGBA,
			GL_FLOAT,
			NULL
			);
		glBindTexture(GL_TEXTURE_RECTANGLE, 0);

		// depth texture
		glBindTexture(GL_TEXTURE_RECTANGLE, depthStencilTO);
		glTexImage2D(
			GL_TEXTURE_RECTANGLE,
			0,
			GL_DEPTH24_STENCIL8,
			width,
			height,
			0,
			GL_DEPTH_STENCIL,
			GL_UNSIGNED_INT_24_8,
			NULL
			);
		glBindTexture(GL_TEXTURE_RECTANGLE, 0);

		GLenum gbuffer_status = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_RECTANGLE, depthStencilTO, 0); // attach depth stencil buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, gbufferTO[0], 0); // attach position buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, gbufferTO[1], 0); // attach normal buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, gbufferTO[2], 0); // attach material buffer

		gbuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (gbuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			tglDebugMessage(GL_DEBUG_SEVERITY_HIGH, "gbuffer not complete");
		}

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, buffers);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		//TODO: need to change to normal texture2D?
		// So that we can do a post process effect, we draw into a texture again
		glBindTexture(GL_TEXTURE_2D, lbufferTO);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA32F,
			width,
			height,
			0,
			GL_RGBA,
			GL_FLOAT,
			NULL
			);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		GLenum lbuffer_status = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, lbufferFBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lbufferTO, 0); // attach position buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_RECTANGLE, depthStencilTO, 0); // attach depth stencil buffer

		lbuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (lbuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			tglDebugMessage(GL_DEBUG_SEVERITY_HIGH, "lbuffer not complete");
		}

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		// shadows woooo
		glBindTexture(GL_TEXTURE_RECTANGLE, shadowDepthTO);
		glTexImage2D(
			GL_TEXTURE_RECTANGLE,
			0,
			GL_DEPTH_COMPONENT16,
			1024,
			1024,
			0,
			GL_DEPTH_COMPONENT,
			GL_FLOAT,
			NULL
			);

		glBindTexture(GL_TEXTURE_RECTANGLE, 0);

		GLenum lbuffer_status = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTO, 0); // attach depth output

		glDrawBuffer(GL_NONE);

		lbuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (lbuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			tglDebugMessage(GL_DEBUG_SEVERITY_HIGH, "shadowDepth buffer not complete");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		// single pixel texture object to allow us to do our depth of field shenanigans
		glBindTexture(GL_TEXTURE_RECTANGLE, depthPrePassTO);
		glTexImage2D(
			GL_TEXTURE_RECTANGLE,
			0,
			GL_RGBA32F,
			1,
			1,
			0,
			GL_RGBA,
			GL_FLOAT,
			NULL
			);
		glBindTexture(GL_TEXTURE_RECTANGLE, 0);

		GLenum lbuffer_status = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, depthPrePassFBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, depthPrePassTO, 0); // attach depth output

		lbuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (lbuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			tglDebugMessage(GL_DEBUG_SEVERITY_HIGH, "depthPrePass buffer not complete");
		}

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		// horizontal dof pass
		glBindTexture(GL_TEXTURE_2D, depthHorizontalPassTO);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA32F,
			width,
			height,
			0,
			GL_RGBA,
			GL_FLOAT,
			NULL
			);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		GLenum lbuffer_status = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, depthHorizontalPassFBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthHorizontalPassTO, 0); // attach depth output

		lbuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (lbuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			tglDebugMessage(GL_DEBUG_SEVERITY_HIGH, "depthPass buffer not complete");
		}

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		// vertical dof pass
		glBindTexture(GL_TEXTURE_2D, depthVerticalPassTO);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA32F,
			width,
			height,
			0,
			GL_RGBA,
			GL_FLOAT,
			NULL
			);

		glGenerateMipmap(GL_TEXTURE_2D); // generate mipmaps so that the fxaa shader will work

		glBindTexture(GL_TEXTURE_2D, 0);

		GLenum lbuffer_status = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, depthVerticalPassFBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthVerticalPassTO, 0); // attach depth output

		lbuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (lbuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			tglDebugMessage(GL_DEBUG_SEVERITY_HIGH, "depthPass buffer not complete");
		}

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		// pbuffer colour buffer
		glBindRenderbuffer(GL_RENDERBUFFER, fxaaColourRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB32F, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		GLenum pbuffer_status = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, fxaaFBO);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, fxaaColourRBO); // attach colour buffer

		pbuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (pbuffer_status != GL_FRAMEBUFFER_COMPLETE)
		{
			tglDebugMessage(GL_DEBUG_SEVERITY_HIGH, "pbuffer not complete");
		}

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void MyView::
windowViewDidStop(std::shared_ptr<tygra::Window> window)
{

	glDeleteFramebuffers(1, &gbufferFBO);
	glDeleteTextures(1, &depthStencilTO);
	glDeleteTextures(3, gbufferTO);

	glDeleteFramebuffers(1, &lbufferFBO);
	glDeleteTextures(1, &lbufferTO);

	glDeleteFramebuffers(1, &shadowDepthFBO);
	glDeleteTextures(1, &shadowDepthTO);

	glDeleteFramebuffers(1, &depthPrePassFBO);
	glDeleteTextures(1, &depthPrePassTO);

	glDeleteFramebuffers(1, &depthHorizontalPassFBO);
	glDeleteTextures(1, &depthHorizontalPassTO);

	glDeleteFramebuffers(1, &depthVerticalPassFBO);
	glDeleteTextures(1, &depthVerticalPassTO);

	glDeleteFramebuffers(1, &fxaaFBO);
	glDeleteRenderbuffers(1, &fxaaColourRBO);

	glDeleteQueries(1, &queryID);

}

void MyView::
windowViewRender(std::shared_ptr<tygra::Window> window)
{
	assert(scene_ != nullptr);

	glm::vec3 p = scene_->getCamera().getPosition();
	glm::vec3 d = scene_->getCamera().getDirection();


	glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);

	if (runningProfiler)
	{
		if (currentRun > maxRuns)
		{
			runningProfiler = false;
			logger.Close();
		}
		else
		{
			gbufferTimer->Check();
			backgroundTimer->Check();
			globalLightsTimer->Check();
			lbufferTimer->Check();
			spotLightTimer->Check();
			dofTimer->Check();
			postTimer->Check();

			logger.Flush();
			currentRun++;
		}
	}
	
	int gbufferTimeID = gbufferTimer->Start();
	renderSSBO.FillData();
	vboInstances[0].FillData();

	// set up the depth and stencil buffers, we are not writing to the onscreen framebuffer, we are filling the relevant data for the light render
	{
		firstPassProgram.useProgram();
		glBindFramebuffer(GL_FRAMEBUFFER, gbufferFBO);

		glClearColor(0.f, 0.f, 0.25f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear all 3 buffers

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		// not using these so disable them
		glDisable(GL_BLEND);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 127, ~0); // we are writing 1 to all pixels that the geometry draws into
		glStencilOp(GL_ZERO, GL_KEEP, GL_REPLACE);

		// the lights are tagged onto the end of the meshes
		for (unsigned int i = 0; i < loadedMeshes.size(); ++i)
		{
			if (!loadedMeshes[i].isStatic)
			{
				vboInstances[i].FillData();
			}

			glBindVertexArray(loadedMeshes[i].mesh.vao);
			glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
				loadedMeshes[i].mesh.element_count,
				GL_UNSIGNED_INT,
				TGL_BUFFER_OFFSET(loadedMeshes[i].mesh.startElementIndex * sizeof(int)),
				vboInstances[i].GetSize(),
				loadedMeshes[i].mesh.startVerticeIndex);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	gbufferTimer->End(gbufferTimeID);

	int backgroundTimeID = backgroundTimer->Start();
	// shade background as school of computing purple
	{
		backgroundProgram.useProgram();
		glBindFramebuffer(GL_FRAMEBUFFER, lbufferFBO);

		glClearColor(0.f, 0.f, 0.25f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT); // clear all 3 buffers

		glDisable(GL_DEPTH_TEST); // disable depth test snce we are drawing a full screen quad
		glDisable(GL_BLEND);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 0, ~0); // equal to background
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		// draw directional light
		glBindVertexArray(globalLightMesh.vao);
		glDrawElementsBaseVertex(GL_TRIANGLE_FAN,
			globalLightMesh.element_count,
			GL_UNSIGNED_INT,
			TGL_BUFFER_OFFSET(globalLightMesh.startElementIndex * sizeof(int)),
			globalLightMesh.startVerticeIndex);
	}
	backgroundTimer->End(backgroundTimeID);

	int globalLightTimeID = globalLightsTimer->Start();
	// global lights
	{
		globalLightProgram.useProgram();

		glDisable(GL_DEPTH_TEST); // disable depth test snce we are drawing a full screen quad
		glDisable(GL_BLEND);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 0, ~0);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		// could remove the glGetUniformLocation, but again, being lazy and fps is still around 100 - 105
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[0]);
		glUniform1i(glGetUniformLocation(globalLightProgram.getProgramID(), "sampler_world_position"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[1]);
		glUniform1i(glGetUniformLocation(globalLightProgram.getProgramID(), "sampler_world_normal"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[2]);
		glUniform1i(glGetUniformLocation(globalLightProgram.getProgramID(), "sampler_world_mat"), 2);

		// draw directional light
		glBindVertexArray(globalLightMesh.vao);
		glDrawElementsBaseVertex(GL_TRIANGLE_FAN,
			globalLightMesh.element_count,
			GL_UNSIGNED_INT,
			TGL_BUFFER_OFFSET(globalLightMesh.startElementIndex * sizeof(int)),
			globalLightMesh.startVerticeIndex);
	}
	globalLightsTimer->End(globalLightTimeID);

	int lbufferTimeID = lbufferTimer->Start();
	// lets draw the lights
	{
		pointLightProgram.useProgram();

		// additive blending
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glEnable(GL_DEPTH_TEST);// enable the depth test for use with lights
		glDepthMask(GL_FALSE);// disable depth writes since we dont want the lights to mess with the depth buffer
		glDepthFunc(GL_GREATER);// set the depth test to check for in front of the back fragments so that we can light correctly

		glEnable(GL_CULL_FACE); // enable the culling (not on by default)
		glCullFace(GL_FRONT); // set to cull forward facing fragments

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 0, ~0); // background is set to 0, we want the geometry pixels
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[0]);
		glUniform1i(glGetUniformLocation(pointLightProgram.getProgramID(), "sampler_world_position"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[1]);
		glUniform1i(glGetUniformLocation(pointLightProgram.getProgramID(), "sampler_world_normal"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[2]);
		glUniform1i(glGetUniformLocation(pointLightProgram.getProgramID(), "sampler_world_mat"), 2);

		vboPointLight.FillData();

		// instance draw the lights woop woop
		glBindVertexArray(pointLightMesh.vao);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
			pointLightMesh.element_count,
			GL_UNSIGNED_INT,
			TGL_BUFFER_OFFSET(pointLightMesh.startElementIndex * sizeof(int)),
			scene_->getAllPointLights().size(),
			pointLightMesh.startVerticeIndex);

		glDisable(GL_STENCIL_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);

		glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	lbufferTimer->End(lbufferTimeID);

	int spotLightTimeID = spotLightTimer->Start();
	{

		spotLightProgram.useProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[0]);
		glUniform1i(glGetUniformLocation(spotLightProgram.getProgramID(), "sampler_world_position"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[1]);
		glUniform1i(glGetUniformLocation(spotLightProgram.getProgramID(), "sampler_world_normal"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_RECTANGLE, gbufferTO[2]);
		glUniform1i(glGetUniformLocation(spotLightProgram.getProgramID(), "sampler_world_mat"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_RECTANGLE, shadowDepthTO);
		glUniform1i(glGetUniformLocation(spotLightProgram.getProgramID(), "shadow_depths"), 3);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 0, ~0); // background is set to 0, we want the geometry pixels
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		auto spotLights = scene_->getAllSpotLights();

		for (unsigned int i = 0; i < spotLights.size(); ++i)
		{


			glm::mat4 finalLightProjectionMatrix;

			if (spotLights[i].getCastShadow() && shadows)
			{
				glViewport(0, 0, 1024, 1024);
				shadowDepthPass.useProgram();

				glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);

				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				glDepthFunc(GL_LEQUAL);

				glClear(GL_DEPTH_BUFFER_BIT);

				glDisable(GL_BLEND);
				glEnable(GL_CULL_FACE);

				glm::mat4 projectionMatrix = glm::perspective(spotLights[i].getConeAngleDegrees(), 1.0f, 1.0f, spotLights[i].getRange());
				glm::mat4 viewMatrix = glm::lookAt(spotLights[i].getPosition(), spotLights[i].getPosition() + spotLights[i].getDirection(), scene_->getUpDirection());
				glm::mat4 projectionViewMatrix = projectionMatrix * viewMatrix;
				finalLightProjectionMatrix = projectionViewMatrix;

				// Send our transformation to the currently bound shader,
				// in the "MVP" uniform
				glUniformMatrix4fv(glGetUniformLocation(shadowDepthPass.getProgramID(), "projectionViewMat"), 1, GL_FALSE, glm::value_ptr(projectionViewMatrix));
				

				for (unsigned int i = 0; i < loadedMeshes.size(); ++i)
				{
					glBindVertexArray(loadedMeshes[i].mesh.vao);
					glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
						loadedMeshes[i].mesh.element_count,
						GL_UNSIGNED_INT,
						TGL_BUFFER_OFFSET(loadedMeshes[i].mesh.startElementIndex * sizeof(int)),
						vboInstances[i].GetSize(),
						loadedMeshes[i].mesh.startVerticeIndex);
				}

				glEnable(GL_BLEND);
				glDisable(GL_CULL_FACE);
				glViewport(0, 0, _width, _height);
			}

			spotLightProgram.useProgram();
			glBindFramebuffer(GL_FRAMEBUFFER, lbufferFBO);

			// additive blending
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);

			glEnable(GL_DEPTH_TEST);// enable the depth test for use with lights
			glDepthMask(GL_FALSE);// disable depth writes since we dont want the lights to mess with the depth buffer
			glDepthFunc(GL_GREATER);// set the depth test to check for in front of the back fragments so that we can light correctly

			glEnable(GL_CULL_FACE); // enable the culling (not on by default)
			glCullFace(GL_FRONT); // set to cull forward facing fragments

			glUniform3fv(glGetUniformLocation(spotLightProgram.getProgramID(), "lightPosition"), 1, glm::value_ptr(spotLights[i].getPosition()));
			glUniform1f(glGetUniformLocation(spotLightProgram.getProgramID(), "coneAngleDegrees"), spotLights[i].getConeAngleDegrees());
			glUniform3fv(glGetUniformLocation(spotLightProgram.getProgramID(), "direction"), 1, glm::value_ptr(spotLights[i].getDirection()));
			glUniform1f(glGetUniformLocation(spotLightProgram.getProgramID(), "range"), spotLights[i].getRange());
			glUniform3fv(glGetUniformLocation(spotLightProgram.getProgramID(), "intensity"), 1, glm::value_ptr(spotLights[i].getIntensity()));

			glUniformMatrix4fv(glGetUniformLocation(spotLightProgram.getProgramID(), "shadowProjectionViewMat"), 1, GL_FALSE, glm::value_ptr(finalLightProjectionMatrix));

			glUniform1i(glGetUniformLocation(spotLightProgram.getProgramID(), "shadows"), spotLights[i].getCastShadow() && shadows ? 1 : 0);


			glBindVertexArray(spotLightMesh.vao);
			glDrawElementsBaseVertex(GL_TRIANGLES,
				spotLightMesh.element_count,
				GL_UNSIGNED_INT,
				TGL_BUFFER_OFFSET(spotLightMesh.startElementIndex * sizeof(int)),
				spotLightMesh.startVerticeIndex);


		}

		glDisable(GL_STENCIL_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);

		glDisable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	spotLightTimer->End(spotLightTimeID);

	int dofID = dofTimer->Start();
	if (dof)
	{

		{
			depthPrePassProgram.useProgram();

			glBindFramebuffer(GL_FRAMEBUFFER, depthPrePassFBO);

			glDisable(GL_BLEND); // disable blending

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE, depthStencilTO);
			glUniform1i(glGetUniformLocation(depthPrePassProgram.getProgramID(), "sampler_depth"), 0);

			glUniform2f(glGetUniformLocation(depthPrePassProgram.getProgramID(), "dimensions"), float(_width), float(_height));

			// shadow perspective depth pass
			glBindVertexArray(globalLightMesh.vao);
			glDrawElementsBaseVertex(GL_TRIANGLE_FAN,
				globalLightMesh.element_count,
				GL_UNSIGNED_INT,
				TGL_BUFFER_OFFSET(globalLightMesh.startElementIndex * sizeof(int)),
				globalLightMesh.startVerticeIndex);

		}

		// depth of field pass
		{

			depthProgram.useProgram();


			glDisable(GL_BLEND); // disable blending

			// horizontal first
			glBindFramebuffer(GL_FRAMEBUFFER, depthHorizontalPassFBO);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE, depthStencilTO);
			glUniform1i(glGetUniformLocation(depthProgram.getProgramID(), "sampler_depth"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_RECTANGLE, depthPrePassTO);
			glUniform1i(glGetUniformLocation(depthProgram.getProgramID(), "sampler_focus_depth"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, lbufferTO);
			glUniform1i(glGetUniformLocation(depthProgram.getProgramID(), "sampler_texture"), 2);

			glUniform1i(glGetUniformLocation(depthProgram.getProgramID(), "horizontal"), 1);

			// shadow perspective depth pass
			glBindVertexArray(globalLightMesh.vao);
			glDrawElementsBaseVertex(GL_TRIANGLE_FAN,
				globalLightMesh.element_count,
				GL_UNSIGNED_INT,
				TGL_BUFFER_OFFSET(globalLightMesh.startElementIndex * sizeof(int)),
				globalLightMesh.startVerticeIndex);



			// vertical now
			glBindFramebuffer(GL_FRAMEBUFFER, depthVerticalPassFBO);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE, depthStencilTO);
			glUniform1i(glGetUniformLocation(depthProgram.getProgramID(), "sampler_depth"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_RECTANGLE, depthPrePassTO);
			glUniform1i(glGetUniformLocation(depthProgram.getProgramID(), "sampler_focus_depth"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, depthHorizontalPassTO);
			glUniform1i(glGetUniformLocation(depthProgram.getProgramID(), "sampler_texture"), 2);

			glUniform1i(glGetUniformLocation(depthProgram.getProgramID(), "horizontal"), 0);

			// shadow perspective depth pass
			glBindVertexArray(globalLightMesh.vao);
			glDrawElementsBaseVertex(GL_TRIANGLE_FAN,
				globalLightMesh.element_count,
				GL_UNSIGNED_INT,
				TGL_BUFFER_OFFSET(globalLightMesh.startElementIndex * sizeof(int)),
				globalLightMesh.startVerticeIndex);

		}
	}
	else
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, lbufferFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthVerticalPassFBO);
		glBlitFramebuffer(0, 0, viewport_size[2], viewport_size[3], 0, 0, viewport_size[2], viewport_size[3], GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	dofTimer->End(dofID);

	int postProcessTimeID = postTimer->Start();
	if (AA)
	{
		// fxaa
		fxaaProgram.useProgram();

		glBindFramebuffer(GL_FRAMEBUFFER, fxaaFBO);

		glClearColor(0.f, 0.f, 0.25f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT); // clear all 3 buffers

		glDisable(GL_BLEND); // disable blending

		//GLuint textureToDraw = dof ? depthVerticalPassTO : lbufferTO;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthVerticalPassTO);
		glUniform1i(glGetUniformLocation(fxaaProgram.getProgramID(), "uSourceTex"), 0);

		glUniform2f(glGetUniformLocation(fxaaProgram.getProgramID(), "RCPFrame"), float(1.0 / float(_width)), float(1.0 / float(_height)));

		glBindVertexArray(globalLightMesh.vao);
		glDrawElementsBaseVertex(GL_TRIANGLE_FAN,
			globalLightMesh.element_count,
			GL_UNSIGNED_INT,
			TGL_BUFFER_OFFSET(globalLightMesh.startElementIndex * sizeof(int)),
			globalLightMesh.startVerticeIndex);

	}
	else
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, depthVerticalPassFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fxaaFBO);
		glBlitFramebuffer(0, 0, viewport_size[2], viewport_size[3], 0, 0, viewport_size[2], viewport_size[3], GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	postTimer->End(postProcessTimeID);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, fxaaFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, viewport_size[2], viewport_size[3], 0, 0, viewport_size[2], viewport_size[3], GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind the framebuffers
}

// method fixes damn inconsistencies of this so called 'legacy code'
inline glm::vec3 ConvVec3(tsl::Vector3 &vec_)
{
	return glm::vec3(vec_.x, vec_.y, vec_.z);
}

void MyView::GenerateShaderPrograms()
{
	{
		Shader vs, fs;
		vs.loadShader("firstpass_vs.glsl", GL_VERTEX_SHADER);
		fs.loadShader("firstpass_fs.glsl", GL_FRAGMENT_SHADER);

		firstPassProgram.createProgram();
		firstPassProgram.addShaderToProgram(&vs);
		firstPassProgram.addShaderToProgram(&fs);

		// set the channels of the output for this one to be sure
		glBindFragDataLocation(firstPassProgram.getProgramID(), 0, "position");
		glBindFragDataLocation(firstPassProgram.getProgramID(), 1, "normal");
		glBindFragDataLocation(firstPassProgram.getProgramID(), 2, "material");

		firstPassProgram.linkProgram();

		firstPassProgram.useProgram();
	}

	{
	}   // this empty scope is here because in my editor there is some weird auto format issue that screws with the next 'background shader' scope. ignore it please.

	{
		Shader vs, fs;
		vs.loadShader("background_vs.glsl", GL_VERTEX_SHADER);
		fs.loadShader("background_fs.glsl", GL_FRAGMENT_SHADER);

		backgroundProgram.createProgram();
		backgroundProgram.addShaderToProgram(&vs);
		backgroundProgram.addShaderToProgram(&fs);

		backgroundProgram.linkProgram();

		backgroundProgram.useProgram();
	}

	{
		Shader vs, fs;
		vs.loadShader("global_light_vs.glsl", GL_VERTEX_SHADER);
		fs.loadShader("global_light_fs.glsl", GL_FRAGMENT_SHADER);

		globalLightProgram.createProgram();
		globalLightProgram.addShaderToProgram(&vs);
		globalLightProgram.addShaderToProgram(&fs);

		globalLightProgram.linkProgram();

		globalLightProgram.useProgram();
	}

	{
		Shader vs, fs;
		vs.loadShader("pointlight_vs.glsl", GL_VERTEX_SHADER);
		fs.loadShader("pointlight_fs.glsl", GL_FRAGMENT_SHADER);

		pointLightProgram.createProgram();
		pointLightProgram.addShaderToProgram(&vs);
		pointLightProgram.addShaderToProgram(&fs);
		pointLightProgram.linkProgram();

		pointLightProgram.useProgram();
	}

	{
		Shader vs, fs;
		vs.loadShader("spotlight_vs.glsl", GL_VERTEX_SHADER);
		fs.loadShader("spotlight_fs.glsl", GL_FRAGMENT_SHADER);

		spotLightProgram.createProgram();
		spotLightProgram.addShaderToProgram(&vs);
		spotLightProgram.addShaderToProgram(&fs);
		spotLightProgram.linkProgram();

		spotLightProgram.useProgram();

	}

	{
		Shader vs, fs;
		vs.loadShader("FXAA.vert", GL_VERTEX_SHADER);
		fs.loadShader("FXAA_Default.frag", GL_FRAGMENT_SHADER);

		fxaaProgram.createProgram();
		fxaaProgram.addShaderToProgram(&vs);
		fxaaProgram.addShaderToProgram(&fs);

		fxaaProgram.linkProgram();

		fxaaProgram.useProgram();
	}

	{
		Shader vs, fs;
		vs.loadShader("depth_pre_pass_vs.glsl", GL_VERTEX_SHADER);
		fs.loadShader("depth_pre_pass_fs.glsl", GL_FRAGMENT_SHADER);

		depthPrePassProgram.createProgram();
		depthPrePassProgram.addShaderToProgram(&vs);
		depthPrePassProgram.addShaderToProgram(&fs);

		depthPrePassProgram.linkProgram();

		depthPrePassProgram.useProgram();
	}

	{
		Shader vs, fs;
		vs.loadShader("depth_pass_vs.glsl", GL_VERTEX_SHADER);
		fs.loadShader("depth_pass_fs.glsl", GL_FRAGMENT_SHADER);

		depthProgram.createProgram();
		depthProgram.addShaderToProgram(&vs);
		depthProgram.addShaderToProgram(&fs);

		depthProgram.linkProgram();

		depthProgram.useProgram();


		float near = scene_->getCamera().getNearPlaneDistance(); // 1
		float far = scene_->getCamera().getFarPlaneDistance(); // 1000

		glUniform2f(glGetUniformLocation(depthProgram.getProgramID(), "p"), float((far + near) / (near - far)), float(((-2 * far) * near) / (far - near)));

	}

	{
		Shader vs, fs;
		vs.loadShader("shadow_depth_pass_vs.glsl", GL_VERTEX_SHADER);
		fs.loadShader("shadow_depth_pass_fs.glsl", GL_FRAGMENT_SHADER);

		shadowDepthPass.createProgram();
		shadowDepthPass.addShaderToProgram(&vs);
		shadowDepthPass.addShaderToProgram(&fs);

		shadowDepthPass.linkProgram();

		shadowDepthPass.useProgram();

	}

}

void MyView::SetupSSBOS()
{
	renderSSBO = SSBO([this](GLuint bufferID_) -> bool
	{
		glm::mat4 projectionMatrix = glm::perspective(75.f, aspectRatio, 1.f, 1000.f);
		glm::mat4 viewMatrix = glm::lookAt(scene_->getCamera().getPosition(), scene_->getCamera().getDirection() + scene_->getCamera().getPosition(), glm::vec3(0, 1, 0));
		glm::mat4 projectionViewMatrix = projectionMatrix * viewMatrix;

		// so since glMapBufferRange does not work, i am going to create a temporary buffer for the per model data, and then copy the full buffer straight into the shaders buffer
		//unsigned int bufferSize = sizeof(projectMat_)+sizeof(camPos_);
		//char* buffer = new char[bufferSize];
		unsigned int bufferSize = 19;
		float buffer[19];

		//projection matrix first!
		memcpy(buffer, glm::value_ptr(projectionViewMatrix), sizeof(glm::mat4));

		// camera position next!
		memcpy(buffer + 16, glm::value_ptr(scene_->getCamera().getPosition()), sizeof(glm::vec3));

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID_);
		glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize * 4, buffer, GL_STREAM_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return true;
	});
	renderSSBO.GenerateBuffer();
	renderSSBO.AttachToProgram(0, firstPassProgram, "BufferRender");
	renderSSBO.AttachToProgram(0, pointLightProgram, "BufferRender");
	renderSSBO.AttachToProgram(0, spotLightProgram, "BufferRender");



	// setup material SSBO
	materialSSBO = SSBO([this](GLuint bufferID_) -> bool
	{
		std::vector<MaterialData> materialData;

		for (unsigned int i = 0; i < scene_->getAllMaterials().size(); ++i)
		{
			mapMaterialIndex[scene_->getAllMaterials()[i].getId()] = materialData.size();

			MaterialData data;
			data.colour = scene_->getAllMaterials()[i].getDiffuseColour();
			data.shininess = scene_->getAllMaterials()[i].getShininess();
			materialData.push_back(data);
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID_);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(MaterialData)* materialData.size(), materialData.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return true;
	});
	materialSSBO.GenerateBuffer();
	materialSSBO.AttachToProgram(1, firstPassProgram, "BufferMaterials");



	directionalLightsSSBO = SSBO([this](GLuint bufferID_) -> bool
	{
		std::vector<DirectionalLightData> directionalLights;
		directionalLights.resize(scene_->getAllDirectionalLights().size());

		for (unsigned int i = 0; i < scene_->getAllDirectionalLights().size(); ++i)
		{
			DirectionalLightData data;
			data.direction = scene_->getAllDirectionalLights()[i].getDirection();
			data.intensity = scene_->getAllDirectionalLights()[i].getIntensity();
			directionalLights[i] = data;
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID_);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DirectionalLightData)* directionalLights.size(), directionalLights.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return true;
	});
	directionalLightsSSBO.GenerateBuffer();
	directionalLightsSSBO.AttachToProgram(2, globalLightProgram, "BufferLights");
}

void MyView::GenerateMeshes(const std::vector<SceneModel::Mesh> &meshes_)
{

	meshBuffer.GenerateBuffers();
	//load scene meshes
	for (unsigned int i = 0; i < meshes_.size(); ++i)
	{
		MeshWrapper wrapper;
		wrapper.mesh = meshBuffer.AddMesh(meshes_[i]);

		std::vector<SceneModel::InstanceId> ids = scene_->getInstancesByMeshId(meshes_[i].getId());
		for (unsigned int j = 0; j < ids.size(); ++j)
		{
			if (!scene_->getInstanceById(ids[j]).isStatic())
			{
				wrapper.isStatic = false;
				break;
			}
		}

		loadedMeshes.push_back(wrapper);
	}

	// set up pointlight mesh
	{
		tsl::IndexedMesh mesh;
		tsl::CreateSphere(1.f, 12, &mesh);
		tsl::ConvertPolygonsToTriangles(&mesh);

		pointLightMesh = meshBuffer.AddMesh(mesh);
		spotLightMesh.element_count = pointLightMesh.element_count;
		spotLightMesh.endElementIndex = pointLightMesh.endElementIndex;
		spotLightMesh.endVerticeIndex = pointLightMesh.endVerticeIndex;
		spotLightMesh.startElementIndex = pointLightMesh.startElementIndex;
		spotLightMesh.startVerticeIndex = pointLightMesh.startVerticeIndex;
		spotLightMesh.verticeCount = pointLightMesh.verticeCount;

	}

	// set up spotlight mesh
	/*{

		tsl::IndexedMesh mesh;
		tsl::CreateCone(1.f, 1.f, 12, &mesh);
		tsl::ConvertPolygonsToTriangles(&mesh);
		spotLightMesh = meshBuffer.AddMesh(mesh);
		}*/

	// square mesh
	{
		std::vector<glm::vec3> vertices = {
			glm::vec3(-1, -1, 0),
			glm::vec3(1, -1, 0),
			glm::vec3(1, 1, 0),
			glm::vec3(-1, 1, 0)
		};

		std::vector<glm::vec3> normals = {
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, -1)
		};

		std::vector<unsigned int> elements = {
			0, 1, 2, 3
		};

		globalLightMesh = meshBuffer.AddMesh(vertices, normals, elements);
	}

	meshBuffer.Flush();
}

void MyView::ToggleDoF()
{
	dof = !dof;
}

void MyView::ToggleShadows()
{
	shadows = !shadows;
}

void MyView::ToggleAA()
{
	AA = !AA;
}

void MyView::RunProfiling()
{
	runningProfiler = true;
	logger.Open();

	currentRun = 0;

}