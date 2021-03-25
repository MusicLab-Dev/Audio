/*
 * @ Author: Pierre Veysseyre
 * @ Description: PluginTable
 */

#pragma once

#include <Core/Assert.hpp>
#include <Core/Vector.hpp>

#include "PluginPtr.hpp"

namespace Audio
{
    class PluginTable;

    /** @brief Vector of plugin factories */
    using PluginFactories = Core::TinyVector<PluginFactoryPtr>;

    /** @brief Vector of plugin pointers */
    using Plugins = Core::TinyVector<IPlugin *>;

    /** @brief Vector of reference counters */
    using RefCounts = Core::TinyVector<std::uint32_t>;
};

class alignas_cacheline Audio::PluginTable
{
public:
    class Instance
    {
    public:
        Instance(void) { PluginTable::Init(); }
        ~Instance(void) { PluginTable::Destroy(); }
    };

    /** @brief Initialize unique instance */
    static void Init(void) { _Instance.reset(new PluginTable()); }

    /** @brief Destroy unique instance */
    static void Destroy(void) { _Instance.reset(); }

    /** @brief Return the unique plugin table instance */
    [[nodiscard]] static PluginTable &Get(void) noexcept { return *_Instance; }


    /** @brief Update the audio parameters of the whole plugin table */
    void updateAudioSpecs(const AudioSpecs &audioSpecs);

    /** @brief Register a factory using a path */
    IPluginFactory &registerFactory(const std::string &path);

    /** @todo Change Name template parameter by std::string_view ? */
    /** @brief Register a factory using a compiled plugin */
    template<typename Type>
    IPluginFactory &registerFactory(void);

    /** @brief Instantiates a new plugin using its factory path */
    [[nodiscard]] PluginPtr instantiate(const std::string_view &path);

    /** @brief Instantiates a new plugin using its factory */
    [[nodiscard]] PluginPtr instantiate(IPluginFactory &factory);


    /** @brief Get a reference of the plugin factories associated to the table */
    [[nodiscard]] PluginFactories &factories(void) noexcept { return _factories; }

    /** @brief Get a constant reference to the plugin factories associated to the table */
    [[nodiscard]] const PluginFactories &factories(void) const noexcept { return _factories; }

    /** @brief Find a factory by path */
    [[nodiscard]] IPluginFactory *find(const std::string_view &path) noexcept
        { return const_cast<IPluginFactory *>(const_cast<const PluginTable *>(this)->find(path)); }
    [[nodiscard]] const IPluginFactory *find(const std::string_view &path) const noexcept;


public: // Functions reserved to internal usage
    /** @brief Register a plugin into the table and icrement its ref count */
    void addPlugin(IPlugin *plugin) noexcept_ndebug;

    /** @brief Increment reference count of a plugin */
    void incrementRefCount(IPlugin *plugin) noexcept_ndebug;

    /** @brief Decrement reference count of a plugin */
    void decrementRefCount(IPlugin *plugin) noexcept_ndebug;

private:
    static inline std::unique_ptr<PluginTable> _Instance {};

    PluginFactories     _factories {};
    Plugins             _instances {};
    RefCounts           _counters {};

    /** @brief Private constructor */
    PluginTable(void);

    /** @brief Deleted semantics */
    PluginTable(const PluginTable &other) noexcept = delete;
    PluginTable(PluginTable &&other) noexcept = delete;
    PluginTable &operator=(const PluginTable &other) noexcept = delete;
    PluginTable &operator=(PluginTable &&other) noexcept = delete;
};

static_assert_fit_cacheline(Audio::PluginTable);

#include "PluginPtr.ipp"

#include "PluginTable.ipp"