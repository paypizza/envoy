#include <string>

#include "unicode/utypes.h"
#include "unicode/locid.h"

#include "common/common/empty_string.h"
#include "common/common/logger.h"
#include "common/common/utility.h"
#include "common/http/utility.h"
#include "src/extensions/filters/http/language/filter.h"

namespace Envoy {
namespace Extensions {
namespace HttpFilters {
namespace Language {

FilterConfigImpl::FilterConfigImpl(
  const envoy::extensions::filters::http::language::v3::Language& config,
  const std::string& stats_prefix,
  Stats::Scope& scope)
    : stats_(Filter::generateStats(stats_prefix, scope)),
      default_language_(config.default_language().data()),
      query_param_name_(config.query_param_name().data()),
      cookie_name_(config.cookie_name().data()) {
  for (const auto& header : config.languages()) {
    languages_.push_back(icu::Locale(header.data()));
  }
}

FilterStats& FilterConfigImpl::stats() { return stats_; }

const icu::Locale& FilterConfigImpl::default_language() const { return default_language_; }
const std::string& FilterConfigImpl::query_param_name() const { return query_param_name_; }
const std::string& FilterConfigImpl::cookie_name() const { return cookie_name_; }

const std::vector<icu::Locale>& FilterConfigImpl::languages() const { return languages_; }

Filter::Filter(const std::shared_ptr<FilterConfig>& config)
  : config_(std::move(config)) {
  UErrorCode errorCode = U_ZERO_ERROR;

  matcher_ = std::make_shared<icu::LocaleMatcher>(icu::LocaleMatcher::Builder()
    .setSupportedLocales(config_->languages().begin(), config_->languages().end())
    .setDefaultLocale(&config_->default_language())
    .build(errorCode));
}

void Filter::onDestroy() {}

FilterStats Filter::generateStats(const std::string& prefix, Stats::Scope& scope) {
  const std::string final_prefix = prefix + "language.";
  return {LANGUAGE_FILTER_STATS(POOL_COUNTER_PREFIX(scope, final_prefix))};
}

Http::FilterHeadersStatus Filter::decodeHeaders(Http::RequestHeaderMap& headers, bool) {
  if (!config_->query_param_name().empty()) {
    // Get the URL query parameter
    Http::Utility::QueryParams params = Http::Utility::parseQueryString(headers.Path()->value().getStringView());

    if (!params.empty()) {
      absl::optional<std::string> param_value = (params.find(config_->query_param_name()) != params.end()) ?
      absl::optional<std::string>{params.at(config_->query_param_name())} :
      absl::nullopt;

      if (param_value.has_value()) {
        const std::string& value = param_value.value();
        icu::Locale locale = icu::Locale(value.data());
        std::string language_tag = matchValue(locale);

        if (!language_tag.empty()) {
          headers.addCopy(Language, language_tag);
          config_->stats().query_param_.inc();

          return Http::FilterHeadersStatus::Continue;
        }
      }
    }
  }

  if (!config_->cookie_name().empty()) {
    // Get the cookie
    const std::string value = Http::Utility::parseCookieValue(headers, config_->cookie_name());
    if (!value.empty()) {
      icu::Locale locale = icu::Locale(value.data());
      std::string language_tag = matchValue(locale);

      if (!language_tag.empty()) {
        headers.addCopy(Language, language_tag);
        config_->stats().cookie_.inc();

        return Http::FilterHeadersStatus::Continue;
      }
    }
  }

  // Get the accept language header
  const Http::HeaderEntry* entry = headers.get(AcceptLanguage);
  if (entry != nullptr) {
    absl::string_view value_str = entry->value().getStringView();

    if (!value_str.empty()) {
      std::string value = std::string(value_str);

      UErrorCode errorCode = U_ZERO_ERROR;
      const icu::Locale *result = matcher_->getBestMatchForListString(value, errorCode);

      if (U_SUCCESS(errorCode)) {
        std::string language_tag = result->toLanguageTag<std::string>(errorCode).data();

        if (U_SUCCESS(errorCode)) {
          if (!language_tag.empty()) {
            headers.addCopy(Language, language_tag);
            config_->stats().header_.inc();

            return Http::FilterHeadersStatus::Continue;
          }
        }
      }
    }
  }

  // Default language fallback
  headers.addCopy(Language, config_->default_language().getLanguage());
  config_->stats().default_language_.inc();

  return Http::FilterHeadersStatus::Continue;
}

Http::FilterDataStatus Filter::decodeData(Buffer::Instance&, bool) {
  return Http::FilterDataStatus::Continue;
}

Http::FilterTrailersStatus Filter::decodeTrailers(Http::RequestTrailerMap&) {
  return Http::FilterTrailersStatus::Continue;
}

void Filter::setDecoderFilterCallbacks(Http::StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

std::string Filter::matchValue(icu::Locale& locale) {
  UErrorCode errorCode = U_ZERO_ERROR;
  const icu::Locale *result = matcher_->getBestMatch(locale, errorCode);

  if (U_SUCCESS(errorCode)) {
    auto language_tag = result->toLanguageTag<std::string>(errorCode);

    if (U_SUCCESS(errorCode)) {
      return language_tag.data();
    } else {
      ENVOY_LOG(error, "ICU error code toLanguageTag: {}", errorCode);
    }
  } else {
    ENVOY_LOG(error, "ICU error code getBestMatch: {}", errorCode);
  }

  return EMPTY_STRING;
}

} // namespace Language
} // namespace HttpFilters
} // namespace Extensions
} // namespace Envoy
