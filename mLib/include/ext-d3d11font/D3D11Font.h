
#ifndef EXT_D3D11FONT_D3D11FONT_H_
#define EXT_D3D11FONT_D3D11FONT_H_

#include "FW1FontWrapper/FW1FontWrapper.h"

namespace ml {

class D3D11Font : GraphicsAsset
{
public:
    D3D11Font()
    {
        m_factory = nullptr;
        m_fontWrapper = nullptr;
    }
    ~D3D11Font()
    {
        SAFE_RELEASE(m_factory);
        SAFE_RELEASE(m_fontWrapper);
    }

    void init(GraphicsDevice &g, const std::string &fontName)
    {
		m_graphics = &g.castD3D11();
        g.castD3D11().registerAsset(this);
        m_fontName = std::wstring(fontName.begin(), fontName.end());
        reset();
    }

    void drawString(GraphicsDevice &g, const std::string &text, const ml::vec2i &pos, const float fontHeight, RGBColor color) const
    {
        std::wstring wText(text.begin(), text.end());
        m_fontWrapper->DrawString(
            &g.castD3D11().getContext(),
            wText.c_str(),
            fontHeight,
            (FLOAT)pos.x,
            (FLOAT)pos.y,
            0xff000000 + 0x00010000 * color.b + 0x00000100 * color.g + 0x00000001 * color.r,// Text color, 0xAaBbGgRr
            FW1_RESTORESTATE// Flags (for example FW1_RESTORESTATE to keep context states unchanged)
            );
    }

    void drawStrings(GraphicsDevice &g, const std::vector< std::pair<std::string, RGBColor> > &text, const ml::vec2i &pos, const float fontHeight, UINT lineHeight) const
    {
        int lineIndex = 0;
        for (const auto &line : text)
        {
            drawString(g, line.first, pos + ml::vec2i(0, lineHeight * lineIndex), fontHeight, line.second);
            lineIndex++;
        }
    }

    void release()
    {
        SAFE_RELEASE(m_factory);
        SAFE_RELEASE(m_fontWrapper);
    }
    void reset()
    {
        //std::cout << () << "resetting fonts" << std::endl;
        release();
        D3D_VALIDATE(FW1CreateFactory(FW1_VERSION, &m_factory));
        D3D_VALIDATE(m_factory->CreateFontWrapper(&m_graphics->getDevice(), m_fontName.c_str(), &m_fontWrapper));
    }

private:
	D3D11GraphicsDevice *m_graphics;
    IFW1Factory *m_factory;
    IFW1FontWrapper *m_fontWrapper;
    std::wstring m_fontName;
};

}  // namespace ml

#endif  // EXT_D3D11FONT_D3D11FONT_H_
