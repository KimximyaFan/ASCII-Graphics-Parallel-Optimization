#define INITGUID
#include "texture_manager.h"
#include "image_texture.h"
#include <windows.h>
#include <wincodec.h>
#include <string>

const Texture* Texture_Manager::GetTexture(Texture_Handle handle) const
{
    return registered_textures[ToIndex(handle)].get();
}

bool Texture_Manager::Is_Exist(Texture_Handle handle) const
{
    return static_cast<bool>(registered_textures[ToIndex(handle)]);
}

bool Texture_Manager::LoadImageFile(const char* path, int& outW, int& outH, std::vector<Color>& outPixels)
{
    if (!path) return false;

    // COM 초기화 (앱 시작 시 한 번만 하는 게 바람직)
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool needUninit = (hr == S_OK || hr == S_FALSE);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) return false;

    auto cleanup = [&](){
        if (needUninit) CoUninitialize();
    };

    IWICImagingFactory* factory = nullptr;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&factory));
    if (FAILED(hr)) { cleanup(); return false; }

    // UTF-8 경로 → UTF-16
    int wlen = MultiByteToWideChar(CP_UTF8, 0, path, -1, nullptr, 0);
    if (wlen <= 0) { factory->Release(); cleanup(); return false; }
    std::wstring wpath; wpath.resize(wlen);
    MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath.data(), wlen);

    IWICBitmapDecoder* decoder = nullptr;
    hr = factory->CreateDecoderFromFilename(
        wpath.c_str(), nullptr, GENERIC_READ,
        WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr)) { factory->Release(); cleanup(); return false; }

    IWICBitmapFrameDecode* frame = nullptr;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) { decoder->Release(); factory->Release(); cleanup(); return false; }

    // RGBA 32bpp로 변환
    IWICFormatConverter* conv = nullptr;
    hr = factory->CreateFormatConverter(&conv);
    if (FAILED(hr)) { frame->Release(); decoder->Release(); factory->Release(); cleanup(); return false; }

    hr = conv->Initialize(
        frame,
        GUID_WICPixelFormat32bppRGBA, // 비-프리멀티 RGBA
        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) { conv->Release(); frame->Release(); decoder->Release(); factory->Release(); cleanup(); return false; }

    UINT W=0, H=0;
    conv->GetSize(&W, &H);
    if (W==0 || H==0) { conv->Release(); frame->Release(); decoder->Release(); factory->Release(); cleanup(); return false; }

    const UINT stride = W * 4;
    const size_t bufSize = size_t(stride) * size_t(H);
    std::vector<uint8_t> buf(bufSize);

    hr = conv->CopyPixels(nullptr, stride, (UINT)bufSize, buf.data());
    if (FAILED(hr)) { conv->Release(); frame->Release(); decoder->Release(); factory->Release(); cleanup(); return false; }

    // RGBA8 → RGBA(float 0..1)
    outW = (int)W; outH = (int)H;
    outPixels.resize(size_t(W)*size_t(H));
    const float k = 1.0f/255.0f;
    for (size_t i=0; i<outPixels.size(); ++i) {
        size_t idx = i*4;
        outPixels[i] = { buf[idx+0]*k, buf[idx+1]*k, buf[idx+2]*k, buf[idx+3]*k };
    }

    conv->Release();
    frame->Release();
    decoder->Release();
    factory->Release();
    cleanup();
    return true;
}

bool Texture_Manager::RegisterImage(Texture_Handle handle, const char* path, const Sampler_Desc sampler)
{
    int w = 0, h = 0;
    std::vector<Color> pixels;

    if ( Is_Exist(handle) )
        return false;

    if ( LoadImageFile(path, w, h, pixels) == false )
        return false;

    if (w <= 0 || h <= 0 || pixels.size() != static_cast<size_t>(w) * static_cast<size_t>(h))
        return false;

    auto tex = std::make_unique<Image_Texture>(w, h, std::move(pixels));
    tex->SetSampler(sampler);
    registered_textures[ToIndex(handle)] = std::move(tex);

    return true;
}

bool Texture_Manager::RegisterProcedural(Texture_Handle handle, const std::vector<Color> pixels, int w, int h, const Sampler_Desc sampler)
{
    if ( Is_Exist(handle) )
        return false;

    auto tex = std::make_unique<Image_Texture>(w, h, std::move(pixels));
    tex->SetSampler(sampler);
    registered_textures[ToIndex(handle)] = std::move(tex);
    return true;
}