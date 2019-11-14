#ifndef __SYSTEM_H__
#define __SYSTEM_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class System {
public:
    static SystemType GetType()
    {
        return SystemType::INVALID;
    }

public:
    System();
    virtual ~System();

    bool RegisterEntity(Entity entity);
    bool DeRegisterEntity(Entity entity);

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

    virtual bool Render()
    {
        return true;
    }

    virtual bool ShutDown()
    {
        return true;
    }

    const Signature& GetSignature();

protected:
    std::vector<Entity> m_entities;
    Signature m_signature;
};
}

#endif
