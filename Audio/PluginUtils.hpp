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


#define REGISTER_PLUGIN(Name, Description, Flags, Tags, ...) \
private: \
    static inline const Audio::PluginMetaData _MetaData = [] { \
        return Audio::PluginMetaData { \
            Audio::TranslationMetaData { Name, Description }, \
            Audio::ControlMetaDataList { ADD_PREFIX_COMMA_EACH(_REGISTER_METADATA_, __VA_ARGS__) }, \
            Flags, \
            Tags \
        }; \
    }(); \
    std::array<Audio::ParamValue, VA_ARGC(__VA_ARGS__)> _controls { ADD_PREFIX_COMMA_EACH(_INIT_, __VA_ARGS__) }; \
public: \
    static const Audio::PluginMetaData &MetaData(void) noexcept { return _MetaData; } \
    static constexpr std::size_t ControlCount = VA_ARGC(__VA_ARGS__); \
    [[nodiscard]] static inline const Audio::PluginMetaData &GetMetaData(void) noexcept { return _MetaData; } \
    [[nodiscard]] const Audio::PluginMetaData &getMetaData(void) const noexcept final { return GetMetaData(); } \
    [[nodiscard]] Audio::ParamValue &getControl(const Audio::ParamID id) noexcept final { return _controls[id]; } \
    [[nodiscard]] Audio::ParamValue getControl(const Audio::ParamID id) const noexcept final { return _controls[id]; } \
    REGISTER_GETTER_CONTROL_EACH(_REGISTER_GETTER_, __VA_ARGS__) \
    REGISTER_GETTER_CONTROL_EACH(_REGISTER_GETTER_REF_, __VA_ARGS__) \
    REGISTER_SEND_CONTROL(__VA_ARGS__) \
private:


#define REGISTER_CONTROL(Type, Variable, Range, Value, Name, Description) CONTROL(Type, Variable, Range, Value, Name, Description)
#define REGISTER_CONTROL_ENUM(Variable, Range, Name, Description) CONTROL_ENUM(Variable, Range, Name, Description)

#define _REGISTER_METADATA_CONTROL(Type, Variable, Range, Value, Name, Description) \
    Audio::ControlMetaData { \
        Audio::TranslationMetaData { Name, Description }, \
        Audio::ParamType::Type, \
        Value, \
        Range, \
        {} \
    }

#define _REGISTER_METADATA_CONTROL_ENUM(Variable, Range, Name, Description) \
    Audio::ControlMetaData { \
        Audio::TranslationMetaData { Name, Description }, \
        Audio::ParamType::Enum, \
        0.0, \
        {}, \
        Range \
    }

#define _INIT_CONTROL(Type, Variable, Range, Value, Name, Description) Value

#define _INIT_CONTROL_ENUM(Variable, Range, Name, Description) 0.0

#define _REGISTER_GETTER_REF_CONTROL(Type, Variable, Range, Value, Name, Description) \
    [[nodiscard]] Audio::ParamValue &Variable(void) noexcept { return _controls

#define _REGISTER_GETTER_CONTROL(Type, Variable, Range, Value, Name, Description) \
    [[nodiscard]] Audio::ParamValue Variable(void) const noexcept { return _controls

#define _REGISTER_GETTER_REF_CONTROL_ENUM(Variable, Range, Name, Description)  \
    [[nodiscard]] Audio::ParamValue &Variable(void) noexcept { return _controls

#define _REGISTER_GETTER_CONTROL_ENUM(Variable, Range, Name, Description) \
    [[nodiscard]] Audio::ParamValue Variable(void) const noexcept { return _controls

#define CONTROL_RANGE(Min, Max) (Audio::ControlMetaData::RangeValues { Min, Max, 0.0 })
#define CONTROL_RANGE_STEP(Min, Max, Step) (Audio::ControlMetaData::RangeValues { Min, Max, Step })
#define CONTROL_ENUM_RANGE(...) (Audio::ControlMetaData::RangeNames { __VA_ARGS__ })

#define TR_TABLE(...) (Audio::TranslationTable { __VA_ARGS__ })
#define TR(Lang, Text) (Audio::TranslationPair { Lang, Text })

#define _FORWARD_TAG(tag) Audio::IPluginFactory::Tags::tag
#define TAGS(...) (Audio::IPluginFactory::Tags { Audio::MakeFlags<Audio::IPluginFactory::Tags, std::uint32_t>(FOR_COMMA_EACH(_FORWARD_TAG, __VA_ARGS__)) })

#define _FORWARD_FLAG(flag) Audio::IPluginFactory::Flags::flag
#define FLAGS(...) (Audio::IPluginFactory::Flags { Audio::MakeFlags<Audio::IPluginFactory::Flags, std::uint32_t>(FOR_COMMA_EACH(_FORWARD_FLAG, __VA_ARGS__)) })

#define _TR_DUMMY_TABLE TR_TABLE(TR({},{}))
#define _REGISTER_DUMMY_CONTROL REGISTER_CONTROL(dummy, _TR_DUMMY_TABLE, _TR_DUMMY_TABLE)
#define _REGISTER_DUMMY_PLUGIN REGISTER_PLUGIN(_TR_DUMMY_TABLE, _TR_DUMMY_TABLE, _REGISTER_DUMMY_CONTROL)