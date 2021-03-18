/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugins Macro Utils
 */

#include <array>

#include <Core/MacroUtils.hpp>

#include "IPlugin.hpp"

#define REGISTER_GETTER_CONTROL_EACH(prefix, ...)        _REGISTER_GETTER_CONTROL_EACH(prefix, VA_ARGC(__VA_ARGS__), __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH(prefix, N, ...)    _CONCATENATE(_REGISTER_GETTER_CONTROL_EACH_, N)(prefix, 0 __VA_OPT__(,) __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_0()
#define _REGISTER_GETTER_CONTROL_EACH_1(prefix, i, x)       _CONCATENATE_SECOND(prefix, x) [i]; }
#define _REGISTER_GETTER_CONTROL_EACH_2(prefix, i, x, ...)  _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_1(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_3(prefix, i, x, ...)  _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_2(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_4(prefix, i, x, ...)  _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_3(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_5(prefix, i, x, ...)  _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_4(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_6(prefix, i, x, ...)  _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_5(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_7(prefix, i, x, ...)  _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_6(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_8(prefix, i, x, ...)  _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_7(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_9(prefix, i, x, ...)  _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_8(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_10(prefix, i, x, ...) _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_9(prefix, i + 1, ___VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_11(prefix, i, x, ...) _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_10(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_12(prefix, i, x, ...) _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_11(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_13(prefix, i, x, ...) _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_12(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_14(prefix, i, x, ...) _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_13(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_15(prefix, i, x, ...) _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_14(prefix, i + 1, __VA_ARGS__)
#define _REGISTER_GETTER_CONTROL_EACH_16(prefix, i, x, ...) _CONCATENATE_SECOND(prefix, x) [i]; } _REGISTER_GETTER_CONTROL_EACH_15(prefix, i + 1, __VA_ARGS__)

#define REGISTER_SEND_CONTROL(...) \
    virtual void sendControls(const Audio::ControlEvents &controls) noexcept { \
        for (auto &control : controls) { \
            _controls[control.paramID] = control.value; \
        } \
    }


#define REGISTER_PLUGIN(Name, Description, ...) \
private: \
    static inline const Audio::PluginMetaData _MetaData = [] { \
        return Audio::PluginMetaData { \
            Audio::TranslationMetaData { Name, Description }, \
            Audio::ControlMetaDataList { ADD_PREFIX_COMMA_EACH(_REGISTER_METADATA_, __VA_ARGS__) } \
        }; \
    }(); \
    std::array<Audio::ParamValue, VA_ARGC(__VA_ARGS__)> _controls; \
public: \
    static constexpr std::size_t ControlCount = VA_ARGC(__VA_ARGS__); \
    [[nodiscard]] static inline const Audio::PluginMetaData &GetMetaData(void) noexcept { return _MetaData; } \
    [[nodiscard]] const Audio::PluginMetaData &getMetaData(void) const noexcept final { return GetMetaData(); } \
    [[nodiscard]] Audio::ParamValue &getControl(const Audio::ParamID id) noexcept final { return _controls[id]; } \
    [[nodiscard]] Audio::ParamValue getControl(const Audio::ParamID id) const noexcept final { return _controls[id]; } \
    REGISTER_GETTER_CONTROL_EACH(_REGISTER_GETTER_, __VA_ARGS__) \
    REGISTER_GETTER_CONTROL_EACH(_REGISTER_GETTER_REF_, __VA_ARGS__) \
    REGISTER_SEND_CONTROL(__VA_ARGS__) \
private:


#define REGISTER_CONTROL(Variable, Name, Description) CONTROL(Variable, Name, Description)

#define _REGISTER_METADATA_CONTROL(Variable, Name, Description) \
    Audio::ControlMetaData { Audio::TranslationMetaData { Name, Description } }


#define _REGISTER_GETTER_REF_CONTROL(Variable, Name, Description) \
    [[nodiscard]] Audio::ParamValue &Variable(void) noexcept { return _controls

#define _REGISTER_GETTER_CONTROL(Variable, Name, Description) \
    [[nodiscard]] Audio::ParamValue Variable(void) const noexcept { return _controls

#define TR_TABLE(...) (Audio::TranslationTable { __VA_ARGS__ })
#define TR(Lang, Text) (Audio::TranslationPair { Lang, Text })

#define _TR_DUMMY_TABLE TR_TABLE(TR({},{}))
#define _REGISTER_DUMMY_CONTROL REGISTER_CONTROL(dummy, _TR_DUMMY_TABLE, _TR_DUMMY_TABLE)
#define _REGISTER_DUMMY_PLUGIN REGISTER_PLUGIN(_TR_DUMMY_TABLE, _TR_DUMMY_TABLE, _REGISTER_DUMMY_CONTROL)