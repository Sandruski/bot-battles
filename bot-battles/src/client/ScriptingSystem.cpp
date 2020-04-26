#include "ScriptingSystem.h"

#include "GameClient.h"
#include "InputComponent.h"

namespace sand {

/*       
    py::dict m_contextDict;
    */

//----------------------------------------------------------------------------------------------------
bool ScriptingSystemClient::StartUp()
{
    py::initialize_interpreter();

    InputComponent& inputComponent = g_gameClient->GetInputComponent();

    try {
        pybind11::module botModule = py::module::import("botCharacter");
        botModule.attr("add")(&inputComponent);
    } catch (const std::runtime_error& re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error initializing sample", MB_OK);
        py::finalize_interpreter();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScriptingSystemClient::Update()
{

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScriptingSystemClient::ShutDown()
{
    py::finalize_interpreter();
    return true;
}
}
