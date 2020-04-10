#ifndef __PANEL_H__
#define __PANEL_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class Panel {
public:
    Panel();
    virtual ~Panel() = default;

    virtual std::string GetName() const = 0;

    virtual bool RenderHeader() const = 0;
    virtual bool RenderBody() const = 0;

    bool m_isEnabled;
};
}

#endif
