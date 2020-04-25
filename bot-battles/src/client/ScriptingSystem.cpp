#include "ScriptingSystem.h"

namespace sand {

/*       
    py::dict m_contextDict;
    */

//----------------------------------------------------------------------------------------------------
bool ScriptingSystemClient::StartUp()
{
    py::initialize_interpreter();

    try {
        pybind11::module botModule = py::module::import("botCharacter");
        auto result = botModule.attr("add")(3, 3);
        int ree = result.cast<int>();
        assert(ree == 6);
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
