#ifndef __STATE_H__
#define __STATE_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class State {
public:
    virtual ~State() = default;

    virtual const char* GetName() = 0;

    virtual bool Enter()
    {
        return true;
    }

    virtual bool PreUpdate()
    {
        return true;
    }

    virtual bool Update()
    {
        return true;
    }

    virtual bool PostUpdate()
    {
        return true;
    }

    virtual bool Exit()
    {
        return true;
    }
};
}

#endif
