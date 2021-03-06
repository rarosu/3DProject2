#ifndef DEFERRED_RENDERER_HPP
#define DEFERRED_RENDERER_HPP

#include <vector>
#include <map>
#include <Framework\D3DContext.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Framework\VertexBuffer.hpp>
#include <Helper\Global.hpp>
#include <Camera\Camera.hpp>
#include <Resources\Texture.hpp>
#include <SSAO.hpp>

struct DirectionalLight
{
	D3DXVECTOR4 Direction;
	D3DXVECTOR4 Intensity;
};

struct PointLight
{
	D3DXVECTOR4 Position;
	D3DXVECTOR3 Intensity;
	float Radius;
};

class DeferredRenderer 
{
public:
	enum GBuffer { Color, Position, Normal, Material, PreSSAO, PostSSAO, Depth, Count };

	DeferredRenderer(Framework::D3DContext* d3dContext, int width, int height);
	~DeferredRenderer() throw();

	void SetDirectionalLight(const DirectionalLight& light);
	size_t AddPointLight(const PointLight& light);
	void SetAmbientLight(const D3DXVECTOR3& light);

	void ToggleSSAO(bool ssaoOn);
	bool GetSSAOToggle() const;

	void BeginDeferredState();
	void EndDeferredState(const Camera::Camera& camera, const Helper::Frustum& frustum);
	void ApplyLightingPhase(const Camera::Camera& camera);
	void BeginForwardState();
	void EndForwardState();

	void RenderBuffer(ID3D10ShaderResourceView* buffer);

	ID3D10ShaderResourceView* GetColorBuffer() const;
	ID3D10ShaderResourceView* GetPositionBuffer() const;
	ID3D10ShaderResourceView* GetNormalBuffer() const;
	ID3D10ShaderResourceView* GetMaterialBuffer() const;
	ID3D10ShaderResourceView* GetDepthStencilBuffer() const;
	ID3D10ShaderResourceView* GetGBufferByIndex(unsigned int index) const;
private:
	struct QuadVertex
	{
		D3DXVECTOR2 Position;
		D3DXVECTOR2 TexCoord;
	};

	ID3D10Texture2D* CreateTexture(DXGI_FORMAT format, UINT bindFlags) const;
	ID3D10RenderTargetView* CreateRenderTargetView(ID3D10Texture2D* buffer) const;
	ID3D10ShaderResourceView* CreateShaderResourceView(ID3D10Texture2D* buffer, DXGI_FORMAT format) const;


	Framework::D3DContext* mD3DContext;
	ID3D10Device* mDevice;
	int mWidth;
	int mHeight;

	// The buffers holding the G-buffer data.
	ID3D10Texture2D* mColorBuffer;
	ID3D10Texture2D* mPositionBuffer;
	ID3D10Texture2D* mNormalBuffer;
	ID3D10Texture2D* mMaterialBuffer;
	ID3D10Texture2D* mSSAOBuffer;
	ID3D10Texture2D* mSSAOPostBuffer;
	ID3D10Texture2D* mDepthStencilBuffer;

	// Render/Depth views for rendering to the buffers
	ID3D10RenderTargetView* mColorView;
	ID3D10RenderTargetView* mPositionView;
	ID3D10RenderTargetView* mNormalView;
	ID3D10RenderTargetView* mMaterialView;
	ID3D10RenderTargetView* mSSAOView;
	ID3D10RenderTargetView* mSSAOPostView;
	ID3D10DepthStencilView* mDepthStencilView;

	// Shader resource views for reading from the buffers
	ID3D10ShaderResourceView* mColorSRV;
	ID3D10ShaderResourceView* mPositionSRV;
	ID3D10ShaderResourceView* mNormalSRV;
	ID3D10ShaderResourceView* mMaterialSRV;
	ID3D10ShaderResourceView* mSSAOSRV;
	ID3D10ShaderResourceView* mSSAOPostSRV;
	ID3D10ShaderResourceView* mDepthStencilSRV;

	// Contains the G buffers: color, normal and depth.
	std::vector<ID3D10Texture2D*> mGBuffers;

	// Contains the G buffer render targets: color and normal
	std::vector<ID3D10RenderTargetView*> mRenderTargets;

	// Contains the G buffer shader resource views: color, normal and depth.
	std::vector<ID3D10ShaderResourceView*> mShaderResourceViews;


	// For applying the SSAO effect
	SSAO mSSAO;

	// Whether SSAO should be on or off
	bool mSSAOToggle;


	// Keeps track of the lights in the scene
	D3DXVECTOR3 mAmbientLight;
	DirectionalLight mDirectionalLight;
	std::vector<PointLight> mPointLights;

	// The shader for the lighting pass, is applied to a fullscreen quad.
	Framework::Effect::Effect mLightEffect;

	// The shader for rendering a G buffer to the screen
	Framework::Effect::Effect mBufferEffect;

	Framework::VertexBuffer mFullscreenQuad;
};

#endif