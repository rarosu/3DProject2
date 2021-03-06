#include <Helper\MorphAnimation.hpp>
#include <cassert>

namespace Helper
{

	MorphAnimation::MorphAnimation(ID3D10Device* device, const std::vector<std::string>& keyFrameFilenames,
								   const std::vector<float> timeSpans, const std::string& glowMap)
		: mTime(0.0f)
		, mDevice(device)
		, mCurrentFrame(0)
		, mLooping(true)
		, mForwards(true)
		, mEffect(device, "Resources/Effects/Morph.fx")
	{
		assert(keyFrameFilenames.size() == timeSpans.size());
		for (int i = 0; i < keyFrameFilenames.size(); ++i)
		{
			//mKeyFrames.push_back(KeyFrame(Resources::ModelResourceManager::Instance().Load(keyFrameFilenames[i]), timeSpans[i]));
			mKeyFrames.push_back(KeyFrame(new Resources::StaticModelData(mDevice, keyFrameFilenames[i]), timeSpans[i]));
		}

		Framework::Effect::InputLayoutVector vec;
		vec.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 0, 0));
		vec.push_back(Framework::Effect::InputLayoutElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 0, 0));
		vec.push_back(Framework::Effect::InputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 0, 0));
		vec.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 1, 1));
		vec.push_back(Framework::Effect::InputLayoutElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 1, 1));
		vec.push_back(Framework::Effect::InputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 1, 1));

		mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(vec);
		mEffect.SetVariable("t", 0.0f);
		mEffect.SetVariable("gTexture", mKeyFrames[0].Data->MaterialData->GetMaterial(mKeyFrames[0].Data->MaterialName)->MainTexture->GetShaderResourceView());
		mEffect.SetVariable("gGlowMap", Resources::Texture(mDevice, glowMap).GetShaderResourceView());
		mEffect.SetVariable("Ka", mKeyFrames[0].Data->MaterialData->GetMaterial(mKeyFrames[0].Data->MaterialName)->Ambient.x);
		mEffect.SetVariable("Kd", mKeyFrames[0].Data->MaterialData->GetMaterial(mKeyFrames[0].Data->MaterialName)->Diffuse.x);
		mEffect.SetVariable("Ks", mKeyFrames[0].Data->MaterialData->GetMaterial(mKeyFrames[0].Data->MaterialName)->Specular.x);
		mEffect.SetVariable("A", mKeyFrames[0].Data->MaterialData->GetMaterial(mKeyFrames[0].Data->MaterialName)->SpecularExp);
	}

	MorphAnimation::KeyFrame::KeyFrame(Resources::StaticModelData* data, float timeSpan)
		: Data(data), TimeSpan(timeSpan)
	{}

	MorphAnimation::~MorphAnimation() throw()
	{
		for (int i = 0; i < mKeyFrames.size(); ++i)
			SafeDelete(mKeyFrames[i].Data);
	}

	void MorphAnimation::Update(float dt)
	{
		HandleTime(dt);

		mEffect.SetVariable("t", mTime / mKeyFrames[mCurrentFrame].TimeSpan);
	}

	void MorphAnimation::Draw(const Camera::Camera& camera, const D3DXMATRIX& modelMatrix)
	{
		D3DXMATRIX wvp;
		wvp = modelMatrix * camera.GetViewProjection();

		mEffect.SetVariable("gWorld", modelMatrix);
		mEffect.SetVariable("gMVP", wvp);

		mKeyFrames[mCurrentFrame].Data->VertexData.Bind(0);
		mKeyFrames[mCurrentFrame + 1].Data->VertexData.Bind(1);

		for (int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mKeyFrames[mCurrentFrame].Data->VertexData.GetElementCount(), 0);
		}

	}

	void MorphAnimation::DrawAABB(const Camera::Camera& camera, const D3DXMATRIX& modelMatrix)
	{
		mKeyFrames[mCurrentFrame].Data->DrawAABB(camera, modelMatrix);
	}

	const AABB3f& MorphAnimation::GetAABB() const
	{
		return mKeyFrames[mCurrentFrame].Data->Box;
	}

	void MorphAnimation::HandleTime(float dt)
	{
		if (mForwards) // If animation is going forwards
		{
			mTime += dt;

			if (mTime > mKeyFrames[mCurrentFrame].TimeSpan) // This key frame is over
			{
				if (mCurrentFrame < mKeyFrames.size() - 2) // This key frame isn't the last one (don't turn the animation around)
				{
					mTime = 0.0f;
					mCurrentFrame++;
				}
				else // This key frame is the last one, turn backward or stop.
				{
					if (mLooping)
					{
						mForwards = false;
					}
					
					// Set time to largest possible for this frame.
					mTime = mKeyFrames[mCurrentFrame].TimeSpan;
					
				}
			}
		}
		else // The animation is going backwards
		{
			mTime -= dt;

			if (mTime < 0.0f) // This key frame is over
			{
				if (mCurrentFrame > 0) // This key frame isn't the first one (don't turn the animation around)
				{
					mCurrentFrame--;
					mTime = mKeyFrames[mCurrentFrame].TimeSpan;
				}
				else // This key frame is the first one, turn forward.
				{
					mForwards = true;

					// Set time to smallest possible for this frame.
					mTime = 0.0f;
				}
			}
		}
	}
}