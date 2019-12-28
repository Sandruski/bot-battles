#ifndef __SYSTEM_H__
#define __SYSTEM_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class System {
public:
    System();
    virtual ~System() = default;

    virtual bool StartUp()
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

    virtual bool PreRender()
    {
        return true;
    }

    virtual bool Render()
    {
        return true;
    }

    virtual bool PostRender()
    {
        return true;
    }

    virtual bool ShutDown()
    {
        return true;
    }

    bool RegisterEntity(Entity entity);
    bool DeRegisterEntity(Entity entity);

    Signature GetSignature();

protected:
    std::vector<Entity> m_entities;
    Signature m_signature;
};
}

#endif
