#ifndef MORPH_ANIMATION_HPP
#define MORPH_ANIMATION_HPP

#include <vector>
#include <Helper\Global.hpp>
#include <Camera\Camera.hpp>
#include <Resources\StaticModelData.hpp>
#include <Framework\Effect\Effect.hpp>

namespace Helper
{
	class MorphAnimation
	{
	public:
		MorphAnimation(ID3D10Device* device, const std::vector<std::string>& keyFrameFilenames,
					   const std::vector<float> timeSpans, const std::string& glowMap);
		~MorphAnimation() throw();

		void Update(float dt);
		void Draw(const Camera::Camera& camera, const D3DXMATRIX& modelMatrix);
		void DrawAABB(const Camera::Camera& camera, const D3DXMATRIX& modelMatrix);

		const AABB3f& GetAABB() const;
	private:
		struct AnimationVertex
		{
			D3DXVECTOR3 Position;
			D3DXVECTOR3 Normal;
			D3DXVECTOR2 UV;
		};
		struct KeyFrame
		{
			KeyFrame(Resources::StaticModelData* data, float timeSpan);

			Resources::StaticModelData* Data;
			float TimeSpan;
		};

		void HandleTime(float dt);
		

		std::vector<KeyFrame> mKeyFrames;

		Framework::Effect::Effect mEffect;

		bool mLooping;
		bool mForwards;
		int mCurrentFrame;
		float mTime;

		//DEBUG
		ID3D10Device* mDevice;
	};
}

#endif