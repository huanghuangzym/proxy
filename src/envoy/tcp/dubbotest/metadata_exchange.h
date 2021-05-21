/* Copyright 2019 Istio Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>

#include "common/protobuf/protobuf.h"
#include "envoy/local_info/local_info.h"
#include "envoy/network/filter.h"
#include "envoy/runtime/runtime.h"
#include "envoy/stats/scope.h"
#include "envoy/stats/stats_macros.h"
#include "envoy/stream_info/filter_state.h"
#include "common/buffer/buffer_impl.h"
#include "extensions/common/context.h"
#include "extensions/common/node_info_bfbs_generated.h"
#include "extensions/common/proto_util.h"
#include "extensions/filters/common/expr/cel_state.h"
#include "extensions/filters/network/dubbo_proxy/metadata.h"
#include "extensions/filters/network/dubbo_proxy/protocol.h"
#include "http_parser.h"


namespace Envoy {
namespace Tcp {
namespace Dubbotest {

using ::Envoy::Extensions::Filters::Common::Expr::CelStatePrototype;

/**
 * All MetadataExchange filter stats. @see stats_macros.h
 */
#define ALL_METADATA_EXCHANGE_STATS(COUNTER) \
  COUNTER(alpn_protocol_not_found)           \
  COUNTER(alpn_protocol_found)               \
  COUNTER(initial_header_not_found)          \
  COUNTER(header_not_found)                  \
  COUNTER(metadata_added)

/**
 * Struct definition for all MetadataExchange stats. @see stats_macros.h
 */
struct MetadataExchangeStats {
    ALL_METADATA_EXCHANGE_STATS(GENERATE_COUNTER_STRUCT)
};

/**
 * Direction of the flow of traffic in which this this MetadataExchange filter
 * is placed.
 */
enum FilterDirection {
    Downstream, Upstream
};


class HttpEncoder {
public:
    // @throw EnvoyException

    Buffer::Instance &response(Extensions::NetworkFilters::DubboProxy::MessageMetadataSharedPtr metadata);

private:
    void request_line(const std::string &method, const std::string &uri);

    void response_line();

    void header(const Http::LowerCaseString &name, const std::string &value);

    // TODO aconway 2018-06-20: there is a LOT of copying in message
    // encoding/decoding

    // Common headers and body
    void message(Extensions::NetworkFilters::DubboProxy::MessageMetadataSharedPtr metadata);

    // Re-use these variables, they will cache some memory between use and
    // reduce allocations.
    Buffer::OwnedImpl http_;
    std::string content_, content_type_;
};


class AmqpBuilder : public Logger::Loggable<Logger::Id::filter> {
public:
    AmqpBuilder(enum http_parser_type type);

    virtual ~AmqpBuilder() {};

    // Consume HTTP data from buffer.
    // Return true if the message is complete, false if more data is needed.
    // @throw EnvoyException
    bool parse(Buffer::Instance &http, bool end_stream);

    int code() const;

    const std::string &status() const;

protected:
    std::string url_, field_, value_, status_,body_;
    bool has_content_;
    http_parser parser_;
    Extensions::NetworkFilters::DubboProxy::MessageMetadataSharedPtr metadata_;
    Extensions::NetworkFilters::DubboProxy::ProtocolPtr protocol_;

private:
    static http_parser_settings settings_;
    bool in_value_, done_;

    // Reset to parse a new message
    void onMessageBegin();

    void onUrl(const char *data, size_t length);

    void onBody(const char *data, size_t length);

    void onStatus(const char *data, size_t length);

    void onHeadersComplete();

    void onHeaderField(const char *data, size_t length);

    void onHeaderValue(const char *data, size_t length);

    void onMessageComplete();
};


class AmqpRequestBuilder : public AmqpBuilder {
public:
    AmqpRequestBuilder() : AmqpBuilder(HTTP_REQUEST) {}

    Extensions::NetworkFilters::DubboProxy::MessageMetadataSharedPtr message();
};

class AmqpResponseBuilder : public AmqpBuilder {
public:
    AmqpResponseBuilder() : AmqpBuilder(HTTP_RESPONSE) {}

    Extensions::NetworkFilters::DubboProxy::MessageMetadataSharedPtr message();
};

