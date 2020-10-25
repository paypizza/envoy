#pragma once

#include <string>

#include "unicode/localematcher.h"

#include "common/common/logger.h"
#include "src/extensions/filters/http/language/language.pb.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Language {

/**
 * All stats for the language filter. @see stats_macros.h
 */
// clang-format off
#define LANGUAGE_FILTER_STATS(COUNTER) \
  COUNTER(query_param)                 \
  COUNTER(cookie)                      \
  COUNTER(header)                      \
  COUNTER(default_language)
// clang-format on

/**
 * Wrapper struct filter stats. @see stats_macros.h
 */
struct FilterStats {
  LANGUAGE_FILTER_STATS(GENERATE_COUNTER_STRUCT)
};

/**
 * Abstract filter configuration.
 */
class FilterConfig {
public:
  virtual ~FilterConfig() = default;

  virtual FilterStats& stats() PURE;

  virtual const icu::Locale& default_language() const PURE;
  virtual const std::string& query_param_name() const PURE;
  virtual const std::string& cookie_name() const PURE;

  virtual const std::vector<icu::Locale>& languages() const PURE;
};

/**
 * Configuration for the language filter.
 */
class FilterConfigImpl : public FilterConfig {
public:
  FilterConfigImpl(const envoy::extensions::filters::http::language::v3::Language& proto_config,
                   const std::string& stats_prefix,
                   Stats::Scope& scope);

  FilterStats& stats() override;

  const icu::Locale& default_language() const override;
  const std::string& query_param_name() const override;
  const std::string& cookie_name() const override;

  const std::vector<icu::Locale>& languages() const override;

private:
  FilterStats stats_;

  const icu::Locale default_language_;
  const std::string query_param_name_;
  const std::string cookie_name_;

  std::vector<icu::Locale> languages_;
};

class Filter : public Http::StreamDecoderFilter,
               Logger::Loggable<Logger::Id::filter> {
public:
  Filter(const std::shared_ptr<FilterConfig>& config);

  static FilterStats generateStats(const std::string& prefix, Stats::Scope& scope);

  const Http::LowerCaseString Language{"x-language"};
  const Http::LowerCaseString AcceptLanguage{"accept-language"};

  // Http::StreamFilterBase
  void onDestroy() override;

  // Http::StreamDecoderFilter
  Http::FilterHeadersStatus decodeHeaders(Http::RequestHeaderMap&, bool) override;
  Http::FilterDataStatus decodeData(Buffer::Instance&, bool) override;
  Http::FilterTrailersStatus decodeTrailers(Http::RequestTrailerMap&) override;
  void setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks&) override;

private:
  std::shared_ptr<FilterConfig> config_;

  Http::StreamDecoderFilterCallbacks* decoder_callbacks_{};

  std::shared_ptr<icu::LocaleMatcher> matcher_;

  std::string matchValue(icu::Locale&);
};

} // namespace Language
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
