#include "Texture.h"
namespace Graphics
{
	D3D11_TEXTURE_ADDRESS_MODE TextureDesc::getGdiMode()
	{
		static D3D11_TEXTURE_ADDRESS_MODE dxModes[] =
		{
			/*eWrap = 0,	 */ D3D11_TEXTURE_ADDRESS_WRAP,
			/*eClamp,		 */ D3D11_TEXTURE_ADDRESS_CLAMP,
			/*eMirror,		 */ D3D11_TEXTURE_ADDRESS_MIRROR
			/*eModeNum		 */
		};
		return dxModes[mode_];
	}

	D3D11_FILTER TextureDesc::getGdiFilter()
	{
		static D3D11_FILTER dxFilters[] =
		{
			/*eLinear,	     */ D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			/*ePoint,    	 */ D3D11_FILTER_MIN_MAG_MIP_POINT,
			/*eAnisotropic,	 */ D3D11_FILTER_ANISOTROPIC,
			/*eFilterNum     */
		};
		return dxFilters[filter_];
	}

	void Texture::getSize(unsigned int * width, unsigned int * height)
	{
		ID3D11Texture2D *texture;
		tex_->GetResource((ID3D11Resource**)(&texture));
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);
		*width = texDesc.Width;
		*height = texDesc.Height;
	}

}