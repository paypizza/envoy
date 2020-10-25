#include "test/integration/http_integration.h"

namespace Envoy {
class HttpFilterLanguageIntegrationTest : public testing::TestWithParam<Network::Address::IpVersion>,
                                          public HttpIntegrationTest {
public:
  HttpFilterLanguageIntegrationTest()
      : HttpIntegrationTest(Http::CodecClient::Type::HTTP2, GetParam(), realTime()) {}

  void TearDown() override { fake_upstream_connection_.reset(); }

  void setupLanguageFilter(std::string query_param_name = DefaultQueryParamName,
                           std::string cookie_name = DefaultCookieName) {
    const std::string filter =
    R"EOF(
      name: envoy.filters.http.language
      typed_config:
        "@type": type.googleapis.com/envoy.extensions.filters.http.language.v3.Language
        default_language: {}
        languages: [en, de, dk, es, fr, fr-ch, it, jp, ko, nl, pt, zh, zh-tw]
        query_param_name: {}
        cookie_name: {}
    )EOF";
    config_helper_.addFilter(fmt::format(filter, DefaultLanguage, query_param_name, cookie_name));
  }

  inline static const std::string DefaultLanguage = "en";

  inline static const std::string DefaultQueryParamName = "language";

  inline static const std::string DefaultCookieName = "LANGUAGE";
};

INSTANTIATE_TEST_SUITE_P(IpVersions, HttpFilterLanguageIntegrationTest,
                         testing::ValuesIn(TestEnvironment::getIpVersionsForTest()),
                         TestUtility::ipTestParamsToString);

TEST_P(HttpFilterLanguageIntegrationTest, DefaultLanguageFallback) {
  setupLanguageFilter();
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ(HttpFilterLanguageIntegrationTest::DefaultLanguage,
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}

TEST_P(HttpFilterLanguageIntegrationTest, ValidQueryParameter) {
  setupLanguageFilter();
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", fmt::format("/?{}=FR", HttpFilterLanguageIntegrationTest::DefaultQueryParamName)},
      {":scheme", "http"},
      {":authority", "host"}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ("fr",
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}

TEST_P(HttpFilterLanguageIntegrationTest, MatchQueryParameter) {
  setupLanguageFilter();
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", fmt::format("/?{}=fr-be", HttpFilterLanguageIntegrationTest::DefaultQueryParamName)},
      {":scheme", "http"},
      {":authority", "host"}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ("fr",
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}

TEST_P(HttpFilterLanguageIntegrationTest, InvalidQueryParameter) {
  setupLanguageFilter();
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", fmt::format("/?{}=foo", HttpFilterLanguageIntegrationTest::DefaultQueryParamName)},
      {":scheme", "http"},
      {":authority", "host"}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ(HttpFilterLanguageIntegrationTest::DefaultLanguage,
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}

TEST_P(HttpFilterLanguageIntegrationTest, IgnoreQueryParameter) {
  setupLanguageFilter(std::string(), HttpFilterLanguageIntegrationTest::DefaultCookieName);
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", fmt::format("/?{}=fr-be", HttpFilterLanguageIntegrationTest::DefaultQueryParamName)},
      {":scheme", "http"},
      {":authority", "host"}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ(HttpFilterLanguageIntegrationTest::DefaultLanguage,
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}

TEST_P(HttpFilterLanguageIntegrationTest, ValidCookie) {
  setupLanguageFilter();
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"cookie", fmt::format("{}=es; Expires=Wed, 09 Jun 2040 10:18:14 GMT", HttpFilterLanguageIntegrationTest::DefaultCookieName)}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ("es",
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}

TEST_P(HttpFilterLanguageIntegrationTest, InvalidCookie) {
  setupLanguageFilter();
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"cookie", fmt::format("{}=bar; Expires=Wed, 09 Jun 2040 10:18:14 GMT", HttpFilterLanguageIntegrationTest::DefaultCookieName)}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ(HttpFilterLanguageIntegrationTest::DefaultLanguage,
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}

TEST_P(HttpFilterLanguageIntegrationTest, IgnoreCookie) {
  setupLanguageFilter(HttpFilterLanguageIntegrationTest::DefaultQueryParamName, std::string());
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"cookie", fmt::format("{}=es; Expires=Wed, 09 Jun 2040 10:18:14 GMT", HttpFilterLanguageIntegrationTest::DefaultCookieName)}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ(HttpFilterLanguageIntegrationTest::DefaultLanguage,
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}

TEST_P(HttpFilterLanguageIntegrationTest, AcceptLanguageHeader) {
  setupLanguageFilter();
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
      {":method", "GET"},
      {":path", "/"},
      {":scheme", "http"},
      {":authority", "host"},
      {"accept-language", "fr-CH,fr;q=0.9,en;q=0.8,de;q=0.7,*;q=0.5"}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ("fr-CH",
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}

TEST_P(HttpFilterLanguageIntegrationTest, InvalidAcceptLanguageHeader) {
  setupLanguageFilter();
  HttpIntegrationTest::initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));
  auto response = codec_client_->makeHeaderOnlyRequest(
    Http::TestRequestHeaderMapImpl{
        {":method", "GET"},
        {":path", "/"},
        {":scheme", "http"},
        {":authority", "host"},
        {"accept-language", "foobar;;;;0000.20;0-"}
    }
  );
  ASSERT_TRUE(fake_upstreams_[0]->waitForHttpConnection(*dispatcher_, fake_upstream_connection_));
  ASSERT_TRUE(fake_upstream_connection_->waitForNewStream(*dispatcher_, upstream_request_));
  ASSERT_TRUE(upstream_request_->waitForEndStream(*dispatcher_));
  response->waitForEndStream();
  EXPECT_TRUE(response->complete());

  EXPECT_EQ(HttpFilterLanguageIntegrationTest::DefaultLanguage,
               upstream_request_->headers().get(Http::LowerCaseString("X-Language"))->value().getStringView());

  codec_client_->close();
  ASSERT_TRUE(fake_upstream_connection_->close());
  ASSERT_TRUE(fake_upstream_connection_->waitForDisconnect());
}
} // namespace Envoy