/**
 * Configuration for the MetadataExchange filter.
 */
class DubboConverConfig {
public:
    DubboConverConfig(const std::string &stat_prefix,
                      const std::string &protocol,
                      const FilterDirection filter_direction,
                      Stats::Scope &scope);

    const MetadataExchangeStats &stats() { return stats_; }

    // Scope for the stats.
    Stats::Scope &scope_;
    // Stat prefix.
    const std::string stat_prefix_;
    // Expected Alpn Protocol.
    const std::string protocol_;
    // Direction of filter.
    const FilterDirection filter_direction_;
    // Stats for MetadataExchange Filter.
    MetadataExchangeStats stats_;

    static const CelStatePrototype &nodeInfoPrototype() {
        static const CelStatePrototype *const prototype = new CelStatePrototype(
                true,
                ::Envoy::Extensions::Filters::Common::Expr::CelStateType::FlatBuffers,
                ::Wasm::Common::nodeInfoSchema(),
                StreamInfo::FilterState::LifeSpan::Connection);
        return *prototype;
    }

private:
    MetadataExchangeStats generateStats(const std::string &prefix,
                                        Stats::Scope &scope) {
        return MetadataExchangeStats{
                ALL_METADATA_EXCHANGE_STATS(POOL_COUNTER_PREFIX(scope, prefix))};
    }
};

using DubboConverConfigSharedPtr = std::shared_ptr<DubboConverConfig>;

/**
 * A MetadataExchange filter instance. One per connection.
 */
class DubboConverFilter : public Network::Filter,
                          protected Logger::Loggable<Logger::Id::filter> {
public:
    DubboConverFilter(DubboConverConfigSharedPtr config,
                      const LocalInfo::LocalInfo &local_info)
            : config_(config),
              local_info_(local_info) {
        protocol_ = Extensions::NetworkFilters::DubboProxy::NamedProtocolConfigFactory::getFactory(Extensions::NetworkFilters::DubboProxy::ProtocolType::Dubbo).createProtocol(Extensions::NetworkFilters::DubboProxy::SerializationType::Hessian2);

    }

    // Network::ReadFilter
    Network::FilterStatus onData(Buffer::Instance &data,
                                 bool end_stream) override;

    Network::FilterStatus onNewConnection() override;

    Network::FilterStatus onWrite(Buffer::Instance &data,
                                  bool end_stream) override;

    void initializeReadFilterCallbacks(
            Network::ReadFilterCallbacks &callbacks) override {
        read_callbacks_ = &callbacks;
        // read_callbacks_->connection().addConnectionCallbacks(*this);
    }

    void initializeWriteFilterCallbacks(
            Network::WriteFilterCallbacks &callbacks) override {
        write_callbacks_ = &callbacks;
    }

    Network::Connection &conn() { return read_callbacks_->connection(); }

protected:
    void process(bool end_amqp, bool end_http);

    void processHttp(bool end_http);

    void processAmqp();


    void forceWrite() {
        // TODO aconway 2018-05-02: hack, need a filter-specific write() call
        // that only gets intercepted by downstream filters. This is a workaround
        // for now - send an empty write to wake up onWrite() so it can notice
        // and write buffered output data.
        conn().write(empty_, false);
    }

private:

    bool receiver_ready_{true};
    uint64_t next_id_{1};
    std::map<uint64_t, Extensions::NetworkFilters::DubboProxy::MessageMetadataSharedPtr> responses_;

    AmqpRequestBuilder request_;

    Buffer::OwnedImpl dubbo_in_, dubbo_out_, http_in_, http_out_, empty_;
    Extensions::NetworkFilters::DubboProxy::ProtocolPtr protocol_;

    HttpEncoder http_encoder_;

    // Config for MetadataExchange filter.
    DubboConverConfigSharedPtr config_;
    // LocalInfo instance.
    const LocalInfo::LocalInfo &local_info_;
    // Read callback instance.
    Network::ReadFilterCallbacks *read_callbacks_{};
    // Write callback instance.
    Network::WriteFilterCallbacks *write_callbacks_{};
    // Stores the length of proxy data that contains node metadata.

};


}  // namespace MetadataExchange
}  // namespace Tcp
}  // namespace Envoy
