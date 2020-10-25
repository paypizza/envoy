#include "envoy/registry/registry.h"

#include "src/extensions/filters/http/language/config.h"
#include "src/extensions/filters/http/language/filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Language {

Http::FilterFactoryCb LanguageFilterFactory::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::language::v3::Language& proto_config,
    const std::string& stats_prefix,
    Server::Configuration::FactoryContext& context) {
  auto filter_config = std::make_shared<FilterConfigImpl>(proto_config, stats_prefix, context.scope());

  return [filter_config](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    auto filter = new Filter(filter_config);
    callbacks.addStreamDecoderFilter(Http::StreamDecoderFilterSharedPtr{filter});
  };
}

/**
 * Static registration for the language filter. @see RegisterFactory.
 */
REGISTER_FACTORY(LanguageFilterFactory,
                 Server::Configuration::NamedHttpFilterConfigFactory);

} // namespace Language
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
