/**
 * @ Author: Pierre V
 * @ Description: SampleManager.ipp
 */

template<typename Type>
inline Audio::Buffer Audio::SampleManager<Type>::LoadSampleFile(const std::string &path, SampleSpecs &specs, bool displaySpecs)
{
    auto ext = std::filesystem::path(path).extension();
    if (ext.empty())
        throw std::logic_error("Audio::SampleManager::LoadSampleFile: Cannot deduce file extension for '" + path + "'. Use 'SampleManager::LoadSampleFileExtension' with a desired extension instead.");
    return LoadSampleFileExtension(path, ext.string(), specs, displaySpecs);
}

template<typename Type>
inline Audio::Buffer Audio::SampleManager<Type>::LoadSampleFileExtension(const std::string &path, const std::string &ext, SampleSpecs &specs, bool displaySpecs)
{
    for (auto i = 0; i < sizeof(SupportedExtension) / sizeof(std::pair<int, int>); ++i) {
        if (std::string(std::get<0>(SupportedExtension[i])) == ext) {
            /** @todo Convert buffer (if needed) into the templated type */
            return std::get<1>(SupportedExtension[i])(path, specs, displaySpecs);
        }
    }
    throw std::runtime_error("Audio::SampleManager::LoadSampleFileExtension: Extension file not supported: '" + ext + "'.");
}
