/**
 * @ Author: Pierre V
 * @ Description: SampleManager.ipp
 */

template<typename Type, bool Normalize>
inline Audio::Buffer Audio::SampleManager<Type, Normalize>::LoadSampleFile(const std::string &path, const SampleSpecs &desiredSpecs, SampleSpecs &fileSpecs, bool displaySpecs)
{
    auto ext = std::filesystem::path(path).extension();
    if (ext.empty())
        throw std::logic_error("Audio::SampleManager::LoadSampleFile: Cannot deduce file extension for '" + path + "'. Use 'SampleManager::LoadSampleFileExtension' with a desired extension instead.");
    return LoadSampleFileExtension(path, ext.string(), desiredSpecs, fileSpecs, displaySpecs);
}

template<typename Type, bool Normalize>
inline Audio::Buffer Audio::SampleManager<Type, Normalize>::LoadSampleFileExtension(const std::string &path, const std::string &ext, const SampleSpecs &desiredSpecs, SampleSpecs &fileSpecs, bool displaySpecs)
{
    // BUG: sizeof(SupportedExtension) / sizeof(std::pair<int, int>) -> Triggers a warning that thinks we try to get the number of elements
    for (const Extension &extension : SupportedExtension) {
        if (ext == extension.name) {
            /** @todo Convert buffer (if needed) into the templated type */
            return extension.loadFunc(path, desiredSpecs, fileSpecs, displaySpecs);
        }
    }
    throw std::runtime_error("Audio::SampleManager::LoadSampleFileExtension: Extension file not supported: '" + ext + "'.");
}

template<typename Type, bool Normalize>
inline bool Audio::SampleManager<Type, Normalize>::WriteSampleFile(const std::string &path,const BufferView &sample)
{
    auto ext = std::filesystem::path(path).extension();
    if (ext.empty())
        throw std::logic_error("Audio::SampleManager::WriteSampleFile: Cannot deduce file extension for '" + path + "'. Use 'SampleManager::WriteSampleFileExtension' with a desired extension instead.");
    return WriteSampleFileExtension(path, sample, ext.string());
}

template<typename Type, bool Normalize>
inline bool Audio::SampleManager<Type, Normalize>::WriteSampleFileExtension(const std::string &path, const BufferView &sample, const std::string &ext)
{
    for (auto i = 0u; i < sizeof(SupportedExtension) / sizeof(std::pair<int, int>); ++i) {
        if (std::string(std::get<0>(SupportedExtension[i])) == ext) {
            /** @todo Convert buffer (if needed) into the templated type */
            return std::get<2>(SupportedExtension[i])(path, sample);
        }
    }
    throw std::runtime_error("Audio::SampleManager::WriteSampleFileExtension: Extension file not supported: '" + ext + "'.");
}
