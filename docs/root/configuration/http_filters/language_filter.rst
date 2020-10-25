
.. _config_http_filters_language:

Language
===================

* :ref:`v3 API reference <envoy_v3_api_msg_extensions.filters.http.language.v3.Language>`
* This filter should be configured with the name *envoy.filters.http.language*.

.. attention::

  The language filter is experimental and is currently under active development.

HTTP filter which matches the language the client is able to understand using [Unicode ICU](https://github.com/unicode-org), either based on
an URL query parameter, a cookie or an Accept-Language header ([RFC 2616 Section 14.4](https://tools.ietf.org/html/rfc2616#section-14.4)).

Example configuration
---------------------

Example filter configuration:

.. code-block:: yaml

  name: envoy.filters.http.language
  typed_config:
    "@type": type.googleapis.com/envoy.extensions.filters.http.language.v3.Language
    default_language: en
    languages: [en, de, dk, es, fr, fr-ch, it, jp, ko, nl, pt, zh, zh-tw]
    query_param_name: "language"
    cookie_name: "LANGUAGE"

Statistics
----------

The language filter outputs statistics in the *http.<stat_prefix>.language.* namespace. The
:ref:`stat prefix <envoy_v3_api_field_extensions.filters.network.http_connection_manager.v3.HttpConnectionManager.stat_prefix>`
comes from the owning HTTP connection manager.

.. csv-table::
  :header: Name, Type, Description
  :widths: 1, 1, 2

  query_param, Counter, Number of requests for which the language from the URL query parameter was matched
  cookie, Counter, Number of requests for which the language from the cookie was matched
  header, Counter, Number of requests for which the language from the Accept-Language header ([RFC 2616 Section 14.4](https://tools.ietf.org/html/rfc2616#section-14.4)) was matched
  default_language, Counter, Number of requests for which the default language was used (fallback)
