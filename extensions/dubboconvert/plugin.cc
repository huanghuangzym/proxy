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

#include "extensions/dubboconvert/plugin.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "extensions/common/context.h"
#include "extensions/common/proto_util.h"
#include "extensions/common/wasm/json_util.h"

#ifndef NULL_PLUGIN

#include "extensions/common/wasm/base64.h"
#include "extensions/metadata_exchange/declare_property.pb.h"

#else

#include "common/common/base64.h"
#include "source/extensions/common/wasm/ext/declare_property.pb.h"

namespace proxy_wasm {
namespace null_plugin {
namespace DubboConvert {
namespace Plugin {

PROXY_WASM_NULL_PLUGIN_REGISTRY;

using Base64 = Envoy::Base64;

#endif

static RegisterContextFactory register_MetadataExchange(
    CONTEXT_FACTORY(PluginContext), ROOT_FACTORY(PluginRootContext));

void PluginRootContext::updateMetadataValue() {
  auto node_info = ::Wasm::Common::extractLocalNodeFlatBuffer();

  google::protobuf::Struct metadata;
  ::Wasm::Common::extractStructFromNodeFlatBuffer(
      *flatbuffers::GetRoot<::Wasm::Common::FlatNode>(node_info.data()),
      &metadata);

  std::string metadata_bytes;
  ::Wasm::Common::serializeToStringDeterministic(metadata, &metadata_bytes);
  metadata_value_ =
      Base64::encode(metadata_bytes.data(), metadata_bytes.size());
}



bool PluginRootContext::configure(size_t configuration_size) {
  auto configuration_data = getBufferBytes(WasmBufferType::PluginConfiguration,
                                           0, configuration_size);
  // Parse configuration JSON string.
  auto result = ::Wasm::Common::JsonParse(configuration_data->view());
  if (!result.has_value()) {
    LOG_WARN(absl::StrCat("cannot parse plugin configuration JSON string: ",
                          configuration_data->view()));
    return false;
  }

  auto j = result.value();
  auto max_peer_cache_size_field =
      ::Wasm::Common::JsonGetField<int64_t>(j, "max_peer_cache_size");
  if (max_peer_cache_size_field.detail() ==
      Wasm::Common::JsonParserResultDetail::OK) {
    max_peer_cache_size_ = max_peer_cache_size_field.value();
  }
  return true;
}

bool PluginRootContext::updatePeer(std::string_view key,
                                   std::string_view peer_id,
                                   std::string_view peer_header) {
  std::string id = std::string(peer_id);
  if (max_peer_cache_size_ > 0) {
    auto it = cache_.find(id);
    if (it != cache_.end()) {
      setFilterState(key, it->second);
      return true;
    }
  }

  auto bytes = Base64::decodeWithoutPadding(peer_header);
  google::protobuf::Struct metadata;
  if (!metadata.ParseFromString(bytes)) {
    return false;
  }

  auto fb = ::Wasm::Common::extractNodeFlatBufferFromStruct(metadata);
  std::string_view out(reinterpret_cast<const char*>(fb.data()), fb.size());
  setFilterState(key, out);

  if (max_peer_cache_size_ > 0) {
    // do not let the cache grow beyond max cache size.
    if (static_cast<uint32_t>(cache_.size()) > max_peer_cache_size_) {
      auto it = cache_.begin();
      cache_.erase(cache_.begin(), std::next(it, max_peer_cache_size_ / 4));
      LOG_DEBUG(absl::StrCat("cleaned cache, new cache_size:", cache_.size()));
    }
    cache_.emplace(std::move(id), out);
  }

  return true;
}

FilterHeadersStatus PluginContext::onRequestHeaders(uint32_t, bool) {
  // strip and store downstream peer metadata


    LOG_DEBUG("PluginContext::onRequestHeaders   dubbo-testhl");

  return FilterHeadersStatus::Continue;
}

FilterHeadersStatus PluginContext::onResponseHeaders(uint32_t, bool) {
    LOG_DEBUG("PluginContext::onResponseHeaders   dubbo-testhl");
  return FilterHeadersStatus::Continue;
}

#ifdef NULL_PLUGIN
}  // namespace Plugin
}  // namespace MetadataExchange
}  // namespace null_plugin
}  // namespace proxy_wasm
#endif
