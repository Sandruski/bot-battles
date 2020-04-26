#ifndef __SCRIPTING_SYSTEM_CLIENT_H__
#define __SCRIPTING_SYSTEM_CLIENT_H__

#include "System.h"

#include <embed.h>
#include <pybind11.h>

namespace py = pybind11;

namespace sand {

//----------------------------------------------------------------------------------------------------
class ScriptingSystemClient : public System, public Subject, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::SCRIPTING;
    }

public:
    bool StartUp() override;
    bool Update() override;
    bool ShutDown() override;

    void OnNotify(const Event& event) override;

private:
    void ImportScript() const;
};
}

#endif
