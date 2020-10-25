#pragma once

#include "extensions/filters/http/common/factory_base.h"
#include "extensions/filters/http/well_known_names.h"
#include "src/extensions/filters/http/language/language.pb.h"
#include "src/extensions/filters/http/language/language.pb.validate.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Language {

/**
 * Config registration for the language filter. @see NamedHttpFilterConfigFactory.
 */
class LanguageFilterFactory
    : public Common::FactoryBase<envoy::extensions::filters::http::language::v3::Language> {
public:
  LanguageFilterFactory() : FactoryBase("envoy.filters.http.language") {}

  Http::FilterFactoryCb createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::http::language::v3::Language& proto_config,
    const std::string& stat_prefix, Server::Configuration::FactoryContext& context) override;
};

} // namespace Language
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
