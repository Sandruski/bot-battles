#ifndef __STATE_H__
#define __STATE_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class State {
public:
    virtual ~State() = default;

    virtual const char* GetName() const = 0;

    virtual bool Enter() const
    {
        return true;
    }

    virtual bool Update() const
    {
        return true;
    }

    virtual bool RenderGui() const
    {
        return true;
    }

    virtual bool Exit() const
    {
        return true;
    }

    virtual void OnNotify(const Event& /*event*/) { }
};
}

#endif
