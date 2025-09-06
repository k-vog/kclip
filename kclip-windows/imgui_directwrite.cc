// dear imgui: DirectWrite font builder

#include "imgui_directwrite.hh"
#include "imgui_internal.h"

#include <dwrite.h>

// Stored in ImFontAtlas::FontLoaderData. ALLOCATED BY US.
struct ImGui_ImplDirectWrite_Data
{
  IDWriteFactory* factory;
};

// Stored in ImFontConfig::FontLoaderData. ALLOCATED BY US.
struct ImGui_ImplDirectWrite_FontSrcData
{
  bool InitFont(IDWriteFactory* factory, ImFontConfig* src);

  int test;
};

// Stored in ImFontBaked::FontLoaderDatas: pointer to SourcesCount instances of this. ALLOCATED BY CORE.
struct ImGui_ImplDirectWrite_FontSrcBakedData
{
  int test;
};

static const wchar_t* BADCODE_WriteTempFontFile(void* buf, int len)
{
  static wchar_t tmpd[MAX_PATH] = { };
  static wchar_t tmpf[MAX_PATH] = { };
  if (!GetTempPathW(MAX_PATH, tmpd) || !GetTempFileNameW(tmpd, L"imdw", 0, tmpf))
    return nullptr;

  HANDLE h = CreateFileW(tmpf, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, nullptr);
  if (h == INVALID_HANDLE_VALUE)
    return nullptr;

  DWORD written = 0;
  BOOL ok = WriteFile(h, buf, len, &written, nullptr);
  CloseHandle(h);

  if (!ok || written != len) {
    DeleteFileW(tmpf);
    return nullptr;
  }

  return tmpf;
}

bool ImGui_ImplDirectWrite_FontSrcData::InitFont(IDWriteFactory* factory, ImFontConfig* src)
{
  const wchar_t* path = BADCODE_WriteTempFontFile(src->FontData, src->FontDataSize);
  if (!path)
    return false;

  IDWriteFontFile* dwrite_file = nullptr;
  HRESULT hr = factory->CreateFontFileReference(path, nullptr, &dwrite_file);

  DeleteFileW(path);

  if (FAILED(hr))
    return false;

  IDWriteFontFace* dwrite_face = { };

  BOOL                  dwrite_is_supported = { };
  DWRITE_FONT_FILE_TYPE dwrite_file_type    = { };
  UINT32                dwrite_face_count   = { };
  DWRITE_FONT_FACE_TYPE dwrite_face_type    = { };
  hr = dwrite_file->Analyze(&dwrite_is_supported, &dwrite_file_type, &dwrite_face_type, &dwrite_face_count);

  if (SUCCEEDED(hr) && dwrite_is_supported)
    hr = factory->CreateFontFace(dwrite_face_type, 1, &dwrite_file, 0, DWRITE_FONT_SIMULATIONS_NONE, &dwrite_face);

  dwrite_file->Release();
  
  return SUCCEEDED(hr) && dwrite_face != nullptr;
}

static bool ImGui_ImplDirectWrite_LoaderInit(ImFontAtlas* atlas)
{
  IM_ASSERT(atlas->FontLoaderData == nullptr);
  ImGui_ImplDirectWrite_Data* bd = IM_NEW(ImGui_ImplDirectWrite_Data)();

  if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&bd->factory)))
  {
    IM_DELETE(bd);
    return false;
  }

  atlas->FontLoaderData = bd;

  return true;
}

static void ImGui_ImplDirectWrite_LoaderShutdown(ImFontAtlas* atlas)
{
  ImGui_ImplDirectWrite_Data* bd = (ImGui_ImplDirectWrite_Data*)atlas->UserData;
  IM_ASSERT(bd != nullptr);
  bd->factory->Release();
  IM_DELETE(bd);
  atlas->UserData = nullptr;
}

static bool ImGui_ImplDirectWrite_FontSrcInit(ImFontAtlas* atlas, ImFontConfig* src)
{
  ImGui_ImplDirectWrite_Data* bd = (ImGui_ImplDirectWrite_Data*)atlas->FontLoaderData;
  ImGui_ImplDirectWrite_FontSrcData* bd_font_data = IM_NEW(ImGui_ImplDirectWrite_FontSrcData);
  IM_ASSERT(src->FontLoaderData == nullptr);
  src->FontLoaderData = bd_font_data;

  if (!bd_font_data->InitFont(bd->factory, src))
  {
    IM_DELETE(bd_font_data);
    src->FontLoaderData = nullptr;
    return false;
  }

  return true;
}

static void ImGui_ImplDirectWrite_FontSrcDestroy(ImFontAtlas* atlas, ImFontConfig* src)
{
}

static bool ImGui_ImplDirectWrite_FontSrcContainsGlyph(ImFontAtlas* atlas, ImFontConfig* src, ImWchar codepoint)
{
  return false;
}

static bool ImGui_ImplDirectWrite_FontBakedInit(ImFontAtlas* atlas, ImFontConfig* src, ImFontBaked* baked, void* loader_data_for_baked_src)
{
  return false;
}

static void ImGui_ImplDirectWrite_FontBakedDestroy(ImFontAtlas* atlas, ImFontConfig* src, ImFontBaked* baked, void* loader_data_for_baked_src)
{
}

static bool ImGui_ImplDirectWrite_FontBakedLoadGlyph(ImFontAtlas* atlas, ImFontConfig* src, ImFontBaked* baked, void* loader_data_for_baked_src, ImWchar codepoint, ImFontGlyph* out_glyph, float* out_advance_x)
{
  return false;
}

const ImFontLoader* ImGuiDirectWrite_GetFontLoader()
{
  static ImFontLoader loader = { };
  loader.Name                       = "DirectWrite";
  loader.LoaderInit                 = ImGui_ImplDirectWrite_LoaderInit;
  loader.LoaderShutdown             = ImGui_ImplDirectWrite_LoaderShutdown;
  loader.FontSrcInit                = ImGui_ImplDirectWrite_FontSrcInit;
  loader.FontSrcDestroy             = ImGui_ImplDirectWrite_FontSrcDestroy;
  loader.FontSrcContainsGlyph       = ImGui_ImplDirectWrite_FontSrcContainsGlyph;
  loader.FontBakedInit              = ImGui_ImplDirectWrite_FontBakedInit;
  loader.FontBakedDestroy           = ImGui_ImplDirectWrite_FontBakedDestroy;
  loader.FontBakedLoadGlyph         = ImGui_ImplDirectWrite_FontBakedLoadGlyph;
  loader.FontBakedSrcLoaderDataSize = sizeof(ImGui_ImplDirectWrite_FontSrcBakedData);
  return &loader;
}
